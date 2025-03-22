#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kfifo.h>
#include <linux/slab.h>
#include <linux/atomic.h>
#include <linux/wait.h>
#include <linux/device.h>
#include <linux/poll.h>
#include <asm/uaccess.h>

#define CHAR_NAME    "chardev"
#define CHAR_MAJOR   0
#define FIFO_SIZE    1024

struct char_desc
{
    struct mutex lock;
    struct kfifo fifo;
    wait_queue_head_t r_wait;
    wait_queue_head_t w_wait;
    struct fasync_struct *async_queue;
};

struct char_device
{
#if CHAR_MAJOR
    struct cdev cdev;
#else
    int major;
#endif
    dev_t devno;
    struct class *class;
    struct device *device;
    
    atomic_t is_opened;
    struct char_desc *desc;
};

static char g_buf[] = "hello world from kernel\n";
static struct char_device g_chardev;

static int char_open(struct inode *inode, struct file *filp)
{
    printk("char_open\n");

    filp->private_data = &g_chardev;

    int ret = 0;
    struct char_device *dev = filp->private_data;

    if (atomic_cmpxchg(&dev->is_opened, 0, 1))
    {
        printk("device is busy\n");
        return -EBUSY;
    }

    dev->desc = kmalloc(sizeof(struct char_desc), GFP_KERNEL);
    if (!dev->desc)
    {
        printk("kmalloc failed\n");
        return -ENOMEM;
    }

    mutex_init(&dev->desc->lock);
    init_waitqueue_head(&dev->desc->r_wait);
    init_waitqueue_head(&dev->desc->w_wait);
    
    ret = kfifo_alloc(&dev->desc->fifo, FIFO_SIZE, GFP_KERNEL);
    if (ret)
    {
        printk("kfifo_alloc failed\n");
        kfree(dev->desc);
        return ret;
    }

    // fill some data
    kfifo_in(&dev->desc->fifo, g_buf, sizeof(g_buf));

    return 0;
}

static int char_release(struct inode *inode, struct file *filp)
{
    printk("char_release\n");

    struct char_device *dev = filp->private_data;
    struct char_desc *desc = dev->desc;

    kfifo_free(&desc->fifo);
    kfree(desc);

    atomic_set(&dev->is_opened, 0);
    return 0;
}

static ssize_t char_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    int ret = 0;
    unsigned int copied;
    unsigned long flags;
    struct char_device *dev = filp->private_data;
    struct char_desc *desc = dev->desc;

    printk("char_read\n");

    if (kfifo_is_empty(&desc->fifo))
    {
        if (filp->f_flags & O_NONBLOCK)
        {
            return -EAGAIN;
        }

        ret = wait_event_interruptible(desc->r_wait, !kfifo_is_empty(&desc->fifo));
        if (ret)
        {
            return ret;
        }
    }

    mutex_lock(&desc->lock);
    ret = kfifo_to_user(&desc->fifo, buf, count, &copied);
    mutex_unlock(&desc->lock);

    if (ret)
        return ret;

    if (kfifo_avail(&desc->fifo) > 0) {
        wake_up_interruptible(&desc->w_wait);
    }

    return copied;
}

static ssize_t char_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    int ret = 0;
    unsigned int copied;
    unsigned long flags;
    struct char_device *dev = filp->private_data;
    struct char_desc *desc = dev->desc;

    printk("char_write\n");

    if (kfifo_is_full(&desc->fifo))
    {
        if (filp->f_flags & O_NONBLOCK)
        {
            return -EAGAIN;
        }

        ret = wait_event_interruptible(desc->w_wait, !kfifo_is_full(&desc->fifo));
        if (ret)
        {
            return ret;
        }
    }

    mutex_lock(&desc->lock);
    ret = kfifo_from_user(&desc->fifo, buf, count, &copied);
    mutex_unlock(&desc->lock);

    if (ret)
    {
        return ret;
    }

    if (kfifo_len(&desc->fifo) > 0)
    {
        wake_up_interruptible(&desc->r_wait);

        kill_fasync(&desc->async_queue, SIGIO, POLL_IN);
    }

    return copied;
}

#define CHAR_MAGIC 'c'
#define CHAR_CLEAR _IO(CHAR_MAGIC, 0)

static long char_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct char_device *dev = filp->private_data;
    struct char_desc *desc = dev->desc;

    switch (cmd)
    {
    case CHAR_CLEAR:
        mutex_lock(&desc->lock);
        kfifo_reset(&desc->fifo);
        mutex_unlock(&desc->lock);
        break;
    default:
        return -ENOTTY;
    }

    return 0;
}

static unsigned int char_poll(struct file *filp, poll_table *wait)
{
    unsigned int mask = 0;
    struct char_device *dev = filp->private_data;
    struct char_desc *desc = dev->desc;

    poll_wait(filp, &desc->r_wait, wait);
    poll_wait(filp, &desc->w_wait, wait);

    if (!kfifo_is_empty(&desc->fifo))
    {
        mask |= POLLIN | POLLRDNORM;
    }

    if (!kfifo_is_full(&desc->fifo))
    {
        mask |= POLLOUT | POLLWRNORM;
    }

    return mask;
}

static int char_fasync(int fd, struct file *filp, int on)
{
    struct char_device *dev = filp->private_data;
    struct char_desc *desc = dev->desc;

    return fasync_helper(fd, filp, on, &desc->async_queue);
}


static struct file_operations char_fops = {
    .owner  = THIS_MODULE,
    .open   = char_open,
    .release = char_release,
    .read   = char_read,
    .write  = char_write,
    .poll   = char_poll,
    .fasync = char_fasync,
    .unlocked_ioctl = char_ioctl,
};

static int chardev_init(struct char_device *dev)
{
    int ret = 0;
#if CHAR_MAJOR
    ret = alloc_chrdev_region(&dev->devno, 0, 1, CHAR_NAME);
    if (ret)
    {
        printk("alloc_chrdev_region failed\n");
        goto fail_alloc;
    }

    cdev_init(&dev->cdev, &char_fops);
    ret = cdev_add(&dev->cdev, dev->devno, 1);
    if (ret)
    {
        printk("cdev_add failed\n");
        goto fail_cdev;
    }
#else
    ret = register_chrdev(0, CHAR_NAME, &char_fops);
    if (ret < 0)
    {
        printk("register_chrdev failed\n");
        goto fail_register;
    }
    dev->major = ret;
    dev->devno = MKDEV(dev->major, 0);
#endif

    dev->class = class_create(THIS_MODULE, CHAR_NAME);
    if (IS_ERR(dev->class))
    {
        printk("class_create failed\n");
        ret = PTR_ERR(dev->class);
        goto fail_class;
    }

    dev->device = device_create(dev->class, NULL, dev->devno, NULL,
                                CHAR_NAME "%d", MINOR(dev->devno));
    if (IS_ERR(dev->device))
    {
        printk("device_create failed\n");
        ret = PTR_ERR(dev->device);
        goto fail_device;
    }

    dev->desc = kmalloc(sizeof(struct char_desc), GFP_KERNEL);
    if (!dev->desc)
    {
        printk("kmalloc failed\n");
        ret = -ENOMEM;
        goto fail_device;
    }
    
    atomic_set(&dev->is_opened, 0);

    return 0;

fail_device:
    class_destroy(dev->class);
fail_class:
#if CHAR_MAJOR
    cdev_del(&dev->cdev);
fail_cdev:
    unregister_chrdev_region(dev->devno, 1);
fail_alloc:
#else
    unregister_chrdev(dev->major, CHAR_NAME);
fail_register:
#endif
    return ret;
}

static void chardev_exit(struct char_device *dev)
{
    device_destroy(dev->class, dev->devno);
    class_destroy(dev->class);
#if CHAR_MAJOR
    cdev_del(&dev->cdev);
    unregister_chrdev_region(dev->devno, 1);
#else
    unregister_chrdev(dev->major, CHAR_NAME);
#endif
}

static int __init mychardev_init(void)
{
    printk("char_init\n");

    return chardev_init(&g_chardev);
}

static void __exit mychardev_exit(void)
{
    printk("char_exit\n");

    chardev_exit(&g_chardev);
}

module_init(mychardev_init);
module_exit(mychardev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("RainbowSeeker");