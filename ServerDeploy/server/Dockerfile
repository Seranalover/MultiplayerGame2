FROM swr.cn-north-4.myhuaweicloud.com/ddn-k8s/gcr.io/distroless/cc-debian12:nonroot

COPY --chown=nonroot:nonroot ./server /home/nonroot/server

# ENTRYPOINT [ "/home/nonroot/server/MultiplayerGame2/Binaries/Linux/MultiplayerGame2Server", "MultiplayerGame2" ]
ENTRYPOINT [ "/home/nonroot/server/MultiplayerGame2/Binaries/Linux/MultiplayerGame2Server-Linux-Shipping", "MultiplayerGame2" ]