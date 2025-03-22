# Linux Module Notebooks

## 0. Environment
```bash
sudo apt install qemu-system-arm

# kernel was configured in buildroot.
# buildroot
wget https://buildroot.org/downloads/buildroot-2024.02.6.tar.gz
tar -xvf buildroot-2024.02.6.tar.gz

make qemu_arm_vexpress_defconfig
# add: BR2_PACKAGE_OPENSSH (root password)
# set wget proxy: echo -e 'http_proxy=http://127.0.0.1:7890\nhttps_proxy=http://127.0.0.1:7890\nftp_proxy=http://127.0.0.1:7890' > ~/.wgetrc
make menuconfig
make -j$(nproc) 

# modify start-qemu.sh
sed -i 's/-net user/-net user,hostfwd=tcp::2222-:22/g' ./output/images/start-qemu.sh
# boot system
./output/images/start-qemu.sh
```