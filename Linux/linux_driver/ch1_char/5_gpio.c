#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/of.h>
#include <linux/of_gpio.h>

#include <dt-bindings/pinctrl/rockchip.h>

#define CHIP_BASE       (96)   // RK3288s gpio3
#define GPIO_ID         (CHIP_BASE + RK_PC0)

#define GPIO_DIRECTION  0   // 0: output, 1: input
/*
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
*/
#if GPIO_DIRECTION == 1
static irqreturn_t my_gpio_irq(int irq, void *dev_id)
{
    int val = gpio_get_value(GPIO_ID);
    printk("IRQ triggered, value: %d\n", val);

    return IRQ_HANDLED;
}
#endif


static int __init gpio_init(void)
{
    int ret = gpio_request(GPIO_ID, "my_led");
    if (ret)
    {
        pr_err("gpio_request failed\n");
        goto fail_find_gpio;
    }

#if GPIO_DIRECTION == 0
    ret = gpio_direction_output(GPIO_ID, 1);
    if (ret)
    {
        pr_err("gpio_direction_output failed\n");
        goto fail_direction;
    }

    gpio_set_value(GPIO_ID, 1);
#else
    ret = gpio_direction_input(GPIO_ID);
    if (ret)
    {
        pr_err("gpio_direction_input failed\n");
        goto fail_direction;
    }

    int irq = gpio_to_irq(GPIO_ID);
    if (irq < 0)
    {
        pr_err("gpio_to_irq failed\n");
        goto fail_direction;
    }

    ret = request_irq(irq, my_gpio_irq, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "my_irq", NULL);
    if (ret)
    {
        pr_err("request_irq failed\n");
        goto fail_direction;
    }

#endif

    printk("gpio init success\n");

    return 0;

fail_direction:
    gpio_free(GPIO_ID);
fail_find_gpio:
    return ret;
}

static void __exit gpio_exit(void)
{
#if GPIO_DIRECTION == 0
    gpio_set_value(GPIO_ID, 0);
#else
    free_irq(gpio_to_irq(GPIO_ID), NULL);
#endif

    gpio_free(GPIO_ID);
}

module_init(gpio_init);
module_exit(gpio_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("RainbowSeeker");