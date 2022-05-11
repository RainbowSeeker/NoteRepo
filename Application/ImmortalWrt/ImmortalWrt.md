# ImmortalWrt

[GitHub - immortalwrt/immortalwrt: An opensource OpenWrt variant for mainland China users.](https://github.com/immortalwrt/immortalwrt)

<aside>
💡 Default login address: [http://192.168.1.1](http://192.168.1.1/) or [http://immortalwrt.lan](http://immortalwrt.lan/), 
username: **root**, password: **password**.

</aside>

### 1. 安装环境

- Method 1: via APT
    
    ```bash
    sudo apt update -y
    sudo apt install -y ack antlr3 asciidoc autoconf automake autopoint binutils bison build-essential \
      bzip2 ccache cmake cpio curl device-tree-compiler ecj fastjar flex gawk gettext gcc-multilib g++-multilib \
      git gperf haveged help2man intltool lib32gcc1 libc6-dev-i386 libelf-dev libglib2.0-dev libgmp3-dev libltdl-dev \
      libmpc-dev libmpfr-dev libncurses5-dev libncursesw5 libncursesw5-dev libreadline-dev libssl-dev libtool lrzsz \
      mkisofs msmtp nano ninja-build p7zip p7zip-full patch pkgconf python2.7 python3 python3-pip python3-ply \
      python-docutils qemu-utils re2c rsync scons squashfs-tools subversion swig texinfo uglifyjs upx-ucl unzip \
      vim wget xmlto xxd zlib1g-dev
    ```
    
- Method 2:
    
    ```bash
    curl -s https://build-scripts.immortalwrt.eu.org/init_build_environment.sh | sudo bash
    ```
    

### 2.编译 immortalwrt 安装包

```bash
git clone -b openwrt-18.06 --single-branch https://github.com/immortalwrt/immortalwrt
```

```bash
cd immortalwrt
./scripts/feeds update -a
./scripts/feeds install -a
```

```bash
make menuconfig
make
```