# 使用 slim 版本大幅减小体积（如果必须用完整版，去掉 -slim 即可
FROM python:3.12-slim

# 安装 curl（如果需要健康检查或调试）
# RUN apt-get update && apt-get install -y --no-install-recommends curl \
#     && rm -rf /var/lib/apt/lists/*
RUN apt-get update && apt-get install -y docker.io

# 最后复制全部项目源码（放在后面，利用 Docker 缓存）
COPY . /app

# 设置工作目录
WORKDIR /app/coordinator

# 直接安装 flask（生产环境建议写进 requirements.txt）
RUN pip install --no-cache-dir flask

# RUN pip install docker

# 启动命令（ coordinator.py 在 /app 下）
ENTRYPOINT ["python", "coordinator.py"]