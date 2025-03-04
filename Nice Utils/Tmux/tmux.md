# Tmux

Tmux 是一款终端复用命令行工具，一般用于 Terminal 的窗口管理。Tmux 拥有如下特性：

- 可以同时开启多个会话和窗口，并持久地保存工作状态。
- 断线后任务能够在后台继续执行。

## 使用 Tmux 管理会话

1. 进入 Tmux 环境

```shell
tmux
```

此时默认开启了一个 session-name 为 0 的 Tmux 会话。

此时如果您和服务器断开连接，tmux 中的任务还会继续保持。

2. 重新进入会话

```shell
tmux a -t 0 
```

其中 0 为之前会话的 session-name。

3. 退出会话

```shell
tmux detach
```

会回到普通的 Terminal。

4. 若想指定 session 名称以便记忆，可以使用命令：

```shell
tmux new -s [session-name]
```

5. 查看所有的 Tmux 会话：

```shell
tmux ls
```

6. 删除会话

```shell
tmux kill-session -t [session-name]		#删除指定会话
tmux kill-server						#删除所有会话
```

## **窗口管理**

在每个 session 会话中，您可以开启多个窗口和面板。Tmux 为了防止与全局快捷键冲突，大部分快捷键需要先需要输入前缀，默认为 Ctrl + b。该操作被定义为 Prefix。

| 快捷键            | 说明                                                         |      |
| :---------------- | :----------------------------------------------------------- | :--- |
| **Prefix "**      | **横向分隔窗格**                                             |      |
| **Prefix %**      | **纵向分隔窗格**                                             |      |
| **Prefix q**      | **显示分隔窗格的编号**                                       |      |
| **Prefix 上下键** | **上一个及下一个分隔窗格**                                   |      |
| Prefix ?          | 显示快捷键帮助                                               |      |
| **Prefix w**      | **以菜单方式显示及选择窗口**                                 |      |
| **Prefix c**      | **创建新窗口**                                               |      |
| **Prefix 0~9**    | **选择几号窗口**                                             |      |
| **Prefix n**      | **选择下一个窗口**                                           |      |
| **Prefix p**      | **选择前一个窗口**                                           |      |
| Prefix C-z        | 挂起会话，不影响其他命令的运行，C 表示 Ctrl 键               |      |
| Prefix C-o        | 调换窗格位置                                                 |      |
| Prefix 空格键     | 采用下一个内置布局                                           |      |
| Prefix !          | 把当前窗格（pane）变为新窗口（window）                       |      |
| Prefix C-方向键   | 调整分隔窗格大小，C 表示 Ctrl 键                             |      |
| Prefix z          | 最大化当前窗格，再一次则恢复                                 |      |
| Prefix l          | 切换到前一个窗口，该快捷键通常会被重定义为符合 vim 下的空格切换 |      |
| Prefix s          | 以菜单方式显示和选择会话                                     |      |
| Prefix t          | 显示时钟                                                     |      |
| Prefix ;          | 切换到最后一个使用的面板                                     |      |
| Prefix x          | 关闭面板                                                     |      |
| Prefix &          | 关闭窗口                                                     |      |
| Prefix d          | 退出 Tmux，并保存当前会话，此时 Tmux 仍在后台运行，可以通过 Tmux attach 进入指定的会话 |      |