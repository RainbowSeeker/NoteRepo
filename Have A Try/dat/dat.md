# Dat 

## Introduction

​	[Dat is a protocol for sharing data between computers. By making sure changes in data are transparent, everyone receives only the data they want.](https://docs.datproject.org/docs/intro)

- **分布式网络**

- **数据备份**

- **安全**

#### Install

```bash
sudo apt install -y nodejs npm wget unzip			#nodejs -v > v6
sudo npm install -g dat
```

在`./.bashrc`末尾添加: `export PATH="$PATH:/home/yangyu/.dat/releases/dat-14.0.2-linux-x64"`，然后 `source ~/.bashrc`

## Command Line :dizzy: 

------

​	Share, download, and backup files with the command line! Automatically sync changes to datasets. Never worry about manually transferring files again.

#### 0.创建 dat(可选)： 

```bash
dat create [<dir>]
```

#### 1.共享文件

```bash
dat share <dir>
```

用于创建 dat 并将文件从计算机同步到其他用户。Dat 将公共链接、版本历史记录和文件信息存储在 `<dir>/.dat`中。

![image-20220421195016274](F:\Desktop\Notion\dat\image-20220421195016274.png)

#### 2.下载文件

```bash
dat clone dat://<link> <download-dir>
```

![image-20220421195640167](F:\Desktop\Notion\dat\image-20220421195640167.png)

#### 3.同步文件

```bash
dat pull [<dir>]					#下载完最新版后退出
dat sync [<dir>] [--no-import]		#实时同步内容 
```

[--no-import]: Use to not import any new or updated files.

#### 4.忽略/选择文件

`.datignore`：  指定忽略文件，同 `git`

`.datdownload`：指定下载文件，同 `git`

#### HTTP 文件演示 :stars: 

------

```bash
dat sync --http
```

> *Tips：使用 `localhost：8080/？version=10` 查看特定版本*
