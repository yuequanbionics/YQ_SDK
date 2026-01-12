#!/bin/bash
set -e  # 命令失败则终止脚本

# 检查是否传入版本号参数，未传入则使用默认值20.04
UBUNTU_VERSION=${1:-20.04}
# 镜像名称包含版本号，避免冲突（如ubuntu20.04、ubuntu22.04）
IMAGE_NAME="ubuntu${UBUNTU_VERSION}"

echo "开始构建 Ubuntu ${UBUNTU_VERSION} 版本的镜像..."

rm -rf lib/${IMAGE_NAME}
rm -rf bin/Tool/${IMAGE_NAME}
# 构建镜像，通过--build-arg传入Ubuntu版本号
docker build \
  --build-arg UBUNTU_VERSION=${UBUNTU_VERSION} \
  -t ${IMAGE_NAME} .

echo "镜像构建成功，启动容器并复制文件..."

# 启动临时容器（容器名包含版本号，避免冲突）
CONTAINER_NAME="temp_container_${UBUNTU_VERSION}"
if [ "$(docker ps -q -f name=^/${CONTAINER_NAME}$)" ]; then
  docker kill ${CONTAINER_NAME}
  echo "容器 ${CONTAINER_NAME} 已被杀死"
fi
docker run --rm -d --name ${CONTAINER_NAME} ${IMAGE_NAME} sleep infinity

# 复制文件到本地
mkdir lib/${IMAGE_NAME}
docker cp -a ${CONTAINER_NAME}:/app/lib ./lib/${IMAGE_NAME}
mv ./lib/${IMAGE_NAME}/lib/* ./lib/${IMAGE_NAME}
rm -rf ./lib/${IMAGE_NAME}/lib

mkdir bin/Tool/${IMAGE_NAME}
docker cp -a ${CONTAINER_NAME}:/app/bin/Tool ./bin/Tool/${IMAGE_NAME}
mv ./bin/Tool/${IMAGE_NAME}/Tool/* ./bin/Tool/${IMAGE_NAME}
rm -rf ./bin/Tool/${IMAGE_NAME}/Tool

if [ ! -d "./third_party/lib" ]; then
    docker cp -a ${CONTAINER_NAME}:/app/third_party/lib ./third_party
fi

# 强制停止容器（快速清理）
docker kill ${CONTAINER_NAME}

echo "文件复制完成：基于 Ubuntu ${UBUNTU_VERSION} 编译的lib已导出"






