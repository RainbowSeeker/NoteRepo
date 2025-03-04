# PVE

## 安装PVE系统

### 换国内源：

PVE换源

```bash
echo "#deb https://enterprise.proxmox.com/debian/pve bullseye pve-enterprise" > /etc/apt/sources.list.d/pve-enterprise.list
echo "deb https://mirrors.tuna.tsinghua.edu.cn/proxmox/debian bullseye pve-no-subscription" > /etc/apt/sources.list.d/pve-no-subscription.list
```

Debian换源

```bash
mv /etc/apt/sources.list /etc/apt/sources.list.bk
vi /etc/apt/sources.list
```

```bash
# 默认注释了源码镜像以提高 apt update 速度，如有需要可自行取消注释
deb https://mirrors.tuna.tsinghua.edu.cn/debian/ bullseye main contrib non-free
# deb-src https://mirrors.tuna.tsinghua.edu.cn/debian/ bullseye main contrib non-free
deb https://mirrors.tuna.tsinghua.edu.cn/debian/ bullseye-updates main contrib non-free
# deb-src https://mirrors.tuna.tsinghua.edu.cn/debian/ bullseye-updates main contrib non-free

deb https://mirrors.tuna.tsinghua.edu.cn/debian/ bullseye-backports main contrib non-free
# deb-src https://mirrors.tuna.tsinghua.edu.cn/debian/ bullseye-backports main contrib non-free

deb https://mirrors.tuna.tsinghua.edu.cn/debian-security bullseye-security main contrib non-free
# deb-src https://mirrors.tuna.tsinghua.edu.cn/debian-security bullseye-security main contrib non-free
```

CT Templates换源

```bash
cp /usr/share/perl5/PVE/APLInfo.pm /usr/share/perl5/PVE/APLInfo.pm_back
sed -i 's|http://download.proxmox.com|https://mirrors.tuna.tsinghua.edu.cn/proxmox|g' /usr/share/perl5/PVE/APLInfo.pm
```

##  开启硬件直通

1. `vi /etc/default/grub`

![image-20220607205624849](.\image-20220607205624849.png)

2. `vi /etc/modules`

```
vfio
vfio_iommu_type1
vfio_pci
vfio_virqfd
```

```
update-grub
update-initramfs -u -k all
reboot
```



```
qm importdisk 101 /var/lib/vz/template/iso/x86.img local-lvm
```



### 为容器加入渲染器硬件，并关闭AppArmor（部分显卡可能需要更新内核才能找到渲染器）

```
vi /etc/pve/lxc/[CT_ID].conf
```

加入硬件参数：（可先用ls -l /dev/dri查询）

```
lxc.cgroup2.devices.allow: c 226:0 rwm
lxc.cgroup2.devices.allow: c 226:128 rwm
lxc.cgroup2.devices.allow: c 29:0 rwm
lxc.mount.entry: /dev/dri dev/dri none bind,optional,create=dir
lxc.mount.entry: /dev/fb0 dev/fb0 none bind,optional,create=file
lxc.apparmor.profile: unconfined
```



```bash
pct create 200 \
        local:vztmpl/openwrt-x86-64-ipv6-generic-rootfs.tar.gz \
        --rootfs local-lvm:8 \
        --ostype unmanaged \
        --hostname RainWrt \
        --arch amd64 \
        --cores 4 \
        --memory 2048 \
        --swap 0 \
        -net0 bridge=vmbr0,name=eth0
```



## docker

1. onekey
```
apt install curl -y
curl -sSL https://get.daocloud.io/docker | sh
```
2. 换源
```
vi /etc/docker/daemon.json
```

```
{
  "registry-mirrors": [
    "https://hub-mirror.c.163.com",
    "https://ustc-edu-cn.mirror.aliyuncs.com",
    "https://ghcr.io",
    "https://mirror.baidubce.com"
  ]
}
```

```
service docker restart
```
3. 安装 `portainer`
```bash
docker volume create portainer_data
docker run -d -p 8000:8000 -p 9000:9000 --name=portainer --restart=always -v /var/run/docker.sock:/var/run/docker.sock -v portainer_data:/data portainer/portainer-ce
```

## 挂载远程smb

安装SMB组件并创建共享目录（nas_share可自定义）

```
apt install cifs-utils -y
mkdir /mnt/nas_share
```

创建密码文件（注意保护文件，此处为明文密码）：

```
vi /root/.smbcredentials
```

设置SMB登录密码，自行替换：

```
username=smb_share
password=share_password
```

修改自动挂载文件

```
vi /etc/fstab
```

加入挂载位置，自行替换

```
//192.168.10.4/dsm_video0 /mnt/video0 cifs credentials=/root/.smbcredentials,iocharset=utf8,vers=1.0 0 0
```