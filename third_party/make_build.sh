#!/bin/bash

# 初始化编译标志
need_build=0

# 检查./lib文件夹是否不存在
if [ ! -d "./lib" ]; then
    need_build=1
fi

# 检查参数中是否包含"make"
for arg in "$@"; do
    if [ "$arg" = "make" ]; then
        need_build=1
        break  # 找到后退出循环
    fi
done

# 满足条件则执行编译
if [ $need_build -eq 1 ]; then
    echo "开始执行编译..."
    
    cd ncurses-6.4
    ./make_build.sh
    cd ..

    cd spdlog
    ./make_build.sh
    cd ..

    cd yaml-cpp
    ./make_build.sh
    cd ..

    echo "编译完成"
else
    echo "无需执行第三方编译(lib已存在且未指定make参数)"
fi
