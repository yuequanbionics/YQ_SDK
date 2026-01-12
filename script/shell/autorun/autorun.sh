#!/bin/bash

# 获取脚本自身的绝对路径
SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
RUN_EXE_SCRIPT="$SCRIPT_DIR/run_stop_exe.sh"

# 控制程序的函数
# 参数: $1=程序名称 $2=操作(run/stop) $3=APP状态变量名
control_app() {
    local app_name=$1
    local action=$2
    local app_state_var=$3
    
    if [ -f "$RUN_EXE_SCRIPT" ]; then
        echo "正在${action}程序: $RUN_EXE_SCRIPT $app_name $action"
        
        if [ "$action" == "run" ]; then
            "$RUN_EXE_SCRIPT" "$app_name" "$action" &
            eval "$app_state_var=1"
        else
            "$RUN_EXE_SCRIPT" "$app_name" "$action"
            eval "$app_state_var=0"
        fi
        
        echo "${app_state_var} 状态更新为: ${!app_state_var} ($([ "${!app_state_var}" == "1" ] && echo "运行" || echo "挂起"))"
    else
        echo "错误: 未找到 $RUN_EXE_SCRIPT"
        return 1
    fi
}

# APP 0
APP0_previous=$(gpio read 3)  # GPIO3 前一个状态
APP0=0  # 应用挂起状态 (0:挂起, 1:运行)
echo "初始状态: GPIO3=$APP0_previous, APP0=$APP0"
gpio mode 10 out
gpio write 10 0

# APP 1
APP1_previous=$(gpio read 4)  # GPIO3 前一个状态
APP1=0  # 应用挂起状态 (0:挂起, 1:运行)
echo "初始状态: GPIO4=$APP1_previous, APP1=$APP1"
gpio mode 12 out
gpio write 12 0

# 捕获Ctrl+C信号
trap 'echo "监控已停止"; exit 0' SIGINT

while true; do
    APP0_current=$(gpio read 3)
    APP1_current=$(gpio read 4)
    
    # 检测状态变化：从0变到1 APP0
    if [[ "$APP0_previous" == "0" && "$APP0_current" == "1" && "$APP1" == "0" ]]; then
        echo "$(date '+%Y-%m-%d %H:%M:%S') - APP0_previous 状态从 0 变为 1"
        if [ "$APP0" == "0" ]; then
            control_app "Test" "run" "APP0"
        else
            control_app "Test" "stop" "APP0"
        fi
        gpio write 10 $APP0
    fi

    # 检测状态变化：从0变到1 APP1
    if [[ "$APP1_previous" == "0" && "$APP1_current" == "1" && "$APP0" == "0" ]]; then
        echo "$(date '+%Y-%m-%d %H:%M:%S') -APP1_previous 状态从 0 变为 1"
        if [ "$APP1" == "0" ]; then
            control_app "pretighten" "run" "APP1"
        else
            control_app "pretighten" "stop" "APP1"
        fi
        gpio write 12 $APP1
    fi

    APP0_previous="$APP0_current"
    APP1_previous="$APP1_current"
    
    # 检测间隔
    sleep 0.1
done




# nohup ./autorun.sh > /dev/null 2>&1 &

# sudo nano /etc/systemd/system/autorun.service

# [Unit]
# Description=My Autorun Script
# After=network.target

# [Service]
# Type=simple
# ExecStart=/home/orangepi/SDK/rhs/script/shell/autorun.sh
# Restart=always
# User=orangepi
# Group=orangepi

# [Install]
# WantedBy=multi-user.target

# sudo chmod 644 /etc/systemd/system/autorun.service
# sudo systemctl daemon-reload
# sudo systemctl enable autorun.service
# sudo systemctl start autorun.service
# sudo systemctl status autorun.service
# watch -n0.1 systemctl status autorun.service


