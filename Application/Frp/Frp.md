# Frp

1. 安装 frp 服务端

```bash
cd ~ && wget -q https://github.com/fatedier/frp/releases/download/v0.44.0/frp_0.44.0_linux_amd64.tar.gz && tar -zxvf frp_0.44.0_linux_amd64.tar.gz && mv frp_0.44.0_linux_amd64 frp && rm frp_0.44.0_linux_amd64.tar.gz
```

```bash
cd frp/
chmod +x ./frps
./frps -c frps.ini
```

1. 配置成系统服务

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

1. 放行端口

![Untitled](./Untitled.png)

1. 启动 客户端 client

![Untitled](./Untitled%201.png)

![Untitled](./Untitled%202.png)

1. PC 开机运行脚本 AlwaysUp