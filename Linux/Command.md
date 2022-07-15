# Command

## File

```bash
#文件操作
file one.txt     #查看文件类型
touch empty.txt  #新建文件
mkdir (-p) new        #新建目录
rmdir new        #移除目录
cp -r doc1 doc2  #拷贝目录下所有文件
rm -r doc2       #移除目录下所有文件
mv xxx1 xxx2     #移动/重命名
cat one.txt      #输出整个文件
head -2 one.txt  #输出前2行
tail -3 one.txt  #输出后3行

#文件搜索
find [path] -name "*.c" [-ls]

#压缩
tar -zcvf directory directory.gz
#解压缩
tar -zxvf directory directory.gz

#软链接
ln -s source.txt direction.txt #--链接文件，相当于快捷方式

#重新定向
ls > output.log           ls >> output.log #如果已存在则附加在结尾
ls 1> output.log 2> error.log  #1:stdout 2:stderr
ls &> output_error.log #1+2
grep abc < content.txt
#pipe
ls | grep txt | wc -l #word count
cat 1.txt | sort | uniq > 2.txt #删除重复

#权限
sudo chown (-R) pi:pi file.txt #(递归)改变文件的拥有者和拥有组
sudo chmod 755 file.txt   #改变文件的权限 4:r 2:w 1:x
sudo chmod a+r,u+w,g+o,o-o file.txt #修改对应组别的权限
**755:rwxr-xr-x**
444:r--r--r--
777:rwxrwxrwx
644:rw-r--r--

#文件系统 FAT:window ext4:raspberry NTFS:Network
sudo fdisk -l          #查看磁盘信息
sudo umount /dev/sda1  #卸载磁盘
sudo df -h             #report file system disk space usage
free -h                #**缓冲**
#/bin:可执行文件
#/etc:配置文件 .conf   *rc
#/proc:硬件信息 cat /proc/cpuinfo
```

## Bash

```bash
#变量
var=abc    # ' $var'==>纯文本  " $var"==>可以加入变量
echo $var  #or ${var}
now=`date` #将date的输出存入now <==> now=$(date)
read name  #将键盘输入存入name

#返回代码
echo $?      #正常运行则返回0
date && date #前一个成功后才运行
date || date #前一个失败后才运行
echo $1      #获得第1个参数
exit 1       #中途退出脚本，并返回1

#Signal信号
**fg                    #回到上一个暂停进程**
ps aux | grep " "     #获取进程信息，如PID
kill (-s SIGINT) -9 xpid #结束进程 <==> Ctrl+C
kill -9 `ps -ef|grep frpc.ini| grep -v grep| awk '{print $1}'`
#SIGINT:中断 SIGTSTP:暂停 SIGQUIT:退出
trap "echo 'interrupted';" SIGINT #捕获信号(异步)

#数值运算 $(())
echo $((2+5*2**(3-1)/2)) #priority:() -> ** -> */% -> +-

#定义函数
myInfo(){
	lscpu > $1
  lsmod >>$1
}
myInfo log      #调用该函数,log为参数

#结构
if [ -e $1 ]          case $var in         while []    
then                 root)                do          
	..                   ..                   ..          
else                 ;;                   done        
	..                 *)                   for x in xxx
fi                     ..                 do
                     ;;                     ..
                     esac                 done

```

### Disk

1. 挂载磁盘

```bash
#查看磁盘情况
sudo fdisk -l 
#显示磁盘空间占用
df -h
#挂载到指定路径
sudo mkdir /mnt/usb_flash
sudo mount -t vfat -o uid=pi,gid=pi,iocharset=utf8 /dev/sda1 /mnt/usb_flash
#设置默认挂载
sudo nano /etc/fstab

#if exFAT
sudo apt install exfat-fuse
/dev/sda1 /mnt/usbdisk vfat rw,defaults 0 0
#if FAT32
/dev/sda1 /mnt/myusbdrive auto defaults,noatime,umask=0000 0 0
#if NTFS
sudo apt install fuse-utils ntfs-3g 
modprobe fuse  #加载内核模块
/dev/sda1 /mnt/myusbdrive ntfs-3g defaults,noexec,umask=0000 0 0

#取消挂载
sudo umount /dev/sda1
```

1. 格式化磁盘（需要先umount）

```bash
#FAT32
sudo mkfs.vfat -F 32 /dev/sdb1

#NTFS
sudo apt install ntfsprogs
sudo mkfs.ntfs /dev/sda1

#EXT
sudo mkfs.ext4 /dev/sda1
sudo mkfs.ext3 /dev/sda1
```

## String

```bash
string="hello world"
#cut
echo $string |cut -d" " -f1 #hello 1表示输出分割后的第一个字段 1-表示输出字段1以及后边的所有字段
echo $string |cut -c 1-4    #hell

#${}
string="hello,shell,haha"
array=${string//,/ }    **#按'，'分隔**
for var in ${array[@]}    
do
echo $var
done

**echo ${#string}  # 11  获得字符串长度**
string="http://www.test.com"
echo ${string#*//} # www.test.com '#'号截取，删除左边字符，保留右边字符
#more:https://blog.csdn.net/Jayccccc_chao/article/details/85281529
```

## **WHO**

```bash
who am i #find myself
who      #find all users
echo hh | write pi  #send msg to pi
groups , id
/etc/passwd #存储用户信息  nologin/false 伪用户
```

### 创建用户及用户组

```bash
#常用command
sudo useradd -G sudo -m yangyu			#创建sudoer用户		-m:创建home-directory
sudo usermod -s /bin/bash yangyu		#设置默认shell
sudo passwd yangyu						#设置密码

# 创建一个用户
sudo useradd www
# 创建一个用户组
sudo groupadd www
# www用户归属于www用户组
sudo usermod -a -G www www
```

## **Exe**

```bash
which date          #定位可执行文件
type date           ****#打印指令类型，如为exe则再打印文件的路径
****alias xxx="free -h" #**起别名，类似于宏定义define**
man ls              #**返回帮助文档**
```

## 网络协议

### 协议分层

1. 物理层(Physical Layer):光纤等真实存在的物理媒介，传输二进制序列
2. 连接层(Link Layer):将二进制序列分割成帧，常用的连接层协议为 Ethernet and Wi-Fi
3. 网络层(Network Layer):“中间人”设备，如路由器
4. 传输层(Transport Layer):通过端口号将信息传送给指定进程
5. 应用层(Application Layer): 某个类型应用间的某个协议，如用于web的HTTP协议，域名解析的DNS协议

```bash
#查询整个局域网内所有ip&MAC
sudo apt install arp-scan -y
sudo arp-scan -l

#修改ip
#wlan:
sudo dhclient -v -r  #更新DHCP租约，设备将释放ip地址
sudo ifconfig wlan0 192.168.x.x up
#eth: 编辑/etc/dhcpcd.conf,在末尾加入
interface eth0
static ip_address=192.168.x.x

#域名解析
host www.baidu.com
```

## Scp

```bash
scp ~/local root@192.168.xx.xx:~/remote
```

```bash
scp root@192.168.xx.xx:~/remote ~/local
```

# NTP 服务

```bash
sudo apt install ntp
sudo ntpq -pn #查看当前服务器
```

```bash
sudo service ntp stop
sudo ntpd -gq
sudo service ntp start
```

```bash
sudo cp /usr/share/zoneinfo/Asia/Shanghai /etc/localtime
```

## GPIO

```bash
echo 21 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio21/direction
echo 1 > /sys/class/gpio/gpio21/value
#echo 21 > /sys/class/gpio/unexport #使用完删除端口
```

## device tree overlay  —配置设备树以开启某些功能

/dev/config.txt

## UART

```bash
stty -F /dev/tty* 115200
echo "hello" > /dev/tty*
cat /dev/tty*
```

## Bluetooth

```bash
#显示全部蓝牙设备信息
hciconfig -a 
#开启/关闭/复位 hci0 蓝牙设备
sudo hciconfig hci0 up(/down/reset)
#查看蓝牙模块信息
hcitool dev
#查询周围蓝牙设备
hcitool scan
#配置蓝牙可发现
sudo hciconfig hci0 piscan
#配置蓝牙名称(重启 BLE 后可见)
sudo hciconfig hci0 name "MyBLE"

#bluetoothctl
discoverable yes #设置蓝牙可见
```

## Pybluez 开发

[https://github.com/pybluez/pybluez](https://github.com/pybluez/pybluez)

1. 搭建环境

```bash
sudo apt install python3-dev libbluetooth-dev bluetooth bluez
pip install pybluez
```

编辑 /etc/bluetooth/main.conf 加入一行：

```bash
disableplugins=pnat
```

1. 生成 UUID

```bash
>>> import uuid
>>> uuid.uuid1()
UUID('83009328-84a3-11ec-975a-e45f0162d0a2')
```

1. 编写代码

<aside>
💡  [Errno 2] No such file or directory 解决办法：修改 Bluetooth 服务文件，以 -C 兼容模式启动，最后重启一下服务即可

</aside>

```python
### Blue_Server.py
import bluetooth

server_sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
server_sock.bind(("", bluetooth.PORT_ANY))
server_sock.listen(1)

port = server_sock.getsockname()[1]

uuid = "94f39d29-7d6d-437d-973b-fba39e49d4ee"

bluetooth.advertise_service(server_sock,
                            "MyBLE",
                            service_id=uuid,
                            service_classes=[uuid, bluetooth.SERIAL_PORT_CLASS],
                            profiles=[bluetooth.SERIAL_PORT_PROFILE],
                            # protocols=[bluetooth.OBEX_UUID]
                            )

print("Waiting for connection on RFCOMM channel", port)

client_sock, client_info = server_sock.accept()
print("Accepted connection from", client_info)

try:
    while True:
        data = client_sock.recv(1024)
        if not data:
            break
        print("Received", data)
except OSError:
    pass

print("Disconnected.")

client_sock.close()
server_sock.close()
print("All done.")
```

```python
### Blue_Client.py
import sys
import bluetooth

addr = None
if len(sys.argv) < 2:
    print("No device specified. Searching all nearby bluetooth devices for "
          "the Server service...")
else:
    addr = sys.argv[1]
    print("Searching for Server on {}...".format(addr))

# search for the SampleServer service
uuid = "94f39d29-7d6d-437d-973b-fba39e49d4ee"
service_matches = bluetooth.find_service(uuid=uuid, address=addr)

if len(service_matches) == 0:
    print("Couldn't find the Server service.")
    sys.exit(0)

first_match = service_matches[0]
port = first_match["port"]
name = first_match["name"]
host = first_match["host"]

print("Connecting to \"{}\" on {}".format(name, host))

# Create the client socket
sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
sock.connect((host, port))

print("Connected. Type something...")
while True:
    data = input()
    if not data:
        break
    sock.send(data)

sock.close()
```

## Camera

- Motion 动作捕捉
  
    ```bash
    sudo apt install motion -y
    #/etc/default/motion
    start_motion_daemon=yes
    #/etc/motion/motion.conf
    daemon on
    stream_localhost off
    framerate 30
    sudo service motion start
    ```