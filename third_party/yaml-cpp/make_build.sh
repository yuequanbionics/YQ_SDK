#!/bin/bash

ARCH=$(uname -m)
# 定义不同架构对应的编译配置
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
    # 关键选项：BUILD_SHARED_LIBS=ON 生成共享库，YAML_CPP_BUILD_TESTS=OFF 禁用测试（加速编译）
    cmake -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_SHARED_LIBS=ON \
        -DYAML_CPP_BUILD_TESTS=OFF \
        -DYAML_CPP_BUILD_TOOLS=OFF \
        -DCMAKE_INSTALL_PREFIX=../../lib/install-x86_64 \
        ..

    # 编译（多线程）
    make -j$(nproc)

    # 安装（头文件、库文件到 install-x86_64）
    make install

    # 返回根目录，准备交叉编译 arm64
    cd ..

    # --------------------------
    # 交叉编译 arm64 架构
    # --------------------------
    # 清理旧 build 目录（避免架构文件冲突）
    if [ -d "build" ]; then
        rm -rf build
    fi
    mkdir -p build && cd build || exit

    # cmake 交叉编译配置（arm64，安装到 install-arm64）
    cmake -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_SHARED_LIBS=ON \
        -DYAML_CPP_BUILD_TESTS=OFF \
        -DYAML_CPP_BUILD_TOOLS=OFF \
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
    # 清理编译残留
    # --------------------------
    if [ -d "build" ]; then
        rm -rf build
    fi

    echo "编译完成！"
    echo "x86_64 版本安装路径：$(pwd)/install-x86_64"
    echo "arm64 版本安装路径：$(pwd)/install-arm64"
    
elif [ "$ARCH" = "aarch64" ]; then
    # --------------------------
    # 原生编译 arm64 架构
    # --------------------------
    # 清理旧 build 目录
    if [ -d "build" ]; then
        rm -rf build
    fi
    mkdir -p build && cd build || exit  # 若创建/进入 build 失败，直接退出

    # cmake 配置
    cmake -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_SHARED_LIBS=ON \
        -DYAML_CPP_BUILD_TESTS=OFF \
        -DYAML_CPP_BUILD_TOOLS=OFF \
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
    echo "arm64 版本安装路径：$(pwd)/install-arm64"
else
    echo "错误: 不支持的架构 $ARCH"
    exit 1
fi
