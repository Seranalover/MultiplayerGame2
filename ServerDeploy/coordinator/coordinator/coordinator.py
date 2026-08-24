# 从flask Lib中引入Flask, request, jsonify组件
from flask import Flask, request, jsonify
import subprocess
from consts import SESSION_NAME_KEY, SESSION_SEARCH_ID_KEY, PORT_KEY
import re
# import docker

app=Flask(__name__)
# client = docker.from_env()

# 获得已被占用的端口号
def GetUsedPorts():
    result = subprocess.run(['docker', 'ps', '--format', '"{{.Ports}}"'], capture_output=True, text=True)
    output = result.stdout
    usedPorts = set()
    for line in output.strip().split("\n"):
        matches = re.findall(r'0\.0\.0\.0:(\d+)->', line)
        usedPorts.update(map(int, matches))
    # containers = client.containers.list()
    # usedPorts = set()
    # for container in containers:
    #     ports = container.attrs['NetworkSettings']['Ports']
    #     for port_bindings in ports.values():
    #         if port_bindings:
    #             for binding in port_bindings:
    #                 if binding['HostIp'] == '0.0.0.0':
    #                     usedPorts.add(int(binding['HostPort']))

    return usedPorts

# 获得下个可用端口
def FindNextAvailablePort(start=7777, end=8000):
    usedPorts = GetUsedPorts()
    for port in range(start, end+1):
        if port not in usedPorts:
            return port
    return 0

def CreateServerImpl(sessionName, sessionSearchId):
    port = FindNextAvailablePort()
    print(f"Launching server: {sessionName}, with id: {sessionSearchId}, at port: {port}")

    subprocess.Popen([
        "docker",
        "run",
        "--rm",
        "-p", f"{port}:{port}/tcp",
        "-p", f"{port}:{port}/udp",
        "multiplayergameserver",
        "-server",
        "-log",
        '-epicapp="ServerClient"',
        f'-SESSION_NAME="{sessionName}"',
        f'-SESSION_SEARCH_ID="{sessionSearchId}"',
        f'-PORT={port}'
        ])
    
    return port

# TODO: remove when using docker in the future
nextAvailablePort = 7777

# 本地windows测试方法
def CreateServerLocalTest(sessionName, sessionSearchId):
    global nextAvailablePort
    subprocess.Popen([
        "C:/Users/Qian/Documents/UnrealSrc/UnrealEngine/Engine/Binaries/Win64/UnrealEditor.exe",
        "C:/Users/Qian/Documents/Unreal Projects/MultiplayerGame2/MultiplayerGame2.uproject",
        "-server",
        "-log",
        '-epicapp="ServerClient"',
        f'-SESSION_NAME="{sessionName}"',
        f'-SESSION_SEARCH_ID="{sessionSearchId}"',
        f'-PORT={nextAvailablePort}'
    ])

    usedPort = nextAvailablePort
    nextAvailablePort += 1
    return usedPort

@app.route('/Sessions', methods=['POST'])
def CreateServer():
    print(dict(request.headers))

    sessionName = request.get_json().get(SESSION_NAME_KEY)
    sessionSearchId = request.get_json().get(SESSION_SEARCH_ID_KEY)

    # port = CreateServerLocalTest(sessionName, sessionSearchId)
    port = CreateServerImpl(sessionName, sessionSearchId)
    return jsonify({"status": "success", PORT_KEY: port}), 200

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=80)