# Vsftp 

### 1.开启`vsftp`服务

```bash
sudo apt install vsftpd
```

取消注释`vsftpd.conf`文件中以下内容：

![image-20220525210059396](.\image-20220525210059396-16534836643966.png)

重新启动服务:

```bash
sudo systemctl restart vsftpd.service
```

###  2.window使用FTP连接

