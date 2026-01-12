#!/bin/bash

wget http://fishros.com/install -O fishros && . fishros
   
systemctl status docker

sudo mkdir -p /etc/docker

sudo tee /etc/docker/daemon.json <<EOF
{
  "registry-mirrors": ["https://docker.registry.cyou", "https://docker-cf.registry.cyou", "https://dockercf.jsdelivr.fyi", "https://docker.jsdelivr.fyi", "https://dockertest.jsdelivr.fyi", "https://mirror.aliyuncs.com", "https://dockerproxy.com", "https://mirror.baidubce.com", "https://docker.m.daocloud.io", "https://docker.nju.edu.cn", "https://docker.mirrors.sjtug.sjtu.edu.cn", "https://docker.mirrors.ustc.edu.cn", "https://mirror.iscas.ac.cn", "https://docker.rainbond.cc", "https://do.nark.eu.org", "https://dc.j8.work", "https://dockerproxy.com", "https://gst6rzl9.mirror.aliyuncs.com", "https://registry.docker-cn.com", "http://hub-mirror.c.163.com", "http://mirrors.ustc.edu.cn/", "https://mirrors.tuna.tsinghua.edu.cn/", "http://mirrors.sohu.com/"],
  "insecure-registries": ["registry.docker-cn.com", "docker.mirrors.ustc.edu.cn"],
  "debug": true,
  "experimental": false
}
EOF

sudo systemctl daemon-reload
sudo systemctl restart docker

sudo docker info

systemctl status docker

sudo docker load -i config/docker_img/ubuntu20_04.tar
sudo docker load -i config/docker_img/ubuntu22_04.tar
sudo docker load -i config/docker_img/ubuntu24_04.tar

# docker save -o /home/user/docker_images/myapp_v1.0.tar myapp:1.0

# sudo docker run hello-world

