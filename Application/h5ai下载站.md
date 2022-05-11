# h5ai下载站  (Nginx+PHP+Aria2)

[h5ai · modern HTTP web server index for Apache httpd, lighttpd, nginx and Cherokee · larsjung.de](https://larsjung.de/h5ai/)

### 1.安装依赖

```bash
sudo apt update
sudo apt install -y php-fpm nginx pkg-config libxml2-dev libsqlite3-dev zlib1g-dev libcurl4-openssl-dev libpng-dev libjpeg-dev libonig-dev libzip-dev libpcre3 libpcre3-dev libxml2-dev libxslt-dev ffmpeg imagemagick
```

### 2.下载_h5i （需下载至映射的文件夹中,如 /nas）

```bash
cd /nas
wget https://release.larsjung.de/h5ai/h5ai-0.30.0.zip
unzip h5ai-0.30.0.zip
```

```bash
rm -r /var/www/html/
ln -s /nas/ /var/www/html
chown -R www-data:www-data /nas
```

- ****设置中文****
    
    修改 _h5ai/private/conf/option.json 中的 lang
    
    ```bash
    "l10n": {
         "enabled": true,
         "lang": "zh-cn",
         "useBrowserLang": true
     },
    ```
    
- ****二维码****
    
    修改 _h5ai/private/conf/option.json 中的 info
    
    ```bash
    "info": {
         "enabled": true,
         "show": true,
         "qrcode": true,
         "qrFill": "#999",
         "qrBack": "#fff"
     },
    ```
    
- ****搜索****
    
    修改 _h5ai/private/conf/option.json 中的 search
    
    ```bash
    "search": {
            "enabled": true,
            "advanced": true,
            "debounceTime": 300,
            "ignorecase": true
        },
    ```
    

### 3.配置 Nginx

```bash
sudo nano /etc/nginx/sites-enabled/default
```

> 在 index 后面加上 `/_h5ai/public/index.php` ，
在 server 里加上下面内容：
> 

```bash
				# php-fpm配置
        location ~ [^/]\.php(/|$){
            # try_files $uri =404;
            fastcgi_pass  127.0.0.1:9000;
            include fastcgi.conf;
            fastcgi_index  index.php;
            fastcgi_param  SCRIPT_FILENAME  $document_root/$fastcgi_script_name;
        }
```

### 4.配置php

```bash
sudo nano /etc/php/7.4/fpm/pool.d/www.conf
```

> 修改 listen 后面内容为 `9000`
> 

### 5.脚本下载 Aria2

```bash
cd ~
git clone https://github.com/Mintimate/AutoInstallAria2.git
unzip AutoInstallAria2-main.zip
```

<aside>
💡 修改 dirAria2=${HOME}/Downloads  为你的下载目录

</aside>

开始安装：

```bash
cd AutoInstallAria2-main/
bash AutoInstallAria2ForLinux.sh
```

- 修改下载目录
    
    ```bash
    sudo nano /etc/aria2/aria2.conf
    ```
    
- 配置环境变量
    
    ```bash
    export PATH=$PATH:/etc/aria2
    ```
    

### 6.设置系统服务

注册 Aria2 的系统服务：

```bash
sudo nano /etc/systemd/system/aria2.service
```

输入下面配置文件：

```bash
[Unit]
Description=Aria2
After=network.target

[Service]
User=root
Type=simple
ExecStart=/etc/aria2/aria2c --conf-path=/etc/aria2/aria2.conf

[Install]
WantedBy=multi-user.target
```

```bash
sudo systemctl enable nginx
sudo systemctl enable php7.4-fpm
sudo systemctl enable aria2
```

大功告成，打开你的ip地址访问吧