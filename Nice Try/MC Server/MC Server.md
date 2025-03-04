# MC Server

### 1.配置正确的 jdk

```bash
sudo update-alternatives --config java
```

### 2.启动服务器

- 修改eula.txt中false为true
- 修改server.properties中view-distance=5等等

```bash
java -jar minecraft_server.1.10.2.jar
```

### 3.NAT安装

```bash
uname -m
```

[SakuraFrp 4.0](https://www.natfrp.com/tunnel/download)

```bash
cd /usr/local/bin/
sudo wget -O frpc https://getfrp.sh/d/frpc_linux_armv7
sudo chmod 755 frpc
```

```bash
frpc -v 
```

> 不推荐采用 **`&`** 将 frpc 放到后台运行，建议将 frpc 注册为系统服务，见6
> 

### 4.NAT配置开机自启

- 执行下面的命令然后查看输出，找到初始化系统
  
    ```bash
    if [[ `/sbin/init --version` =~ upstart ]]; then echo Upstart; elif [[ `systemctl` =~ -\.mount ]]; then echo Systemd; elif [[ -f /etc/init.d/cron && ! -h /etc/init.d/cron ]]; then echo SysV-Init; else echo Unknown; fi
    ```
    
- Openwrt
  
    [Openwrt 服务配置 (natfrp.com)](https://doc.natfrp.com/#/frpc/service/openwrt)
    

#### 下面为Systemd配置：

a.编写配置文件

```bash
sudo nano /etc/systemd/system/frpc@.service
```

b.加入以下内容

> **[Unit]
> Description=SakuraFrp Service
> After=network.target
>
> [Service]
> Type=idle
> User=nobody
> Restart=on-failure
> RestartSec=60s
> ExecStart=/usr/local/bin/frpc -f %i
>
> [Install]
> WantedBy=multi-user.target**

c.重载Systemd

```bash
sudo systemctl daemon-reload
```

d.启动/停止隧道(start/stop + frpc@访问密钥:ID)

```bash
sudo systemctl start frpc@7ave3b1xhufdi26s:2671862
```

e.查看隧道状态

```bash
sudo systemctl status frpc@7ave3b1xhufdi26s:2671862
```

![Untitled](./Untitled.png)

f.列出所有隧道

```bash
sudo systemctl list-units -all frpc@*
```

g.配置开机自启

```bash
sudo systemctl enable frpc@7ave3b1xhufdi26s:2671862
```