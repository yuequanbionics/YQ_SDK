#!/bin/bash

echo "===== IP地址配置工具 ====="
        
# 获取所有非lo网络接口
interfaces=$(ip link show | grep -E '^[0-9]+:' | grep -v lo | awk -F: '{print $2}' | sed 's/ //g')

# 显示可用网卡菜单
echo "请选择要配置的网卡:"
i=1
for iface in $interfaces; do
    echo "$i) $iface"
    ((i++))
done
echo "d) 删除选中网卡的所有临时IP"
echo "q) 退出"

# 读取用户选择
read -p "请输入选项 (1-$((i-1))、d 或 q): " choice

# 处理用户选择
case $choice in
    [1-$((i-1))])
        # 获取选择的网卡名称
        selected_iface=$(echo "$interfaces" | sed -n "${choice}p")
        
        # IP地址选择菜单
        echo "请选择IP地址配置方式:"
        echo "1) 使用预设IP地址"
        echo "2) 手动输入IP地址"
        read -p "请选择(1或2): " ip_choice
        
        case $ip_choice in
            1)
                # 预设IP地址列表
                echo "请选择预设IP地址:"
                echo "1) 192.168.3.245"
                echo "2) 192.168.3.246"
                echo "3) 192.168.3.247"
                echo "4) 192.168.3.248"
                echo "5) 192.168.3.249"
                read -p "请选择预设IP(1-5): " preset_choice
                
                case $preset_choice in
                    1) ip_addr="192.168.3.245" ;;
                    2) ip_addr="192.168.3.246" ;;
                    3) ip_addr="192.168.3.247" ;;
                    4) ip_addr="192.168.3.248" ;;
                    5) ip_addr="192.168.3.249" ;;
                    *)
                        echo "❌ 错误:无效的预设IP选择"
                        exit 1
                        ;;
                esac
                ;;
            2)
                # 手动输入IP地址
                read -p "请输入IP地址 (如: 192.168.1.1): " ip_addr
                ;;
            *)
                echo "❌ 错误:无效的选项，请输入 1 或 2"
                exit 1
                ;;
        esac
        
        # 设置默认子网掩码为24
        prefix=24
        
        # 验证输入格式（基础验证）
        if [[ $ip_addr =~ ^[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
            echo "正在配置网卡 $selected_iface ..."
            echo "使用默认子网掩码: /$prefix (255.255.255.0)"
            echo "配置IP地址: $ip_addr/$prefix"
            
            # 自动计算下一个可用的标签编号
            # 获取当前网卡已有的标签编号
            existing_labels=$(ip addr show dev "$selected_iface" | grep -oE "${selected_iface}:[0-9]+" | cut -d: -f2 | sort -n)
            
            if [ -z "$existing_labels" ]; then
                # 如果没有现有标签，从0开始
                next_label=0
            else
                # 获取最大的标签编号并加1
                max_label=$(echo "$existing_labels" | tail -1)
                next_label=$((max_label + 1))
            fi
            
            # 使用自动计算的标签编号
            label="${selected_iface}:${next_label}"
            sudo ip addr add "${ip_addr}/${prefix}" dev "$selected_iface" label "$label"
            
            if [ $? -eq 0 ]; then
                echo "✅ 配置成功！网卡 ${selected_iface} 的IP已设置为 ${ip_addr}/${prefix} (标签: $label)"
            else
                echo "❌ 配置失败，请检查权限和参数是否正确"
            fi
        else
            echo "❌ 错误:IP地址格式无效"
            exit 1
        fi
        ;;
    d|D)
        # 删除选中网卡的所有临时IP功能
        echo "请选择要删除临时IP的网卡:"
        j=1
        for iface in $interfaces; do
            echo "$j) $iface"
            ((j++))
        done
        read -p "请输入要删除IP的网卡编号 (1-$((j-1))): " del_choice
        
        if [[ "$del_choice" =~ ^[0-9]+$ ]] && [ "$del_choice" -ge 1 ] && [ "$del_choice" -le $((j-1)) ]; then
            selected_del_iface=$(echo "$interfaces" | sed -n "${del_choice}p")
            echo "正在删除网卡 $selected_del_iface 的所有临时IP..."
            
            # 使用flush命令删除指定网卡的所有IP地址
            sudo ip addr flush dev "$selected_del_iface"
            
            if [ $? -eq 0 ]; then
                echo "✅ 删除成功！网卡 ${selected_del_iface} 的所有临时IP已清除"
                echo "当前网卡状态:"
                ip addr show dev "$selected_del_iface" | grep -E "inet|state"
            else
                echo "❌ 删除失败，请检查权限和网卡名称是否正确"
            fi
        else
            echo "❌ 错误:无效的网卡选择"
            exit 1
        fi
        ;;
    q|Q)
        echo "退出配置程序"
        exit 0
        ;;
    *)
        echo "❌ 错误:无效的选项，请输入 1-$((i-1)) 之间的数字、d 或 q"
        exit 1
        ;;
esac
