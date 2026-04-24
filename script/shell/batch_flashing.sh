#!/bin/bash
set -o pipefail

# ==================== 配置 ====================
HEX_DIR="./config/OTA_BAG"
SN_MAP_FILE="./config/OTA_BAG/mcu_bl_HEX/sn_map.json"  # 编号映射配置文件
TIMEOUT=10                   # 烧录超时秒数
# ==============================================

# ==============================================
# 检查是否安装 stlink-tools，没有则自动安装
# ==============================================
check_and_install_stlink() {
    if ! command -v st-info &> /dev/null || ! command -v st-flash &> /dev/null; then
        echo "⚠️  未检测到 stlink-tools，正在自动安装..."
        sudo apt update
        sudo apt install -y stlink-tools
        echo "✅ 安装完成！"
    fi
}

check_and_install_stlink

# ==============================================
# 读取 SN 编号映射
# ==============================================
get_sn_number() {
    local sn="$1"
    jq -r --arg sn "$sn" '.[$sn] // "未知"' "$SN_MAP_FILE" 2>/dev/null || echo "未知"
}

# ==============================================
# 扫描所有 ST-LINK：序列号 + 芯片型号
# ==============================================
scan_devices() {
    local output=$(st-info --probe 2>/dev/null)
    local -n serials_ref=$1
    local -n chips_ref=$2
    local -A seen

    local serial=""
    local chip=""
    while IFS= read -r line; do
        if [[ $line =~ serial:[[:space:]]*(.+) ]]; then
            serial="${BASH_REMATCH[1]}"
        elif [[ $line =~ dev-type:[[:space:]]*(.+) ]]; then
            chip="${BASH_REMATCH[1]}"
            if [[ -n $serial && -n $chip && -z ${seen[$serial]} ]]; then
                serials_ref+=("$serial")
                chips_ref+=("$chip")
                seen[$serial]=1
                serial=""
                chip=""
            fi
        fi
    done <<< "$output"
}

SERIALS=()
CHIPS=()
scan_devices SERIALS CHIPS

if [ ${#SERIALS[@]} -eq 0 ]; then
    echo "❌ 未找到任何 ST-LINK 设备"
    exit 1
fi

# 提取唯一芯片型号
UNIQUE_CHIPS=($(printf "%s\n" "${CHIPS[@]}" | sort -u))

# ==============================================
# 菜单 1：选择芯片型号
# ==============================================
select_chip() {
selected_chip=0
chip_total=${#UNIQUE_CHIPS[@]}

draw_chip_menu() {
    clear
    echo "=================================================="
    echo "          请选择要批量烧录的芯片型号"
    echo "=================================================="
    echo " 共检测到 ${#SERIALS[@]} 个设备"
    echo "=================================================="
    for i in "${!UNIQUE_CHIPS[@]}"; do
        if [ $i -eq $selected_chip ]; then
            echo " > ${UNIQUE_CHIPS[$i]}  ◀︎"
        else
            echo "   ${UNIQUE_CHIPS[$i]}"
        fi
    done
    echo ""
    echo " ↑ ↓ 选择    回车 确认    Q 退出"
    echo "=================================================="
}

while true; do
    draw_chip_menu
    read -rsn1 key

    if [[ $key == "q" || $key == "Q" ]]; then
        clear
        exit 0
    fi

    if [[ $key == $'\x1b' ]]; then
        read -rsn2 key
        case $key in
            '[A') ((selected_chip--)) ; (( selected_chip < 0 )) && selected_chip=$((chip_total-1)) ;;
            '[B') ((selected_chip++)) ; (( selected_chip >= chip_total )) && selected_chip=0 ;;
        esac
    fi

    if [[ $key == "" ]]; then
        break
    fi
done

TARGET_CHIP="${UNIQUE_CHIPS[$selected_chip]}"

# 筛选对应芯片的设备
TARGET_SERIALS=()
for i in "${!SERIALS[@]}"; do
    if [[ "${CHIPS[$i]}" == "$TARGET_CHIP" ]]; then
        TARGET_SERIALS+=("${SERIALS[$i]}")
    fi
done
}

# 先选择芯片
select_chip

# ==============================================
# 菜单 2：选择 HEX 文件
# ==============================================
select_hex() {
hex_list=()
while IFS= read -r -d $'\0' file; do
    hex_list+=("$file")
done < <(find . -type f -name "*.hex" -print0)

if [ ${#hex_list[@]} -eq 0 ]; then
    echo "❌ 目录中没有 .hex 文件"
    exit 1
fi

selected_hex=0
hex_total=${#hex_list[@]}

draw_hex_menu() {
    clear
    echo "=================================================="
    echo "       芯片：$TARGET_CHIP （共 ${#TARGET_SERIALS[@]} 个）"
    echo "=================================================="
    for i in "${!hex_list[@]}"; do
        if [ $i -eq $selected_hex ]; then
            echo " > ${hex_list[$i]}  ◀︎"
        else
            echo "   ${hex_list[$i]}"
        fi
    done
    echo ""
    echo " ↑ ↓ 选择    回车 确认    B 返回    Q 退出"
    echo "=================================================="
}

while true; do
    draw_hex_menu
    read -rsn1 key

    if [[ $key == "q" || $key == "Q" ]]; then
        clear
        exit 0
    fi

    if [[ $key == "b" || $key == "B" ]]; then
        select_chip
        continue
    fi

    if [[ $key == $'\x1b' ]]; then
        read -rsn2 key
        case $key in
            '[A') ((selected_hex--)) ; (( selected_hex < 0 )) && selected_hex=$((hex_total-1)) ;;
            '[B') ((selected_hex++)) ; (( selected_hex >= hex_total )) && selected_hex=0 ;;
        esac
    fi

    if [[ $key == "" ]]; then
        break
    fi
done

SELECTED_HEX="${hex_list[$selected_hex]}"
}

# 选择HEX
select_hex

# ==============================================
# 并行烧录（带超时 + 编号映射）
# ==============================================
clear
echo "=================================================="
echo " 芯片：$TARGET_CHIP"
echo " 烧录：$SELECTED_HEX"
echo " 设备数：${#TARGET_SERIALS[@]}"
echo " 超时：${TIMEOUT}秒"
echo "=================================================="
echo ""

flash() {
    local sn=$1
    local hex=$2
    local num=$(get_sn_number "$sn")

    echo "🔹 编号 [$num]  $sn"
    echo "=================================================="

    # 静默烧录，不输出任何 st-flash 日志（只保留结果）
    timeout "$TIMEOUT" st-flash --serial "$sn" --format=ihex write "$hex" > /dev/null 2>&1

    if [ $? -eq 0 ]; then
        echo "✅ 编号 [$num] 烧录成功"
    else
        echo "❌ 编号 [$num] 烧录失败/超时"
    fi
    echo ""
}

for sn in "${TARGET_SERIALS[@]}"; do
    flash "$sn" "$SELECTED_HEX" &
done

wait

# clear
# echo "=================================================="
# echo " 🎉 所有 $TARGET_CHIP 烧录完成！"
# echo "=================================================="