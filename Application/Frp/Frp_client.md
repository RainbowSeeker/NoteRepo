# Frp

### 1.查看架构

```
uname -a
```

### 2.下载

~/.bashrc 添加代理 写完 source ~/.bashrc

```
export https_proxy=http://172.27.192.1:7890
export http_proxy=http://172.27.192.1:7890
export all_proxy=socks5://172.27.192.1:7890
```

```
wget https://github.com/fatedier/frp/releases/download/v0.46.0/frp_0.46.0_linux_amd64.tar.gz -qO frp.tar.gz && tar -zxvf frp.tar.gz
mv frp_0.46.0_linux_amd64/ frp
```

```
vi frpc.ini
```

```
[common]
server_addr = 101.43.121.224
server_port = 7000

[ssh]
type = tcp
local_ip = 127.0.0.1
local_port = 22
remote_port = 6001
```

```
sudo vi /etc/systemd/system/frpc.service
```

```
[Unit]
Description=MyFrp
After=network.target

[Service]
Type=idle
Restart=on-failure
RestartSec=60s
ExecStart=/home/yangyu/frp/frpc -c /yangyu/home/frp/frpc.ini

[Install]
WantedBy=multi-user.target
```

```
~/frp/frpc -c ~/frp/frpc.ini
```

