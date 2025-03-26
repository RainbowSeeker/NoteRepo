#include <linux/module.h>
#include <linux/of_irq.h>
#include <linux/of_gpio.h>
#include <linux/of_platform.h>
#include <linux/interrupt.h>
#include <linux/input.h>

struct input_platform_data {
    struct input_dev *input_dev;
    int gpio;
    int irq;
    int debounce;
    int code;

    struct delayed_work work;
};

/* DTS:
mybutton0 {
    label = "mybutton_test0";
    gpios = <&gpio3 RK_PD5 GPIO_ACTIVE_LOW>;
    linux,code = <KEY_ENTER>;
    debounce-interval = <50>;
    pinctrl-names = "default";
    pinctrl-0 = <&mybutton0>;
    interrupt-parent = <&gpio3>;
    interrupts = <RK_PD5 IRQ_TYPE_EDGE_FALLING>;
};
*/

static void input_key_reporter(struct work_struct *work)
{
    struct input_platform_data *pdata = container_of(work, struct input_platform_data, work.work);

    int val = gpio_get_value(pdata->gpio);
    input_report_key(pdata->input_dev, pdata->code, val);
    input_sync(pdata->input_dev);

    pr_info("input-key: %s\n", val ? "pressed" : "released");
}

static irqreturn_t input_handler(int irq, void *dev_id)
{
    struct input_platform_data *pdata = dev_id;

    mod_delayed_work(system_wq, &pdata->work, msecs_to_jiffies(pdata->debounce));

    return IRQ_HANDLED;
}

static int input_probe(struct platform_device *pdev)
{
    struct device_node *np = pdev->dev.of_node;
    struct input_platform_data *pdata;
    int ret = 0, flags;

    pdata = devm_kzalloc(&pdev->dev, sizeof(*pdata), GFP_KERNEL);
    if (!pdata)
        return -ENOMEM;

    pdata->gpio = of_get_named_gpio(np, "gpios", 0);
    if (!gpio_is_valid(pdata->gpio)) {
        dev_err(&pdev->dev, "Failed to get gpio\n");
        return -ENODEV;
    }

    // pdata->irq = gpio_to_irq(pdata->gpio);
    // pdata->irq = irq_of_parse_and_map(np, 0);
    pdata->irq = platform_get_irq(pdev, 0);
    if (pdata->irq < 0) {
        dev_err(&pdev->dev, "Failed to get irq\n");
        return -ENODEV;
    }

    flags = irq_get_trigger_type(pdata->irq);
    if (flags < 0) {
        dev_err(&pdev->dev, "Failed to get irq flags\n");
        return -ENODEV;
    }

    ret = devm_request_irq(&pdev->dev, pdata->irq, input_handler, flags, "input-key", pdata);
    if (ret) {
        dev_err(&pdev->dev, "Failed to request irq\n");
        return ret;
    }

    // read properties
    ret = device_property_read_u32(&pdev->dev, "linux,code", &pdata->code);
    if (ret) {
        dev_err(&pdev->dev, "Failed to get keycode\n");
        return ret;
    }

    ret = device_property_read_u32(&pdev->dev, "debounce-interval", &pdata->debounce);
    if (ret) {
        pdata->debounce = 10;
    }

    INIT_DELAYED_WORK(&pdata->work, input_key_reporter);

    // register input device
    pdata->input_dev = devm_input_allocate_device(&pdev->dev);
    if (!pdata->input_dev) {
        dev_err(&pdev->dev, "Failed to allocate input device\n");
        return -ENOMEM;
    }

    device_property_read_string(&pdev->dev, "label", &pdata->input_dev->name);
    pdata->input_dev->phys = "input-key/input0";

    set_bit(EV_KEY, pdata->input_dev->evbit);
    set_bit(pdata->code, pdata->input_dev->keybit);

    platform_set_drvdata(pdev, pdata);

    ret = input_register_device(pdata->input_dev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to register input device\n");
        return ret;
    }

    dev_info(&pdev->dev, "input-key (irq=%d, gpio=%d, code=%d) probed.\n", pdata->irq, pdata->gpio, pdata->code);

    return 0;
}

static int input_remove(struct platform_device *pdev)
{
    struct input_platform_data *pdata = platform_get_drvdata(pdev);

    input_unregister_device(pdata->input_dev);

    cancel_delayed_work_sync(&pdata->work);

    dev_info(&pdev->dev, "input-key removed\n");

    return 0;
}

static struct of_device_id input_of_match[] = {
    { .compatible = "mybutton", },
    { /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, input_of_match);

static struct platform_driver input_driver = {
    .driver = {
        .name = "mybutton",
        .of_match_table = of_match_ptr(input_of_match),
    },
    .probe = input_probe,
    .remove = input_remove,
};

module_platform_driver(input_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RainbowSeeker");
MODULE_DESCRIPTION("A simple input driver");