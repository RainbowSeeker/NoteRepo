#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/workqueue.h>

#define TASKLET_WQ_TYPE     0   // 0: Tasklet, 1: Workqueue

#if TASKLET_WQ_TYPE == 0
static struct tasklet_struct my_tasklet;
void tasklet_handler(unsigned long data)
{
    printk("Tasklet running on CPU %d, data=%lu\n", smp_processor_id(), data);
}
#else
static struct work_struct my_work;
void work_handler(struct work_struct *work)
{
    printk("Workqueue running on CPU %d\n", smp_processor_id());
}
#endif

static int __init tasklet_wq_init(void)
{
#if TASKLET_WQ_TYPE == 0
    tasklet_init(&my_tasklet, tasklet_handler, 1234);

    tasklet_schedule(&my_tasklet);
#else
    INIT_WORK(&my_work, work_handler);

    schedule_work(&my_work);
#endif
    return 0;
}

static void __exit tasklet_wq_exit(void)
{
#if TASKLET_WQ_TYPE == 0
    tasklet_kill(&my_tasklet);
#else
    cancel_work_sync(&my_work);
#endif
}

module_init(tasklet_wq_init);
module_exit(tasklet_wq_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("RainbowSeeker");