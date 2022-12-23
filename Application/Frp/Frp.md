# Frp

## 安装 frp 服务端

```bash
cd ~ && wget -O frp.tar.gz https://github.com/fatedier/frp/releases/download/v0.44.0/frp_0.44.0_linux_amd64.tar.gz 
tar -zxvf frp.tar.gz 
rm frp_0.44.0_linux_amd64.tar.gz
```

```bash
cd frp/
chmod +x ./frps
./frps -c frps.ini
```

### 1.配置成系统服务

```bash
vi /etc/systemd/system/frps.service
```

```bash
[Unit]
Description=MyFrp
After=network.target

[Service]
Type=idle
Restart=on-failure
RestartSec=60s
ExecStart=/root/frp/frps -c /root/frp/frps.ini

[Install]
WantedBy=multi-user.target
```

```bash
systemctl daemon-reload
systemctl enable frps.service
```

```bash
systemctl restart frps.service
systemctl status frps.service
```

### 2.放行端口

<img src="./Untitled.png" alt="Untitled" style="zoom: 50%;" />

## 启动 客户端 client

### 1.配置成系统服务

```bash
[common]
server_addr = 101.43.121.224
server_port = 7000
auth_token = 12134

[ssh]
type = tcp
local_ip = 127.0.0.1
local_port = 22
remote_port = 6000
[jupyter]
type = tcp
local_ip = 127.0.0.1
local_port = 8888
remote_port = 8888
```



