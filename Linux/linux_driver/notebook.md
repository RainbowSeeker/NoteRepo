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

pmap