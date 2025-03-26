#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/io.h>

#define MISC_NAME "my_misc_device"
#define MISC_SIZE (4 * PAGE_SIZE)    // 0x4000 -> 16KB

struct misc_device
{
    void   *buf;
    size_t  size;
};

int misc_open(struct inode *inode, struct file *file)
{
    int ret = 0;
    struct misc_device *dev = kzalloc(sizeof(struct misc_device), GFP_KERNEL);
    if (!dev)
    {
        ret = -ENOMEM;
        goto fail_dev;
    }

    dev->size = MISC_SIZE;
    dev->buf = kmalloc(dev->size, GFP_KERNEL);
    if (!dev->buf)
    {
        ret = -ENOMEM;
        goto fail_buf;
    }
    
    printk("alloc dev->buf, physical address: 0x%llx, virtual address: 0x%p\n", 
            virt_to_phys(dev->buf), dev->buf);

    memset(dev->buf, 0, dev->size);
    sprintf(dev->buf, "Hello from misc device\n");

    file->private_data = dev;

    return 0;

fail_buf:
    kfree(dev);
fail_dev:
    return ret;
}

int misc_release(struct inode *inode, struct file *file)
{
    struct misc_device *dev = file->private_data;
    kfree(dev->buf);
    kfree(dev);
    return 0;
}

ssize_t misc_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    struct misc_device *dev = file->private_data;
    return simple_read_from_buffer(buf, count, ppos, dev->buf, dev->size);
}

ssize_t misc_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    struct misc_device *dev = file->private_data;
    return simple_write_to_buffer(dev->buf, dev->size, ppos, buf, count);
}

static int misc_mmap(struct file *file, struct vm_area_struct *vma)
{
    struct misc_device *dev = file->private_data;
    unsigned long size = vma->vm_end - vma->vm_start;
    unsigned long pfn = virt_to_phys(dev->buf) >> PAGE_SHIFT;

    if (size > dev->size)
    {
        pr_warn("mmap size too large: %lu > %lu\n", size, dev->size);
        return -EINVAL;
    }
    
    // !!! 不能配合 kmalloc 申请的内存使用, 因为 kmalloc 申请的内存默认是 WB 的, 这样做会引发 Bus error. !!!
    // vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
    // vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);

    if (remap_pfn_range(vma, vma->vm_start, pfn, size, vma->vm_page_prot))
    {
        return -EAGAIN;
    }

    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = misc_open,
    .release = misc_release,
    .read = misc_read,
    .write = misc_write,
    .mmap = misc_mmap,
    .llseek = default_llseek,
};

static struct miscdevice my_miscdev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = MISC_NAME,
    .fops = &fops,
};

static int __init misc_init(void)
{
    return misc_register(&my_miscdev);
}

static void __exit misc_exit(void)
{
    misc_deregister(&my_miscdev);
}

module_init(misc_init);
module_exit(misc_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("RainbowSeeker");