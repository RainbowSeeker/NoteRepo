# Anaconda

### 1.创建新的python环境

```bash
conda info --env							#查看现有环境
conda create -n <your_name> python=3.6		#新建环境
conda remove -n <your_name> --all			#删除环境
```

> 安装pytorch，参见：https://pytorch.org/get-started/locally/#windows-anaconda
> 

> 换源：修改 ~/.condarc 为
>

```bash
channels:
  - http://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/main/
  - http://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/free/
  - http://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud/conda-forge/
  - http://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud/Paddle/
  - http://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud/fastai/
  - http://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud/pytorch/
  - http://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud/bioconda/
show_channel_urls: true
ssl_verify: false
```

### 2.激活环境

```bash
conda activate <your_name> #开启
conda deactivate           #关闭
```

### 3.安装jupyter环境

```bash
conda install nb_conda			#(base)
pip install jupyter ipython		#(your_env)
```

### 4.启动jupyter

```bash
jupyter notebook
```