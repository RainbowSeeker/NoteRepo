# Ngrok

---

## 个人网站

1. 安装服务器软件

```bash
#安装apache2
sudo apt install -y apache2 apache2-utils
#安装php
sudo apt install -y libapache2-mod-php8.0 php8.0

#重启Apache服务器   
sudo systemctl restart apache2 
sudo systemctl status  apache2
```

<aside>
💡 完成后，可以通过输入树莓派的**ip地址**访问到页面了！

</aside>

1. 安装pi-dashboard

```bash
#在www路径下下载文件
cd /var/www
sudo git clone https://github.com/nxez/pi-dashboard.git
sudo unzip pi-dashboard
#用 pi-dashboard 替换原先的html文件夹
sudo rm -r html/
sudo mv pi-dashboard/ html
sudo chown -R www-data html/  #非常重要
```

1. 配置开机自启

[https://www.notion.so/MC-Server-0ce1afcbf59c4ec69224557d63826229#1983963e6d8e4197b49989adafb95e7b](MC%20Server%200ce1a.md)

```bash
sudo nano /etc/systemd/system/ngrok.service
#input as follow
[Unit]
Description=Ngrok Service
After=network.target

[Service]
User=pi
Restart=on-failure
RestartSec=60s
ExecStart=/home/pi/Blog/ngrok http 80 --region=jp
StandardOutput=inherit
StandardError=inherit

[Install]
WantedBy=multi-user.target
#then
sudo systemctl daemon-reload
sudo systemctl restart ngrok.service
sudo systemctl status ngrok.service
sudo systemctl enable ngrok.service
```

## 远程ssh

1. 安装ngrok

```bash
wget https://bin.equinox.io/c/4VmDzA7iaHb/ngrok-stable-linux-arm.tgz
tar -zxvf ngrok-stable-linux-arm.tgz
```

```bash
./ngrok authtoken 24GZhdRVkBEn3PpRHF2jjMuxs1r_82y6dkcv3mqhWNWJajqby
```

<aside>
💡 Tips: authtoken 需要从官网注册获取 [Your Authtoken - ngrok](https://dashboard.ngrok.com/get-started/your-authtoken)

</aside>

1. 启动ngrok

```bash
./ngrok tcp 22 --region=jp
```

![Untitled](Ngrok%2030e4c/Untitled.png)

1. 配置开机自启

[系统服务注册](../Linux%206d393/%E7%B3%BB%E7%BB%9F%E6%9C%8D%E5%8A%A1%E6%B3%A8%E5%86%8C%20998d1.md) 

1. 查看个人ngrok映射地址，并连接

[ngrok - secure introspectable tunnels to localhost](https://dashboard.ngrok.com/endpoints/status)

![Untitled](Ngrok%2030e4c/Untitled%201.png)