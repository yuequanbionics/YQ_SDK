#!/bin/bash

# 自动配置 autorun.service 的脚本

# 检查是否传入参数
if [ $# -ne 1 ]; then
    echo "用法: $0 <true|false>"
    exit 1
fi

# 获取参数并转为小写
user_input=$(echo "$1" | tr '[:upper:]' '[:lower:]')

# 获取当前用户名
CURRENT_USER=$(whoami)

# 获取当前脚本的绝对路径（无论从哪里调用）
SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
SCRIPT_PATH="$SCRIPT_DIR/autorun.sh"

# 检查脚本是否存在
if [ ! -f "$SCRIPT_PATH" ]; then
    echo "错误：未找到 autorun.sh 脚本"
    echo "请确保此脚本与 autorun.sh 在同一目录"
    exit 1
fi

# 判断参数
if [ "$user_input" = "true" ]; then
    echo "参数为 true"
elif [ "$user_input" = "false" ]; then
    echo "输入为 false，正在关闭 autorun.service 自启动..."
    sudo systemctl disable autorun.service
    sudo systemctl stop autorun.service
    echo "已执行：sudo systemctl disable autorun.service"
    exit 0
else
    echo "错误：参数必须是 true 或 false"
    echo "用法: $0 <true|false>"
    exit 1
fi

# 检查并杀死正在运行的实例
if pgrep -f "$SCRIPT_PATH" > /dev/null; then
    echo "发现正在运行的 $SCRIPT_PATH 实例，正在停止..."
    sudo pkill -f "$SCRIPT_PATH"
    sleep 1  # 等待进程完全停止
fi

# 创建服务文件内容
SERVICE_CONTENT="[Unit]
Description=AutoRun Service
After=network.target

[Service]
Type=simple
ExecStart=$SCRIPT_PATH
Restart=always
RestartSec=10s     
StartLimitInterval=60s  
StartLimitBurst=3
User=$CURRENT_USER
WorkingDirectory=$SCRIPT_DIR

[Install]
WantedBy=multi-user.target"

# 写入服务文件
echo "$SERVICE_CONTENT" | sudo tee /etc/systemd/system/autorun.service > /dev/null

# 设置权限
sudo chmod 644 /etc/systemd/system/autorun.service

# 重载并启用服务
sudo systemctl daemon-reload
sudo systemctl enable autorun.service
sudo systemctl start autorun.service

# 显示结果
echo -e "\n服务已配置完成！"
echo "======================================"
echo "服务文件路径: /etc/systemd/system/autorun.service"
echo "运行用户: $CURRENT_USER"
echo "启动脚本: $SCRIPT_PATH"
echo "工作目录: $SCRIPT_DIR"
echo "======================================"
echo -e "\n使用以下命令管理服务："
echo "启动服务: sudo systemctl start autorun.service"
echo "停止服务: sudo systemctl stop autorun.service"
echo "查看状态: sudo systemctl status autorun.service"
echo "查看日志: journalctl -u autorun.service -f"
echo "查看日志: watch -n0.1 systemctl status autorun.service"
