#!/bin/bash

# 定义OTA工具路径（方便后续修改）
TOOL_PATH="./bin/Tool/x86/OTA"
# 定义要搜索的目录和目标文件
SEARCH_DIR="./config/YAML"
TARGET_FILE="OTA.yaml"

# ====================== 核心函数：无参数时的文件选择逻辑 ======================
select_ota_file() {
    echo -e "开始搜索 [$SEARCH_DIR] 及其子目录下的 [$TARGET_FILE] 文件...\n"
    
    # 1. 递归搜索所有OTA.yaml，结果存入数组（处理文件路径含空格的情况，兼容大部分场景）
    # find参数说明：-type f 仅找文件，-name 精准匹配文件名，-print0 处理空格
    while IFS= read -r -d '' file; do
        ota_files+=("$file")
    done < <(find "$SEARCH_DIR" -type f -name "$TARGET_FILE" -print0)

    # 2. 校验：如果没找到任何OTA.yaml，提示并退出脚本
    if [ ${#ota_files[@]} -eq 0 ]; then
        echo "❌ 错误：在【$SEARCH_DIR】及其子目录下未找到任何【$TARGET_FILE】文件！"
        exit 1
    fi

    # 3. 打印带序号的选择表（序号从1开始，用户友好）
    echo "✅ 共找到 ${#ota_files[@]} 个目标文件，请选择序号执行："
    for i in "${!ota_files[@]}"; do
        # 序号 = 数组索引+1，数组索引从0开始
        echo "   [$((i+1))] ${ota_files[$i]} [$((i+1))]"
    done
    echo ""

    # 4. 接收用户输入，并做合法性校验（非数字/越界则循环重输）
    while true; do
        read -p "请输入选择的序号（1-${#ota_files[@]}）：" user_choice
        # 校验1：输入是否为纯数字（正则匹配）
        if ! [[ "$user_choice" =~ ^[0-9]+$ ]]; then
            echo "⚠️  请输入有效数字，不要输入字母/符号！"
            continue
        fi
        # 校验2：数字是否在1~文件总数范围内
        if [ "$user_choice" -lt 1 ] || [ "$user_choice" -gt ${#ota_files[@]} ]; then
            echo "⚠️  序号越界！请输入1-${#ota_files[@]}之间的数字！"
            continue
        fi
        # 输入合法，退出循环
        break
    done

    # 5. 转换为数组索引（用户输入的序号-1），获取最终选择的文件路径
    select_index=$((user_choice-1))
    selected_file="${ota_files[$select_index]}"
    echo -e "\n✅ 你选择的文件：$selected_file"

    # 6. 执行OTA工具，传入选择的文件路径
    echo -e "\n开始执行OTA工具...\n"
    "$TOOL_PATH" "$selected_file"
}

# ====================== 主逻辑：参数判断 ======================
if [ $# -eq 0 ]; then
    # 无参数：调用选择函数，自动搜索+用户选择
    select_ota_file
else
    # 有参数：直接将第一个参数传给OTA工具（保持原逻辑）
    echo "直接执行OTA工具，传入参数：$1"
    "$TOOL_PATH" "$1"
fi

# ====================== 最后执行日志清理（保持原逻辑） ======================
echo -e "\nOTA工具执行完成，开始清理logs目录..."
rm -rf ./logs
echo "✅ logs目录清理完成！脚本执行结束。"
