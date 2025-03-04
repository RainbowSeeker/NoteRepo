# 系统服务注册

## 1.创建配置文件

Systemd默认从目录/etc/systemd/system/读取配置文件。但是里面存放的大部分文件都是符号链接，指向目录/usr/lib/systemd/system/，真正的配置文件存放在那个目录。比如：我们创建一个Nginx的服务

```bash
sudo vi /etc/systemd/system/nginx.service
```

## 2.编写配置文件

```bash
- Unit
   - Description,服务的描述
   - Documentation,文档介绍
   - After,该服务要在什么服务启动之后启动，比如Mysql需要在network和syslog启动之后再启动
- Install
   - WantedBy,值是一个或多个Target，当前Unit激活时(enable)符号链接会放入/etc/systemd/system目录下面以Target名+.wants后缀构成的子目录中
   - RequiredBy,它的值是一个或多个Target，当前Unit激活(enable)时，符号链接会放入/etc/systemd/system目录下面以Target名+.required后缀构成的子目录中
   - Alias,当前Unit可用于启动的别名
   - Also,当前Unit激活(enable)时，会被同时激活的其他Unit
- Service
   - Type,定义启动时的进程行为。它有以下几种值。
   - Type=simple,默认值，执行ExecStart指定的命令，启动主进程
   - Type=forking,以 fork 方式从父进程创建子进程，创建后父进程会立即退出
   - Type=oneshot,一次性进程，Systemd 当前服务退出，再继续往下执行
   - Type=dbus,当前服务通过D-Bus启动
   - Type=notify,当前服务启动完毕，会通知Systemd，再继续往下执行
   - Type=idle,若有其他任务执行完毕，当前服务才会运行
   - User,指定服务执行的用户
	 - ExecStart,启动当前服务的命令
   - ExecStartPre,启动当前服务之前执行的命令
   - ExecStartPost,启动当前服务之后执行的命令
   - ExecReload,重启当前服务时执行的命令
   - ExecStop,停止当前服务时执行的命令
   - ExecStopPost,停止当其服务之后执行的命令
   - RestartSec,自动重启当前服务间隔的秒数
   - Restart,定义何种情况 Systemd 会自动重启当前服务，可能的值包括always（总是重启）、on-success、on-failure、on-abnormal、on-abort、on-watchdog
   - PrivateTmp=True,表示给服务分配独立的临时空间
	 - TimeoutSec,定义 Systemd 停止当前服务之前等待的秒数
   - Environment,指定环境变量
```

比如，创建nginx.service配置文件：

```bash
[Unit]
Description=nginx
After=network.target
  
[Service]
Type=forking
ExecStart=/usr/local/nginx/sbin/nginx
ExecReload=/usr/local/nginx/sbin/nginx -s reload
ExecStop=/usr/local/nginx/sbin/nginx -s quit
PrivateTmp=true
  
[Install]
WantedBy=multi-user.target
```

## 3.生效配置文件

重载Systemd：

```bash
sudo systemctl daemon-reload
```

开启/关闭/查看服务：

```bash
sudo systemctl start/stop./status xxx.service
```

开机自启：

```bash
sudo systemctl enable xxx.service
```

如：

```bash
sudo nano /etc/systemd/system/nginx.service
```

输入下面配置文件：

```bash
[Unit]
Description=nginx
After=network.target
  
[Service]
Type=forking
ExecStart=/usr/local/nginx/sbin/nginx
ExecReload=/usr/local/nginx/sbin/nginx -s reload
ExecStop=/usr/local/nginx/sbin/nginx -s quit
PrivateTmp=true

[Install]
WantedBy=multi-user.target
```

---

输入下面配置文件：

```bash
[Unit]
Description=Aria2
After=network.target

[Service]
User=www-data
Type=simple
ExecStart=/etc/aria2/aria2c --conf-path=/etc/aria2/aria2.conf
RestartSec=60
Restart=on-failure

[Install]
WantedBy=multi-user.target
```

```bash
sudo systemctl enable nginx
sudo systemctl enable php-fpm
sudo systemctl enable aria2
```