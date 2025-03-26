#include <linux/module.h>
#include <linux/of_gpio.h>
#include <linux/of_platform.h>
#include <linux/leds.h>

struct myled_platform_data {
    struct led_classdev cdev;
    struct fwnode_handle *fwnode;
    int gpio;
};

/* DTS:
myled {
    compatible = "myled";
    label = "myled0";
    default-state = "on";
    gpios = <&gpio3 RK_PC0 GPIO_ACTIVE_LOW>;
    linux,default-trigger = "heartbeat";
};
*/

static void led_brightness_set(struct led_classdev *led_cdev, enum led_brightness brightness)
{
    struct myled_platform_data *pdata = container_of(led_cdev, struct myled_platform_data, cdev);

    if (brightness == LED_FULL)
        gpio_set_value(pdata->gpio, 1);
    else
        gpio_set_value(pdata->gpio, 0);
}

static enum led_brightness led_brightness_get(struct led_classdev *led_cdev)
{
    struct myled_platform_data *pdata = container_of(led_cdev, struct myled_platform_data, cdev);

    if (gpio_get_value(pdata->gpio))
        return LED_FULL;
    else
        return LED_OFF;
}

static int led_probe(struct platform_device *pdev)
{
    struct device_node *np = pdev->dev.of_node;
    struct myled_platform_data *pdata;
    struct led_init_data init_data = {};
    const char *state;
    int ret = 0;

    pdata = devm_kzalloc(&pdev->dev, sizeof(struct myled_platform_data), GFP_KERNEL);
    if (!pdata) {
        dev_err(&pdev->dev, "no memory\n");
        return -ENOMEM;
    }

    pdata->gpio = of_get_named_gpio(np, "gpios", 0);
    if (pdata->gpio < 0) {
        dev_err(&pdev->dev, "no gpios property\n");
        return -ENODEV;
    }

    device_property_read_string(&pdev->dev, "label", &pdata->cdev.name);

    ret = devm_gpio_request(&pdev->dev, pdata->gpio, pdata->cdev.name);
    if (ret) {
        dev_err(&pdev->dev, "request gpio failed\n");
        return ret;
    }

    gpio_direction_output(pdata->gpio, 1);

    ret = of_property_read_string(np, "default-state", &state);
    if (!ret) {
        if (strcmp(state, "on") == 0) {
            pdata->cdev.brightness = LED_FULL;
        } else {
            pdata->cdev.brightness = LED_OFF;
        }
    } else {
        pdata->cdev.brightness = LED_OFF;
    }

    if (pdata->cdev.brightness == LED_FULL)
        gpio_set_value(pdata->gpio, 1);
    else
        gpio_set_value(pdata->gpio, 0);

    pdata->cdev.brightness_set = led_brightness_set;
    pdata->cdev.brightness_get = led_brightness_get;

    init_data.fwnode = of_fwnode_handle(np);
    init_data.devicename = "myled";
    init_data.default_label = ":";

    ret = devm_led_classdev_register_ext(&pdev->dev, &pdata->cdev, &init_data);
    if (ret) {
        dev_err(&pdev->dev, "Failed to register led\n");
        return ret;
    }

    platform_set_drvdata(pdev, pdata);

    return 0;
}

static int led_remove(struct platform_device *pdev)
{
    struct myled_platform_data *pdata = platform_get_drvdata(pdev);

    gpio_set_value(pdata->gpio, 0);

    return 0;
}

static struct of_device_id led_of_match[] = {
    { .compatible = "myled", },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, led_of_match);

static struct platform_driver led_driver = {
    .driver = {
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