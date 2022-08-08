# Chapter 5 相机与图像

## 1.opencv基本操作

### 1.1 安装依赖

```bash
sudo apt install build-essential libgtk2.0-dev libvtk6-dev libjpeg-dev libtiff-dev libopenexr-dev libtbb-dev
```

编译安装 opencv

```bash
mkdir build &&cd build
cmake ..
make -j8
sudo make install
```

