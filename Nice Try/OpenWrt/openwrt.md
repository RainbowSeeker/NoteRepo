# OpenWrt 固件自编译教程：从入门到酸爽！

​	个人非常不建议在国内的本地环境进行固件编译（自备美国区富强全局模式），若非使用 GitHub Ac­tions 进行云编译，那么我强烈推荐首选存储空间不要少于 40GB 的美国区 VPS 作为编译环境，我遇到的很多坑都是墙大环境造成的，以此为鉴。

## 编译环境准备

### 创建用户

不推荐使用 `root` 用户进行编译，所以先使用 `root` 权限创建一个用户，以创建 `helloworld` 用户为例。

```none
# 添加用户
adduser helloworld
```

为新用户设置密码

```none
passwd helloworld
```

为该用户指定命令解释程序

```none
usermod -s /bin/bash helloworld
```

为该用户添加 `sudo` 权限

```none
chmod u+w /etc/sudoers
```

因 `/etc/sudoers` 文件默认是只读文件，因此需先添加 `sudoers` 文件的写权限

```none
chmod u+w /etc/sudoers
```

编辑 `sudoers` 文件

```none
vim /etc/sudoers

#找到这行 root ALL=(ALL) ALL 追加新行
helloworld ALL=(ALL:ALL) ALL
```

保存修改后，安全考虑建议撤销 `sudoers` 文件写权限

```none
chmod u-w /etc/sudoers
```

创建新用户会自动创建一个以用户名命名的新目录，需要为新目录添加读写权限

```none
chown helloworld:helloworld -R /home/helloworld/
```

从 `root` 用户切换使用新用户

```none
# 运行两次，并输入新用户的密码进行切换
su helloworld
```

从此刻开始，以非 `root` 用户执行编译过程的所有操作。

### 更新本地环境

```none
# 更新软件列表、升级软件包
sudo sh -c "apt update && apt upgrade -y"
```

### 安装依赖

ubuntu 20.04 （推荐系统），安装依赖

```none
sudo apt-get -y install build-essential asciidoc binutils bzip2 gawk gettext git libncurses5-dev libz-dev patch python3 python2.7 unzip zlib1g-dev lib32gcc1 libc6-dev-i386 subversion flex uglifyjs git-core gcc-multilib p7zip p7zip-full msmtp libssl-dev texinfo libglib2.0-dev xmlto qemu-utils upx libelf-dev autoconf automake libtool autopoint device-tree-compiler g++-multilib antlr3 gperf wget curl swig rsync
```

ubuntu 18.04 安装依赖

```none
sudo apt-get -y install build-essential asciidoc binutils bzip2 gawk gettext git libncurses5-dev libz-dev patch unzip zlib1g-dev lib32gcc1 libc6-dev-i386 subversion flex uglifyjs gcc-multilib g++-multilib p7zip p7zip-full msmtp libssl-dev texinfo libglib2.0-dev xmlto qemu-utils upx libelf-dev autoconf automake libtool autopoint device-tree-compiler antlr3 gperf wget swig rsync
```

## 首次编译步骤

> 不要使用 `root` 用户！

### 拉取源码

拉取源码到编译环境

```none
git clone https://github.com/coolsnowwolf/lede.git openwrt
```

### 添加自定义源

**注：**以下是自己正在使用的软件源，这里你也可以换成自己喜欢的软件源

```none
# 进入openwrt目录
cd ~/openwrt

# 添加自定义源
cat >> feeds.conf.default <<EOF
src-git kenzo https://github.com/kenzok8/openwrt-packages
src-git passwall https://github.com/xiaorouji/openwrt-passwall
EOF
```

更新下载软件仓库的源码到本地，并安装自定义源包含的软件包

```none
./scripts/feeds update -a && ./scripts/feeds install -a
```

### 单独添加软件包（可选项）

如果需要单独添加软件，请拉取源码到 pack­age 目录之下。示例如下：

```none
# 添加 OpenAppFilter 应用过滤插件
git clone https://github.com/destan19/OpenAppFilter package/OpenAppFilter
```

### 设置默认主题（可选项）

注：此操作请在编译前执行，设置 ar­gon 为默认主题，以此为例举一反三

```none
# 删除自定义源默认的 argon 主题
rm -rf package/lean/luci-theme-argon

# 拉取 argon 原作者的源码
git clone -b 18.06 https://github.com/jerrykuku/luci-theme-argon.git package/lean/luci-theme-argon

# 替换默认主题为 luci-theme-argon
sed -i 's/luci-theme-bootstrap/luci-theme-argon/' feeds/luci/collections/luci/Makefile
```

### 设置路由器默认的 LAN IP（可选项）

注：此操作请在编译前执行

```none
# 设置默认IP为 192.168.199.1
sed -i 's/192.168.1.1/192.168.199.1/g' package/base-files/files/bin/config_generate
```

### **仅制作.config文件，无需本地编译固件**（可选项）

根据自己的定制需求，调整 [Open­Wrt](https://www.moewah.com/tag/openwrt/) 系统组件

```none
make menuconfig
```

通过以下命令行获得 `seed.config` 配置文件，然后使用 GitHub Ac­tions 云编译。

```none
# 若在调整OpenWrt系统组件的过程有多次保存操作，则建议先删除.config.old文件再继续操作
rm -f .config.old

# 根据编译环境生成默认配置
make defconfig

# 对比默认配置的差异部分生成配置文件（可以理解为增量）
./scripts/diffconfig.sh > seed.config
```

### **若需要编译固件**

根据自己的定制需求，调整 [Open­Wrt](https://www.moewah.com/tag/openwrt/) 系统组件

```none
make menuconfig
```

下载编译所需的软件包

```none
make download -j8 V=s
```

编译 Open­Wrt 固件

```none
# -j1：单线程编译
make -j1 V=s
```

单线程编译可提高编译成功率，但过程非常漫长，也完全取决于机器的性能。编译完成后固件输出路径： `/openwrt/bin/targets/` 之下。

## 二次编译步骤

> 不要使用 `root` 用户！

更新本地编译环境

```none
# 更新软件列表、升级软件包
sudo sh -c "apt update && apt upgrade -y"

# 拉取最新源码
cd ~/openwrt && git pull

# 更新下载安装订阅源包含的软件包
cd ~/openwrt
./scripts/feeds update -a && ./scripts/feeds install -a
```

清理旧文件

```none
# 删除/bin和/build_dir目录中的文件
make clean
```

如果要更换架构，例如要从 `x86_64` 换到 `MediaTek Ralink MIPS` 建议执行以下命令深度清理 `/bin` 和 `/build_dir` 目录的中的文件 (`make clean`) 以及 `/staging_dir`、`/toolchain`、`/tmp` 和 `/logs` 中的文件。

```none
make dirclean
```

如果需要对组件重新调整，则建议先删除旧配置

```none
rm -rf ./tmp && rm -rf .config
```

根据自己的定制需求，再次调整 Open­Wrt 系统组件

```none
make menuconfig
```

下载编译所需的软件包

```none
make download -j8 V=s
```

编译 Open­Wrt 固件

```none
make -j$(nproc) || make -j1 || make -j1 V=s
```

二次编译可以优先使用多线程，报错会自动使用单线程，仍然报错会单线程执行编译并输出详细日志。

## 附加内容

如果没有特殊的定制需求，运行 `make menuconfig` 命令配置目标架构完成后，进入 `LUCI --> Applications` 选择需要的插件，进入 `LUCI --> Themes` 选择喜欢的主题皮肤，编译即可。

如果是硬路由务必记得勾选 `luci-app-mtwifi` 闭源的 Wi-Fi 驱动

### 1. x86_64 平台

```none
Target System (x86)  --->
Subtarget (x86_64)  --->
Target Profile (Generic x86/64)  --->
```

无特殊需求，那么 `Target Images` 勾选

```none
[*] squashfs (NEW)
[*] Build GRUB images (Linux x86 or x86_64 host only) (NEW)
[*] GZip images (NEW)
```

自定义固件大小

```none
(32) Kernel partition size (in MB) (NEW)
(400) Root filesystem partition size (in MB) (NEW)
```

如果因为勾选基于 EXT4 文件系统编译固件而造成编译报错，建议调高 `Root filesystem partition size`

### 2. 常用插件列表

在调整 Open­Wrt 系统组件时：`[*] `表示编译软件包并加入到固件；`[M]` 表示编译软件包但不加入固件；`[ ]` 则表示不对软件包进行任何设置。

```none
## 插件类
LuCI ---> Applications ---> luci-app-adbyby-plus   #去广告
LuCI ---> Applications ---> luci-app-arpbind  #IP/MAC绑定
LuCI ---> Applications ---> luci-app-autoreboot  #高级重启
LuCI ---> Applications ---> luci-app-aliddns   #阿里DDNS客户端
LuCI ---> Applications ---> luci-app-ddns   #动态域名 DNS
LuCI ---> Applications ---> luci-app-filetransfer  #文件传输
LuCI ---> Applications ---> luci-app-firewall   #添加防火墙
LuCI ---> Applications ---> luci-app-frpc   #内网穿透 Frp
LuCI ---> Applications ---> luci-app-mwan3   #MWAN负载均衡
LuCI ---> Applications ---> luci-app-nlbwmon   #网络带宽监视器
LuCI ---> Applications ---> luci-app-ramfree  #释放内存
LuCI ---> Applications ---> luci-app-samba   #网络共享(Samba)
LuCI ---> Applications ---> luci-app-sqm  #流量智能队列管理(QOS)
LuCI ---> Applications ---> luci-app-openclash #你懂的那只猫
LuCI ---> Applications ---> luci-app-dnsfilter #广告过滤
LuCI ---> Applications ---> luci-app-passwall #不敢解释
LuCI ---> Applications ---> luci-app-mtwifi #闭源Wi-Fi驱动
LuCI ---> Applications ---> luci-app-eqos #根据IP控制网速
LuCI ---> Applications ---> luci-app-syncdial #薛定谔的多拨应用
LuCI ---> Applications ---> luci-app-zerotier #虚拟局域网
LuCI ---> Applications ---> luci-app-jd-dailybonus #京东签到自动领豆
LuCI ---> Applications ---> luci-app-oaf #应用过滤神器
LuCI ---> Applications ---> luci-app-watchcat #断网检测功能与定时重启
LuCI ---> Applications ---> luci-app-wol   #WOL网络唤醒
LuCI ---> Applications ---> luci-app-wrtbwmon  #实时流量监测
LuCI ---> Applications ---> luci-app-upnp   #通用即插即用UPnP(端口自动转发)
LuCI ---> Applications ---> luci-app-argon-config #Argon主题设置

# 常用主题类
LuCI ---> Themes ---> luci-theme-argon

# 网络相关 (普通用户用不上）
Network ---> IP Addresses and Names ---> ddns-scripts_cloudflare.com-v4
Network ---> IP Addresses and Names --->  bind-dig
Network ---> Routing and Rediction ---> ip-full
Network ---> File Transfer ---> curl
Network ---> File Transfer ---> wget-ssl
Network ---> iperf3
Network ---> ipset
Network ---> socat #多功能的网络工具
Base system --> dnsmasq-full #DNS缓存和DHCP服务（dnsmasq-full和dnsmasq二者不可共存）

# 工具类 (普通用户用不上）
Utilities --> acpid  #电源管理接口（适用于x86平台）
Utilities --> Editors --> nano #Nano 编辑器
Utilities --> Shells --> bash #命令解释程序
Utilities --> disc --> fdisk #MBR分区工具
Utilities --> disc --> gdisk #GBT分区工具
Utilities --> disc --> lsblk #列出磁盘设备及分区查看工具
Utilities --> Filesystem --> resize2fs #调整文件系统大小
Utilities --> Filesystem --> e2fsprogs #Ext2（及Ext3/4）文件系统工具

# IPv6（未来运营商可能不再提供 IPv4 公网地址，有远程访问需求的建议加入）
Extra packages ---> ipv6helper （勾选此项即可，下面几项自动勾选）
Network ---> odhcp6c
Network ---> odhcpd-ipv6only
LuCI ---> Protocols ---> luci-proto-ipv6
LuCI ---> Protocols ---> luci-proto-ppp
```

### 3. 为 OpenWrt 单独编译 ipk 插件

相关文章，请移步 -> [《为 OpenWrt 单独编译 ipk 插件》](https://www.moewah.com/archives/4658.html)

### 4. OpenWrt 新手配置使用指南

相关文章，请移步 -> [《OpenWrt 新手配置使用指南》](https://www.moewah.com/archives/4054.html)

------

## 相关推荐

- [将 OpenWrt 设置为交换机 / AP 模式](https://www.moewah.com/archives/4977.html)
- [Tailscale：基于 WireGuard 异地组建虚拟局域网](https://www.moewah.com/archives/4796.html)
- [OpenWrt 存储空间扩容的两种方案](https://www.moewah.com/archives/4719.html)
- [OpenWrt 远程访问：DDNS 动态解析和 ZeroTier 虚拟局域网](https://www.moewah.com/archives/4106.html)
- [群晖 VMM 虚拟机安装 OpenWrt 配置旁路由](https://www.moewah.com/archives/4697.html)
- [Proxmox VE 虚拟机安装 OpenWrt 配置旁路由教程](https://www.moewah.com/archives/3643.html)

