#include <linux/hrtimer.h>
#include <linux/jiffies.h>
#include <linux/module.h>

#define TIMER_TYPE      1   // 0: jiffies 1: hrtimer

static int interval_ms = 100;

#if TIMER_TYPE == 0
static struct timer_list g_timer;
static void timer_func(struct timer_list *timer)
{
    printk("timer_func\n");

    mod_timer(timer, jiffies + msecs_to_jiffies(interval_ms));
}
#else
static struct hrtimer g_timer;
static enum hrtimer_restart timer_func(struct hrtimer *timer)
{
    printk("timer_func\n");

    hrtimer_forward_now(timer, ms_to_ktime(interval_ms));
    return HRTIMER_RESTART;
}
#endif

static int __init timer_init(void)
{
    printk("timer init\n");

    // 1. init timer
#if TIMER_TYPE == 0
    timer_setup(&g_timer, timer_func, 0);
#else
    hrtimer_init(&g_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    g_timer.function = timer_func;
#endif

    // 2. start timer
#if TIMER_TYPE == 0
    mod_timer(&g_timer, jiffies + msecs_to_jiffies(interval_ms));
#else
    hrtimer_start(&g_timer, ms_to_ktime(interval_ms), HRTIMER_MODE_REL);
#endif

    return 0;
}

static void __exit timer_exit(void)
{
    printk("timer exit\n");

#if TIMER_TYPE == 0
    del_timer(&g_timer);
#else
    hrtimer_cancel(&g_timer);
#endif
}

module_init(timer_init);
module_exit(timer_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("RainbowSeeker");