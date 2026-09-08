# 使用 slim 版本大幅减小体积（如果必须用完整版，去掉 -slim 即可
FROM python:3.12-slim

# RUN apt-get update && apt-get install -y --no-install-recommends curl \
#     && rm -rf /var/lib/apt/lists/*
#RUN apt-get update && apt-get install -y docker.io
#RUN echo "deb https://mirrors.aliyun.com/debian trixie main" > /etc/apt/sources.list && \
#    echo "deb https://mirrors.aliyun.com/debian-security trixie-security main" >> /etc/apt/sources.list && \
#    apt-get update && apt-get install -y docker-ce-cli

# 安装 curl（如果镜像没有）
RUN apt-get update && apt-get install -y curl

# 从阿里云镜像下载 Docker CLI 静态二进制文件
RUN curl -fsSL https://mirrors.aliyun.com/docker-ce/linux/static/stable/x86_64/docker-27.3.1.tgz -o docker.tgz && \
    tar xzf docker.tgz --strip-components=1 -C /usr/local/bin docker/docker && \
    rm docker.tgz

# 最后复制全部项目源码（放在后面，利用 Docker 缓存）
COPY . /app

# 设置工作目录
WORKDIR /app/coordinator

# 安装最新版 flask（生产环境建议写进 requirements.txt）
RUN pip3 install --no-cache-dir --upgrade flask

# RUN pip install docker

# 启动命令（ coordinator.py 在 /app 下）
ENTRYPOINT ["python", "coordinator.py"]