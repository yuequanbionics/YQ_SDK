#!/bin/bash

# 获取脚本自身的绝对路径
SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
BIN_DIR="$SCRIPT_DIR/../../../bin"

# 检查是否提供了程序名称
if [ $# -eq 0 ]; then
    echo "错误：请提供要运行的程序名称"
    echo "用法：$0 <程序名称> [run|stop]"
    exit 1
fi

PROGRAM_NAME=$1
ACTION=${2:-"run"}  # 默认为run
FULL_PATH="$BIN_DIR/$PROGRAM_NAME"

# 检查程序文件是否存在
if [ ! -f "$FULL_PATH" ]; then
    echo "错误：程序 '$PROGRAM_NAME' 在 $BIN_DIR 目录中不存在"
    exit 1
fi

# 检查程序是否可执行
if [ ! -x "$FULL_PATH" ]; then
    echo "错误：程序 '$PROGRAM_NAME' 不可执行"
    exit 1
fi

case $ACTION in
    "run")
        # 检查并杀死已运行的实例
        if pgrep -x "$PROGRAM_NAME" > /dev/null; then
            echo "发现已运行的 '$PROGRAM_NAME' 实例，正在停止..."
            pkill -x "$PROGRAM_NAME"
            sleep 1  # 等待进程完全停止
        fi

        # 执行程序
        echo "正在启动程序: $PROGRAM_NAME"
        "$FULL_PATH" &

        # 检查程序是否成功启动
        sleep 0.2
        if pgrep -x "$PROGRAM_NAME" > /dev/null; then
            echo "程序 '$PROGRAM_NAME' 已成功启动 (PID: $(pgrep -x "$PROGRAM_NAME"))"
        else
            echo "警告：程序 '$PROGRAM_NAME' 可能启动失败"
            exit 1
        fi
        ;;
    "stop")
        if pgrep -x "$PROGRAM_NAME" > /dev/null; then
            echo "正在停止程序: $PROGRAM_NAME"
            pkill -x "$PROGRAM_NAME"
            sleep 0.5
            if ! pgrep -x "$PROGRAM_NAME" > /dev/null; then
                echo "程序 '$PROGRAM_NAME' 已成功停止"
            else
                echo "警告：停止程序 '$PROGRAM_NAME' 失败"
                exit 1
            fi
        else
            echo "程序 '$PROGRAM_NAME' 当前没有运行"
        fi
        ;;
    *)
        echo "错误：无效的第二个参数 '$ACTION'"
        echo "可用参数: run (默认) - 启动程序, stop - 停止程序"
        exit 1
        ;;
esac