#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/of.h>
#include <linux/of_gpio.h>

#include <../common_drivers/include/dt-bindings/gpio/meson-g12a-gpio.h>

#define CHIPA_BASE      (410)
#define GPIO_ID         (CHIPA_BASE + GPIOH_4)
/*
gpioleds {
		compatible = "gpio-leds";
		status = "okay";

		sys_led {
			label="sys_led";
			gpios=<&gpio_ao GPIOAO_4 GPIO_ACTIVE_HIGH>;
			default-state ="on";
			retain-state-suspended;
			linux,default-trigger="default-on";
		};

		red_led {
			label = "red_led";
			gpios = <&gpio_expander 5 GPIO_ACTIVE_HIGH>;
			default-state ="off";
			linux,default-trigger="none";
		};
	};
*/
static irqreturn_t my_gpio_irq(int irq, void *dev_id)
{
    int val = gpio_get_value(GPIO_ID);
    printk("IRQ triggered, value: %d\n", val);

    return IRQ_HANDLED;
}


static int __init gpio_init(void)
{
    int ret = gpio_request(GPIO_ID, "my_led");
    if (ret)
    {
        pr_err("gpio_request failed\n");
        goto fail_find_gpio;
    }

#if 0
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

    ret = request_irq(irq, my_gpio_irq, IRQF_TRIGGER_FALLING, "my_irq", NULL);
    if (ret)
    {
        pr_err("request_irq failed\n");
        goto fail_direction;
    }

#endif

    return 0;

fail_direction:
    gpio_free(GPIO_ID);
fail_find_gpio:
    return ret;
}

static void __exit gpio_exit(void)
{
    free_irq(gpio_to_irq(GPIO_ID), NULL);

    gpio_free(GPIO_ID);
}

module_init(gpio_init);
module_exit(gpio_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("RainbowSeeker");