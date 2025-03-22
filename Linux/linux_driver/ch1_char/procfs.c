#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>

#define PROCFS_NAME "myproc"
#define BUF_SIZE 1024

static char procfs_buf[BUF_SIZE] = "Hello, procfs!\n";
static struct proc_dir_entry *proc_file, *proc_dir;

static ssize_t procfs_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    return simple_read_from_buffer(buf, count, ppos, procfs_buf, strlen(procfs_buf));
}

static ssize_t procfs_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    return simple_write_to_buffer(procfs_buf, sizeof(procfs_buf), ppos, buf, count);
}

static const struct proc_ops procfs_fops = {
    .proc_read = procfs_read,
    .proc_write = procfs_write,
};

static int __init procfs_init(void)
{
    int ret = 0;
    proc_dir = proc_mkdir("mydir", NULL);
    if (!proc_dir)
    {
        ret = -ENOMEM;
        goto fail_mkdir;
    }

    proc_file = proc_create(PROCFS_NAME, 0666, proc_dir, &procfs_fops);
    if (!proc_file)
    {
        ret = -ENOMEM;
        goto fail_create;
    }

    char buf[128];
    memset(buf, 0, sizeof(buf));
    struct file *file = filp_open("/dev/my_misc_device", O_RDWR, 0);
    if (IS_ERR(file))
    {
        ret = PTR_ERR(file);
        goto fail_open;
    }
    
    ssize_t size = kernel_write(file, "Hello, kernel!\n", 15, &file->f_pos);
    if (size < 0)
    {
        ret = size;
        goto fail_write;
    }

    size = kernel_read(file, buf, sizeof(buf), &file->f_pos);
    if (size < 0)
    {
        ret = size;
        goto fail_read;
    }
    printk("read from procfs: %s\n", buf);

    filp_close(file, NULL);

    return 0;

fail_read:
fail_write:
    filp_close(file, NULL);
fail_open:
fail_create:
    proc_remove(proc_dir);
fail_mkdir:
    return ret;
}

static void __exit procfs_exit(void)
{
    proc_remove(proc_file);
    proc_remove(proc_dir);
}

module_init(procfs_init);
module_exit(procfs_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("RainbowSeeker");