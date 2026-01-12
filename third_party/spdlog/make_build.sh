#!/bin/bash

ARCH=$(uname -m)
# 定义不同架构对应的执行路径
if [ "$ARCH" = "x86_64" ]; then
    # --------------------------
    # 编译 x86_64 架构（默认架构）
    # --------------------------
    # 清理旧 build 目录
    if [ -d "build" ]; then
        rm -rf build
    fi
    mkdir -p build && cd build || exit  # 若创建/进入 build 失败，直接退出

    # cmake 配置（x86_64，安装到 install-x86_64）
    cmake -DCMAKE_BUILD_TYPE=Release \
        -DSPDLOG_HEADER_ONLY=OFF \
        -DSPDLOG_BUILD_SHARED=ON \
        -DCMAKE_INSTALL_PREFIX=../../lib/install-x86_64 \
        ..

    # 编译（多线程）
    make -j$(nproc)

    # 安装（头文件、库文件到 install-x86_64）
    make install

    # 返回根目录，准备下一个架构编译
    cd ..

    # --------------------------
    # 编译 arm64 架构（交叉编译）
    # --------------------------
    # 清理旧 build 目录（避免架构文件冲突）
    if [ -d "build" ]; then
        rm -rf build
    fi
    mkdir -p build && cd build || exit

    # cmake 配置（arm64 交叉编译，安装到 install-arm64）
    cmake -DCMAKE_BUILD_TYPE=Release \
        -DSPDLOG_HEADER_ONLY=OFF \
        -DSPDLOG_BUILD_SHARED=ON \
        -DCMAKE_INSTALL_PREFIX=../../lib/install-arm64 \
        -DCMAKE_SYSTEM_NAME=Linux \
        -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
        -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc \
        -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++ \
        ..

    # 编译（多线程）
    make -j$(nproc)

    # 安装（头文件、库文件到 install-arm64）
    make install

    # 返回根目录
    cd ..

    # --------------------------
    # 清理编译残留（可选，参考原脚本风格）
    # --------------------------
    if [ -d "build" ]; then
        rm -rf build
    fi

    echo "编译完成！"
    echo "x86_64 版本安装路径：$(pwd)/install-x86_64"
    echo "arm64 版本安装路径：$(pwd)/install-arm64"
    
elif [ "$ARCH" = "aarch64" ]; then
    # --------------------------
    # 编译 arm64 架构
    # --------------------------
    # 清理旧 build 目录
    if [ -d "build" ]; then
        rm -rf build
    fi
    mkdir -p build && cd build || exit  # 若创建/进入 build 失败，直接退出

    # cmake 配置
    cmake -DCMAKE_BUILD_TYPE=Release \
        -DSPDLOG_HEADER_ONLY=OFF \
        -DSPDLOG_BUILD_SHARED=ON \
        -DCMAKE_INSTALL_PREFIX=../../lib/install-arm64 \
        ..

    # 编译（多线程）
    make -j$(nproc)

    # 安装
    make install

    # 返回根目录
    cd ..

    if [ -d "build" ]; then
        rm -rf build
    fi

    echo "编译完成！"
else
    echo "错误:不支持的架构 $ARCH"
    exit 1
fi