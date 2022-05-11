# RTMP 流媒体服务器

## 服务器端

1. 安装 nginx rtmp 插件

```bash
sudo apt install nginx libnginx-mod-rtmp
```

1. nginx 配置

```bash
sudo nano /etc/nginx/nginx.conf
```

在 nginx.conf 文件末尾加上：

```bash
rtmp {
        server {
                listen 1935;
                application live {
                        live on;
                        record off;
                }

        }
}
```

1. 防火墙开启端口 1935
   
    ![Untitled](./Untitled.png)
    
- 或者使用 firewall-cmd:
  
    ```bash
    sudo apt install firewalld
    ```
    
    ```bash
    sudo firewall-cmd --permanent --add-port=1935/tcp
    sudo firewall-cmd --reload
    ```
    

## 流媒体传送端

1. 查看摄像头设备节点

```bash
sudo apt install v4l-utils
```

```bash
v4l2-ctl --list-devices
```

1. 拍张照片

```bash
sudo apt install fswebcam
```

**capture a picture which called test1.jpg**

```bash
fswebcam -d /dev/video0 test1.jpg
```

1. 传输流媒体到服务器：

**FFMPEG:**

```bash
sudo apt install ffmpeg
```

```bash
ffmpeg -channel 0 -i /dev/video0 -r 10 -video_size 120x100 -b:v 100k -c:v libx264 -g 10 -f flv rtmp://101.43.121.224/live
```

**GSTREAMER:**

```bash
sudo apt-get install gstreamer
```

```bash
gst-launch-1.0 -v v4l2src device=/dev/video0 ! 'video/x-raw, width=1024, height=768, framerate=30/1' ! queue ! videoconvert ! omxh264enc ! h264parse ! flvmux ! rtmpsink location='rtmp://101.43.121.224/live'
```

**avconv: (软编码)**

```bash
avconv -f video4linux2 -r 24 -i /dev/video0 -f flv rtmp://101.43.121.224/live
```

---

<aside>
💡 rtmp://101.43.121.224/live 换成你服务器的公网ip

</aside>

1. 配置成系统服务

```bash
sudo nano /etc/systemd/system/ffmpeg.service
```

```bash
[Unit]
Description=ffmpeg
After=network.target

[Service]
Type=simple
ExecStart=ffmpeg -i /dev/video0 -r 10 -video_size 320X240 -b:v 100k -c:v libx264 -g 10 -f flv rtmp://101.43.121.224/live
RestartSec=30
Restart=on-failure

[Install]
WantedBy=multi-user.target
```

设置开机启动：

```bash
sudo systemctl enable ffmpeg.service
```

看下效果：

```bash
sudo systemctl daemon-reload
sudo systemctl restart ffmpeg.service
sudo systemctl status ffmpeg.service
```