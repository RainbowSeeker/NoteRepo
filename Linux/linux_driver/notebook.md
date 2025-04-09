# Notebook

## Chapter 1

### 1. 内存页的缓存属性
CPU 缓存行为由页表项的缓存策略控制：

| 内存类型            | 缩写 | 特性描述                                                                 |
|---------------------|------|--------------------------------------------------------------------------|
| **Write-Back**      | WB   | 默认缓存模式，写操作先更新缓存后异步回写内存                              |
| **Uncached**        | UC   | 禁用缓存，直接访问物理内存（用于设备寄存器/DMA）                          |
| **Write-Combining** | WC   | 写合并优化，延迟批量写入（适用于显卡帧缓冲区）                            |
| **Write-Through**   | WT   | 同时更新缓存和内存，保证强一致性（性能较差）                              |

> 判断缓存状态
> ```c
> #include <linux/mm.h>
> 
> int is_uncached(void *vaddr) {
>     pgprot_t prot = vm_get_page_prot(vaddr); 
>     return (pgprot_val(prot) & _PAGE_PCD); // PCD=1 表示禁用缓存
> }
> Tips：需参考架构特定标志位（如 ARM 的 pgprot_noncached()）
> ```

常用内存分配接口的缓存策略：
| 分配函数	| 缓存策略 |
| -----  | -----  |
| `kmalloc()`	| WB（默认缓存） |
| `vmalloc()`	| WB（物理地址可能不连续） |
| `ioremap()`	| UC（默认非缓存） |
| `ioremap_wc()`	| WC（写合并） |
| `dma_alloc_coherent()`	| UC（DMA 一致性内存） |


<details>
<summary>设备树配置外设地址空间无缓存</summary>
```
reserved-memory {
    fpga_region: fpga@1F000000 {
        reg = <0x1F000000 0x100000>;
        no-map;
    };
};
```
</details>

### 2. ioremap
用于将*物理内存地址*映射到*内核虚拟地址*空间。默认为无缓存（no-cache）。
```c
void __iomem *regs = ioremap(phys_addr, size);
```

> 思考：`copy_to_user` 与 `memcpy` 的替换问题？
> 答案：`copy_to_user` 相较于 `memcpy` 多了一个内存的合法性检验（uaccess）。当 `MMU` 存在或不存在时，都可以替换。但某些架构，如 arm64 在开启了 `CONFIG_ARM64_PAN` 或 `CONFIG_ARM64_SW_TTBR0_PAN` 配置后，直接访问用户空间地址会触发 kernel oop，详见 [html](http://www.wowotech.net/memory_management/454.html)。

### 3. kmalloc
用于分配的一段连续的物理内存，默认按 ARCH_KMALLOC_MINALIGN 对齐（32 位系统通常为 8 字节，64 位系统通常为 16 字节）。
若分配大小超过 ARCH_KMALLOC_MINALIGN，则按2的幂次对齐（例如分配 64 字节对齐 64 字节）。
```c
char *buf = kmalloc(size, GFP_KERNEL);
```
Flags 参数解释：
1. GFP_KERNEL
- 用途：在进程上下文（如系统调用、内核线程）中使用，适用于可以休眠（睡眠）的场景。

- 行为：
    - 允许内核通过回收内存（如页面换出、缓存清理）或等待 I/O 来满足分配请求。
    - 调用此标志时，当前线程可能进入休眠状态，触发调度。

- 限制：不可用于原子上下文（如中断处理、持有自旋锁时），否则会导致内核死锁或崩溃。

2. GFP_ATOMIC
- 用途：在原子上下文（如中断处理、定时器回调）中使用，分配时不允许休眠。

- 行为：
    - 内核会立即尝试分配内存，失败时直接返回 NULL。
    - 分配优先级高于 GFP_KERNEL，但可能浪费更多内存（如预留紧急内存池）。

- 限制：分配失败的概率较高，需检查返回值。

3. GFP_DMA
- 用途：为 DMA（直接内存访问）设备分配物理连续的低端内存（通常低于 16MB）。

- 行为：通常与 GFP_KERNEL 或 GFP_ATOMIC 组合使用（如 GFP_DMA | GFP_KERNEL）。

- 限制：可用内存区域有限，分配可能失败。

4. 其他常见标志
__GFP_ZERO：分配后将内存初始化为零。

GFP_USER：为用户空间分配内存（较少使用）。

GFP_HIGHUSER：为用户空间分配高端内存（适用于 32 位系统）。

### 4. Page
Page：物理内存被划分为固定大小的块（通常为 4KB），称为物理页（Physical Page）。
Linux 内核用 struct page 结构体描述每个物理页的元数据。每个物理页对应一个 struct page 实例。
```c
struct page {
    unsigned long flags;       // 页状态标志（如 PG_locked, PG_dirty）
    atomic_t _refcount;        // 引用计数
    struct list_head lru;      // 用于 LRU 链表（内存回收）
    void *virtual;             // 虚拟地址（若已映射）
    ... 
};
```
PFN（Page Frame Number，页帧号）：是一个整数，表示物理页在物理内存中的序号，等于物理地址除以页大小。
> 转换：pfn_to_page() / page_to_pfn() / page_address()
>
> pmap：查看程序内存分布

#### :star2: IMPORTANT 页、段、块、扇区概念对比
|概念	|含义	|大小	|操作主体	|主要用途|
|--|--|--|--|--|
|页（Page）	|**虚拟内存管理的最小单位**	|:arrow_up:4KB |CPU MMU	|虚拟内存映射、进程隔离	|
|段（Segment）	|程序在内存中的**逻辑划分**	|可变	|Compiler/OS	|程序内存布局（.text/.data）|
|块（Block）	|文件系统管理磁盘空间的**基本单位**	|4KB |File System	|文件存储和访问|
|扇区（Sector）	|物理存储设备的**最小物理读写单位**	|512B/4KB	|磁盘控制器	|磁盘物理读写|

#### 常见问题

1. 为什么页和块大小通常相同（如 4KB）？
   简化数据在内存和磁盘间的传输，避免多次拆分或合并操作。
2. 页与内存映射文件（mmap）的关系？
   通过 mmap 将文件直接映射到进程的虚拟地址空间，以页为单位按需加载到内存。
   **优势**：避免用户态和内核态的内存拷贝，适合大文件随机访问。
   **风险**：频繁映射小文件可能浪费内存（每个文件至少占用一页）。
3. 如何查看进程的地址空间段分布？`pmap <pid>` 或`cat /proc/<pid>/maps`。
4. 如何确认磁盘的扇区大小？`fdisk -l`
5. 调整块大小对性能的影响？大块减少元数据开销，适合大文件；小块节省空间，适合小文件。

#### :recycle: ​Page Table

- **内核空间**：所有进程共享内核页表（高地址空间，如 64系统`ffffffff80000000`）。
- **用户空间**：每个进程拥有独立的页表，低地址空间（如 `0x0000000000000000`）。
- **ARMv7**：

![img](/home/yangyu/.embedded_linux/linux_driver/.asset/屏幕快照 2020-02-05 15.18.37.png)

![img](/home/yangyu/.embedded_linux/linux_driver/.asset/屏幕快照 2020-02-05 15.33.03.png)

- **x86**：

![img](/home/yangyu/.embedded_linux/linux_driver/.asset/linux-memopt12.png)

### 5. 中断

- **上半部**：运行在硬件中断上下文中，需要快速执行，通常只记录状态或触发下半部。不能睡眠，不能调用可能阻塞的函数（如 copy_to_user/mutex_lock等）。
- **下半部**：运行在软中断或进程上下文中，可以处理耗时任务/睡眠。
#### softirq
softirq（软中断）是 Linux 内核中实现 ​下半部 的一种机制，适合处理高频率、低延迟的任务。
- **高优先级**：softirq 的优先级高于普通进程，但低于硬件中断。
- **​不能睡眠**：softirq 运行在软中断上下文中，**`不能调用可能导致睡眠的函数`**。
- **​静态分配**：softirq 的数量是固定的（Linux 内核预定义了 10 个 softirq）。
```c
enum
{
    HI_SOFTIRQ=0,       // 高优先级 tasklet
    TIMER_SOFTIRQ,      // 定时器
    NET_TX_SOFTIRQ,     // 网络发送
    NET_RX_SOFTIRQ,     // 网络接收
    BLOCK_SOFTIRQ,      // 块设备
    IRQ_POLL_SOFTIRQ,   // IRQ 轮询
    TASKLET_SOFTIRQ,    // 普通 tasklet
    SCHED_SOFTIRQ,      // 调度器
    HRTIMER_SOFTIRQ,    // 高精度定时器
    RCU_SOFTIRQ,        // RCU 回调
    NR_SOFTIRQS         // Softirq 总数
};
```
> softirq 用户不可用，只能调用 tasklet/workqueue 等类似的接口。
#### tasklet
tasklet 属于 softirq，适用于处理硬件中断触发的快速任务。
- **`不可睡眠`**，**`​原子性`**，**`不可重入`**，**`避免长时间运行`**

#### workqueue
运行在 **进程上下文**，由内核线程（如 kworker）执行，​适用于处理耗时任务。
- **`可睡眠`**，**`​高延迟`**，**`并发性`**

### 6. GPIO
```c
// register a pin controller device
struct pinctrl_dev *devm_pinctrl_register(struct device *dev,
					  struct pinctrl_desc *pctldesc,
					  void *driver_data);
// register a gpiochip with userdata
#define gpiochip_add_data(gc, data)
```
```dts
// register a `gpio-leds` device
leds: leds {
    status = "okay";
    compatible = "gpio-leds";

    sys_led: sys-led {
        label = "sys_led";
        linux,default-trigger = "heartbeat";
        default-state = "on";
        gpios = <&gpio4 RK_PB5 GPIO_ACTIVE_LOW>;
        pinctrl-names = "default";
        pinctrl-0 = <&sys_status_led_pin>;
    };
};
// cat /sys/class/gpio/gpiochipxxx/label
```
gpio nr 编号由具体平台设备驱动实现，计算公式为 `nr = gpiochip->base + offset`。其中 `gpiochip->base` 为 gpio 的 chip 控制器基址（不同于实际硬件地址，只是内核分配给 gpiochip 的一个编号），`offset`为具体 pin 在 chip 的偏移（\in [0, ngpio-1]）。
> 当 gpiochip->base 指定为 -1 时， gpiochip_add_data 会自动计算基址，即为 ARCH_NR_GPIOS 开始向前占据 ngpio 个编号。

[WorkFlow](./ch2_platform/1_led.c):
```c
pdata->cdev && devm_led_classdev_register_ext
```

### 7. Input
用于监听和上报来自硬件的输入信号，如键盘\鼠标等。
[WorkFlow](./ch2_platform/2_input.c): 
```c
devm_input_allocate_device 
    -> set_bit && input_register_device 
        -> input_report_key && input_sync(in isr) 
            -> input_unregister_device
```
See [input.c](ch2_platform/2_input.c)。
- `devm_*` 开头的函数用于在设备remove时自动释放自身资源（RAII 思想）。
- `delayed_work` or `timer` 用于按键消抖。

### 8. I2C
硬件原理见 [i2c](https://doc.embedfire.com/linux/rk356x/driver/zh/latest/linux_driver/subsystem_i2c.html)。
当设备树上的节点和具体的 I2C 驱动匹配时，会调用驱动的 `probe` 函数，并为你提供 `i2c_client` 字段，包含 `i2c_adapter` 主机控制器设备，子设备地址 `addr` 等。
[WorkFlow](./ch2_platform/3_i2c.c): 
```c
// Send
int i2c_transfer(struct i2c_adapter *adap, struct i2c_msg *msgs, int num);
// or
int i2c_master_send(const struct i2c_client *client, const char *buf, int count)
int i2c_master_recv(const struct i2c_client *client, char *buf, int count)
```

### 9. SPI

### 10. FrameBuffer
用于视频输出设备的显示缓冲区。结合 `mmap` 可以实现用户空间操作单点像素。
[WorkFlow](./ch2_platform/3_i2c.c):
```c
framebuffer_alloc (fbops, fix, var, screen_base, fbdefio)
    -> dma_alloc_coherent / devm_kmalloc
    -> fb_deferred_io_init
    -> register_framebuffer
        -> fb_write     // write buf to fb
        -> deferred_io  // update display
            -> unregister_framebuffer
            -> fb_deferred_io_cleanup
            -> dma_free_coherent
```

### 11. Filesystem For Nor Flash
Nor Flash 可用的文件系统有 `jffs2`，`ubifs`，`fatfs`。
```dts
w25q64: flash@0 {
    status = "okay";
    compatible = "jedec,spi-nor";
    reg = <0>;
    spi-max-frequency = <8000000>;

    #address-cells = <1>;
    #size-cells = <1>;
    partition@0 {
        label = "bootloader";
        reg = <0x00000000 0x100000>; // 1 MB
        read-only;
    };
    partition@1 {
        label = "kernel";
        reg = <0x100000 0x300000>; // 3 MB
    };
    partition@2 {
        label = "rootfs";
        reg = <0x400000 0x400000>; // 4 MB
    };
};
```
```bash
cat /proc/mtd               # 查看所有mtd设备
sudo mtdinfo /dev/mtd0      # 查看具体mtd设备信息
sudo mtd_debug [info]/[read]/[write]/[erase] # mtd 操作命令
sudo flash_erase /dev/mtd0 0 0  # 擦除全部

# Partition 0: uboot
sudo dd if=uboot.bin of=/dev/mtd0
# Partition 1: kernel
sudo dd if=vmlinuz of=/dev/mtd1
# Partition 2: rootfs
#   FS choose 1 -> FAT32
sudo mkfs.vfat /dev/mtdblock2
sudo mount -t vfat /dev/mtdblock2 /mnt/flash
#   FS choose 2 -> JFFS2 (NOT RECOMMEND)
sudo mkfs.jffs2 -e 8KiB -d ~/test -o /dev/mtdblock2
sudo mount -t jffs2 /dev/mtdblock2 /mnt/flash
```

### 12. USB

- OTG (On-The-Go)：使同一设备既能充当主机（HOST），也能作为外设（DEVICE）。

#### Function List For USB Device (Gadget)
| 功能名称         | 描述                             | 内核配置                  | 模块             |
| ---------------- | -------------------------------- | ------------------------- | ---------------- |
| **​Mass Storage​**​ | 模拟U盘/存储设备                 | `CONFIG_USB_MASS_STORAGE` | `g_mass_storage` |
| **​CDC ACM**​      | 模拟串口通信设备（**Serial**）   | `CONFIG_USB_G_SERIAL`     | `g_serial`       |
| **​CDC ECM​**​      | 模拟CDC以太网设备（**RNDIS**）   | `CONFIG_USB_ETH`          | `g_ether`        |
| **​UVC​**​          | 模拟USB摄像头                    | `CONFIG_USB_G_WEBCAM`     | `g_webcam`       |
| HID​​              | 模拟键盘/鼠标等输入设备          | `CONFIG_USB_G_HID`        | `g_hid`          |
| MTP​​              | 媒体传输协议（手机文件传输）     | `CONFIG_USB_CONFIGFS`     | `configfs`       |
| MIDI​​             | 模拟MIDI音乐设备                 | `CONFIG_USB_MIDI_GADGET`  | `g_midi`         |
| Printer​​          | 模拟USB打印机                    | `CONFIG_USB_G_PRINTER`    | `g_printer`      |
| Audio​​            | 模拟USB音频设备                  | `CONFIG_USB_AUDIO`        | `g_audio`        |
| FunctionFS​​       | 用户空间自定义功能               | `CONFIG_USB_FUNCTIONFS`   | `g_ffs`          |
| NCM​​              | 新一代CDC网络控制模型            | `CONFIG_USB_G_NCM`        | `g_ncm`          |
| **MS && ACM**    | Mass Storage + CDC ACM..         | `CONFIG_USB_G_ACM_MS`     | `g_acm_ms`       |
| **​g_multi​**​      | Mass Storage + RNDIS + CDC ACM.. | `CONFIG_USB_G_MULTI`      | `g_multi`        |

- Way 1: modprobe

```bash
sudo modprobe g_acm_ms file=/dev/mtdblock2 removable=y
sudo modprobe g_multi \
    file=/dev/mtdblock2 \  			# Mass Storage 使用的磁盘镜像
    host_addr=12:34:56:78:9a:bc \  	# 设备端 MAC Address
    dev_addr=de:ad:be:ef:12:34 \   	# 主机端 MAC Address
    iSerialNumber=12345678 \       	# Serial Number
    idVendor=0x1d6b \              	# Vendor ID
    idProduct=0x0104               	# Product ID
```

- Way 2: configfs

```bash
#!/bin/bash
set -e

if [ "$EUID" -ne 0 ]; then
  echo "Must be root"
  exit 1
fi

# mount configfs
mountpoint /sys/kernel/config > /dev/null || mount -t configfs none /sys/kernel/config

# clear old config
if [ -d "/sys/kernel/config/usb_gadget/g1" ]; then
	cd /sys/kernel/config/usb_gadget/g1 || exit 1
	rm -rf configs/c.1/*
	rm -rf functions/*
	cd .. && rmdir g1
fi

cd /sys/kernel/config/usb_gadget
mkdir g1 && cd g1
echo 0x1d6b > idVendor   # Linux Foundation Vendor ID
echo 0x0104 > idProduct  # Multifunction Composite Gadget
echo 0x0100 > bcdDevice  # Version v1.0.0
echo 0x0200 > bcdUSB     # USB 2.0
mkdir -p strings/0x409
echo "MySerialDevice" > strings/0x409/manufacturer
echo "USB Serial Gadget" > strings/0x409/product
echo "12345678" > strings/0x409/serialnumber
mkdir -p configs/c.1/strings/0x409
echo "Serial Config" > configs/c.1/strings/0x409/configuration
mkdir -p functions/acm.usb0  # 创建ACM（Abstract Control Model）串口
ln -s functions/acm.usb0 configs/c.1/
ls /sys/class/udc/ > UDC
```

