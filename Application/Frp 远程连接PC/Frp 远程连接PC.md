# Frp 远程连接PC

1. 安装 frp 服务端

```bash
cd ~ && wget -O frp https://github.com/fatedier/frp/releases/download/v0.40.0/frp_0.40.0_linux_amd64.tar.gz
cd frp/ 
chmod +x ./frps
```

```bash
./frps -c frps.ini
```

1. 配置成系统服务

```bash
sudo nano /etc/systemd/system/frps.service
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
sudo systemctl daemon-reload
sudo systemctl restart frps.service
sudo systemctl status frps.service
```

1. 放行端口

![Untitled](./Untitled.png)

1. 启动 客户端 client

![Untitled](./Untitled%201.png)

![Untitled](./Untitled%202.png)

1. PC 开机运行脚本 AlwaysUp