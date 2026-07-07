#!/bin/bash

# 初始化计数器
count=0

# 无限循环
while true; do  
    ((count++))     # 增加计数
    ./Test          # 执行程序
    echo "执行次数: $count"
    sleep 0.5       # 休眠
    echo "GOGOGO"
done