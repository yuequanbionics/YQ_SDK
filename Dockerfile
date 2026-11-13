# 接收外部传入的Ubuntu版本号（默认20.04）
ARG UBUNTU_VERSION=20.04

# 使用指定版本的Ubuntu镜像（确保该版本存在，如18.04、20.04、22.04等）
FROM ubuntu:${UBUNTU_VERSION}

# 设置非交互式环境
ENV DEBIAN_FRONTEND=noninteractive

# 安装依赖
FROM ubuntu:20.04

WORKDIR /workspace

RUN DEBIAN_FRONTEND=noninteractive TZ=Asia/Shanghai \
    echo "Asia/Shanghai" > /etc/timezone && \
    ln -sf /usr/share/zoneinfo/Asia/Shanghai /etc/localtime && \
    echo "deb [arch=amd64] http://mirrors.ustc.edu.cn/ubuntu/ focal main restricted universe multiverse" > /etc/apt/sources.list && \
    echo "deb [arch=amd64] http://mirrors.ustc.edu.cn/ubuntu/ focal-updates main restricted universe multiverse" >> /etc/apt/sources.list && \
    echo "deb [arch=amd64] http://mirrors.ustc.edu.cn/ubuntu/ focal-security main restricted universe multiverse" >> /etc/apt/sources.list && \
    echo "deb [arch=arm64] http://mirrors.ustc.edu.cn/ubuntu-ports/ focal main restricted universe multiverse" >> /etc/apt/sources.list && \
    echo "deb [arch=arm64] http://mirrors.ustc.edu.cn/ubuntu-ports/ focal-updates main restricted universe multiverse" >> /etc/apt/sources.list && \
    echo "deb [arch=arm64] http://mirrors.ustc.edu.cn/ubuntu-ports/ focal-security main restricted universe multiverse" >> /etc/apt/sources.list && \
    dpkg --add-architecture arm64 && \
    apt-get update && \
    apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    g++ \
    ncurses-term \
    gcc-aarch64-linux-gnu g++-aarch64-linux-gnu \
    libboost-all-dev \
    libreadline-dev \
    libncurses5-dev \
    libreadline-dev:arm64 \
    libncurses5-dev:arm64 \
    && rm -rf /var/lib/apt/lists/*

COPY . .

RUN rm -rf build lib/ubuntu20.04 lib/ubuntu22.04 lib/ubuntu24.04 bin/Tool/ubuntu20.04 bin/Tool/ubuntu22.04 bin/Tool/ubuntu24.04 || true && \
    cd ./third_party && \
    ./make_build.sh && \
    cd .. && \
    mkdir build && cd build && \
    cmake .. && \
    make -j12 && \
    rm -rf ./* || true && \
    cmake -DCMAKE_TOOLCHAIN_FILE=../config/cmake/aarch64-toolchain.cmake .. && \
    make -j12



WORKDIR /app
COPY . /app

# 编译项目
RUN rm -rf build lib/ubuntu20.04 lib/ubuntu22.04 lib/ubuntu24.04 bin/Tool/ubuntu20.04 bin/Tool/ubuntu22.04 bin/Tool/ubuntu24.04 || true && \
    # 
    cd ./third_party && \
    ./make_build.sh && \
    cd .. && \
    # 
    mkdir build && cd build && \
    cmake .. && \
    make -j12 && \
    rm -rf ./* || true && \
    cmake -DCMAKE_TOOLCHAIN_FILE=../config/cmake/aarch64-toolchain.cmake .. && \
    make -j12 