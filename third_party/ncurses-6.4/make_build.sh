#!/bin/bash

ARCH=$(uname -m)
# 定义不同架构对应的执行路径
if [ "$ARCH" = "x86_64" ]; then
  # 清理之前的编译缓存
  make distclean

  # 重新配置（添加 --enable-pc-files 生成 .pc 文件）
  ./configure \
    --prefix=$(pwd)/../lib/install-x86_64 \
    --host=x86_64-linux-gnu \
    --with-shared \
    --with-termlib \
    --enable-widec \
    --enable-pc-files 

  # 编译并安装
  make -j$(nproc) && make install


  # 清理缓存
  make distclean

  # 重新配置 arm64 版本
  ./configure \
    --prefix=$(pwd)/../lib/install-arm64 \
    --host=aarch64-linux-gnu \
    --with-shared \
    --with-termlib \
    --enable-widec \
    --enable-pc-files \
    CC=aarch64-linux-gnu-gcc \
    CXX=aarch64-linux-gnu-g++

  # 编译并安装
  make -j$(nproc)
  # 1. 安装头文件（到 install-arm64/include）
  make install.includes
  # 2. 安装库文件（到 install-arm64/lib，包含 .so 动态库和 .pc 配置文件）
  make install.libs

  make distclean

elif [ "$ARCH" = "aarch64" ]; then
  # 清理之前的编译缓存
  make distclean

  # 重新配置（添加 --enable-pc-files 生成 .pc 文件）
  ./configure \
    --prefix=$(pwd)/../lib/install-arm64 \
    --host=aarch64-linux-gnu \
    --with-shared \
    --with-termlib \
    --enable-widec \
    --enable-pc-files 

  # 编译并安装
  make -j$(nproc) && make install

  make distclean
  echo "编译完成！"
else
  echo "错误:不支持的架构 $ARCH"
  exit 1
fi