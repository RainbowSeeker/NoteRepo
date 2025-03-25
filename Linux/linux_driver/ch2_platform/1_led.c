#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>


static int led_probe(struct platform_device *pdev)
{
    printk("led_probe\n");
    return 0;
}

static int led_remove(struct platform_device *pdev)
{
    printk("led_remove\n");
    return 0;
}

static struct of_device_id led_of_match[] = {
    { .compatible = "myled", },
    { /* sentinel */ }
};

static struct platform_driver led_driver = {
    .driver = {
        .owner = THIS_MODULE,
        .name = "myled",
        .of_match_table = led_of_match,
    },
    .probe = led_probe,
    .remove = led_remove,
};

module_platform_driver(led_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RainbowSeeker");
MODULE_DESCRIPTION("A simple led platform driver");