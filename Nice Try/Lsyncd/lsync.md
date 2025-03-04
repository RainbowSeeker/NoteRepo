# Lsyncd

### 1.Rsync基本用法

```bash
rsync -avz [options] src/ dst/		#local
rsync -avz [options] src/ usr@ip:dst/	#remote
#options:
--progress		#参数表示显示进展
--exclude		#参数指定排除不进行同步的文件
--include		#参数用来指定必须同步的文件模式
--exclude-from	#参数指定一个本地文件，里面是需要排除的文件模式，每个模式一行
--delete		#删除只存在于目标目录、不存在于源目录的文件
--min-size		#参数设置传输的最小文件的大小限制
```

### 2.Lsyncd

```bash
lsyncd [OPTIONS] -rsync [SOURCE] [TARGET]					#普通传输
lsyncd [OPTIONS] -rsyncssh [SOURCE] [HOST] [TARGETDIR]		#通过ssh移动文件
lsyncd [OPTIONS] -direct [SOURCE] [TARGETDIR]				#本地文件同步

#OPTIONS:
  -delay SECS         #Overrides default delay times
  -insist             #Continues startup even if it cannot connect
  -nodaemon           #Does not detach and logs to stdout/stderr
```

#### Examples

https://iant.work/blog/740.html

1. 每隔2s同步文件

```bash
lsyncd -nodaemon -delay 2 -rsync . root@101.43.121.224:/root/sync
```

