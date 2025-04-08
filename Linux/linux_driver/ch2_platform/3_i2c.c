#include <linux/module.h>
#include <linux/of_gpio.h>
#include <linux/of_platform.h>
#include <linux/i2c.h>
#include <linux/fb.h>
#include <linux/dma-mapping.h>

#define USE_DMA

// OLED parameters
#define OLED_ADDR    0x3C
#define OLED_WIDTH   128
#define OLED_HEIGHT  64

// OLED commands
#define OLED_DATA        0x40
#define OLED_CMD         0x00
#define OLED_CONTRAST    0x81
#define OLED_DISPLAY_ON  0xAF
#define OLED_DISPLAY_OFF 0xAE

static const struct fb_fix_screeninfo oled_fix = {
    .id = "Solomon SSD1306",
    .type = FB_TYPE_PACKED_PIXELS,
    .visual = FB_VISUAL_MONO10,
    .xpanstep = 0,
    .ypanstep = 0,
    .ywrapstep = 0,
    .line_length = OLED_WIDTH / 8,
    .accel = FB_ACCEL_NONE,
};

static const struct fb_var_screeninfo oled_var = {
    .bits_per_pixel = 1,
    .grayscale = 1,
    .red = { .length = 1 },
    .green = { .length = 1 },
    .blue = { .length = 1 },
};

struct oled_fb_array {
    u8 type;
    u8 data[];
};

struct oled_device {
    struct i2c_client *client;
    struct fb_info *fb_info;
    struct oled_fb_array *fb_array;

    u32 width;
    u32 height;
    u32 fb_size;
    u32 page_offset;
    u32 com_offset;
};

static int oled_write_cmd(struct oled_device *dev, u8 cmd)
{
    u8 buf[2] = {OLED_CMD, cmd};
    return i2c_master_send(dev->client, buf, 2) == 2 ? 0 : -EIO;
}

static int oled_bsp_init(struct oled_device *dev)
{
    const u8 init_seq[] = {
        OLED_CMD, OLED_DISPLAY_OFF,       // Display Off
        0xD5, 0x80,          // Set Osc Frequency
        0xA8, 0x3F,          // Set MUX Ratio
        0xD3, dev->page_offset, // Set Display Offset
        0x40,                // Set Display Start Line
        0x8D, 0x14,          // Enable Charge Pump
        0x20, 0x00,          // Memory Mode
        0x21, 0x00, dev->width - 1, // Set Column Address
        0x22, 0x00, DIV_ROUND_UP(dev->height, 8) - 1, // Set Page Address
        0xA1,                // Segment Remap
        0xC8,                // COM Output Scan Direction
        0xDA, 0x12,          // Set COM Pins
        0x81, 0xCF,          // Set Contrast
        0xD9, 0xF1,          // Set Precharge Period
        0xDB, 0x40,          // Set VCOMH Deselect Level
        0xA4,                // Entire Display On
        0xA6,                // Normal Display
        OLED_DISPLAY_ON,     // Display On
    };

    return i2c_master_send(dev->client, init_seq, sizeof(init_seq)) == sizeof(init_seq) ? 0 : -EIO;
}

/*------ Frame Buffer ---------*/
static int oled_update_display(struct oled_device *dev)
{
    u8 *vmem = dev->fb_info->screen_base;
    unsigned int pages = DIV_ROUND_UP(dev->height, 8);
    unsigned int line_len = dev->fb_info->fix.line_length;
    unsigned int i, j, k, len = dev->fb_size + sizeof(struct oled_fb_array);
    struct oled_fb_array *array = dev->fb_array;
    int ret = 0;

    for (i = 0; i < pages; i++) {
        for (j = 0; j < dev->width; j++) {
            int m = 8, idx = i * dev->width + j;
            array->data[idx] = 0;
            /* Last page may be partial */
			if (i + 1 == pages && dev->height % 8)
                m = dev->height % 8;
            for (k = 0; k < m; k++) {
                u8 byte = vmem[(i * 8 + k) * line_len + j / 8];
                u8 bit = (byte >> (j % 8)) & 1;
                array->data[idx] |= bit << k;
            }
        }
    }
    
    return i2c_master_send(dev->client, (u8 *)array, len) == len ? 0 : -EIO;
}

static ssize_t oled_fb_write(struct fb_info *info, const char __user *buf, size_t count, loff_t *ppos)
{
    struct oled_device *dev = info->par;
    u8 *vmem = dev->fb_info->screen_base;
    unsigned long len = info->fix.smem_len;
    int ret;

    if (*ppos >= len)
        return 0;

    if (*ppos + count > len)
        count = len - *ppos;

    if (copy_from_user(vmem + *ppos, buf, count))
        return -EFAULT;

    ret = oled_update_display(dev);

    if (ret < 0)
        return ret;

    *ppos += count;
    return count;
}

static int oled_fb_blank(int blank_mode, struct fb_info *info)
{
    struct oled_device *dev = info->par;

    if (blank_mode == FB_BLANK_UNBLANK)
        return oled_write_cmd(dev, OLED_DISPLAY_ON);
    else
        return oled_write_cmd(dev, OLED_DISPLAY_OFF);
}

static void oled_fb_fillrect(struct fb_info *info, const struct fb_fillrect *rect)
{
    struct oled_device *dev = info->par;
    cfb_fillrect(info, rect);
    schedule_delayed_work(&info->deferred_work, 0);
}

static void oled_fb_copyarea(struct fb_info *info, const struct fb_copyarea *area)
{
    struct oled_device *dev = info->par;
    cfb_copyarea(info, area);
    schedule_delayed_work(&info->deferred_work, 0);
}

static void oled_fb_imageblit(struct fb_info *info, const struct fb_image *image)
{
    struct oled_device *dev = info->par;
    cfb_imageblit(info, image);
    schedule_delayed_work(&info->deferred_work, 0);
}

static void oled_fb_deferred_io(struct fb_info *info, struct list_head *pagelist)
{
    oled_update_display(info->par);
}

static struct fb_deferred_io oled_fbdefio = {
    .delay = HZ / 2,
    .deferred_io = oled_fb_deferred_io,
};

static struct fb_ops oled_fb_ops = {
    .owner = THIS_MODULE,
    .fb_read = fb_sys_read,
    .fb_write = oled_fb_write,
    .fb_blank = oled_fb_blank,
    .fb_fillrect = oled_fb_fillrect,
    .fb_copyarea = oled_fb_copyarea,
    .fb_imageblit = oled_fb_imageblit,
};

static int oled_fb_init(struct oled_device *dev)
{
    struct fb_info *info;
    int ret;

    info = framebuffer_alloc(0, NULL);
    if (!info)
        return -ENOMEM;

    info->fbdefio = &oled_fbdefio;

    info->fbops = &oled_fb_ops;
    info->fix = oled_fix;
    info->fix.line_length = dev->width / 8;
    info->var = oled_var;
    info->var.xres = dev->width,
    info->var.yres = dev->height,
    info->var.xres_virtual = dev->width,
    info->var.yres_virtual = dev->height,
    
    info->fix.smem_len = info->fix.line_length * dev->height;
#ifdef USE_DMA
    ret = dma_set_coherent_mask(&dev->client->dev, DMA_BIT_MASK(32));
    if (ret) {
        dev_err(&dev->client->dev, "failed to set coherent mask: %d\n", ret);
        goto fail_alloc;
    }

    info->screen_base = dma_alloc_coherent(&dev->client->dev, info->fix.smem_len, (dma_addr_t *)&info->fix.smem_start, GFP_KERNEL);
    if (!info->screen_base) {
        dev_err(&dev->client->dev, "failed to allocate framebuffer\n");
        ret = -ENOMEM;
        goto fail_alloc;
    }
#else
    info->screen_base = devm_kzalloc(&dev->client->dev, info->fix.smem_len, GFP_KERNEL);
    if (!info->screen_base) {
        ret = -ENOMEM;
        goto fail_alloc;
    }
    info->fix.smem_start = virt_to_phys(info->screen_base);
#endif

    fb_deferred_io_init(info);

    ret = register_framebuffer(info);
    if (ret) {
        dev_err(&dev->client->dev, "fb%d: failed to register framebuffer: %d\n", info->node, ret);
        goto fail_register;
    }

    info->par = dev;
    dev->fb_info = info;

    dev_info(&dev->client->dev, "fb%d: %s frame buffer device registered, using %d bytes\n",
                    info->node, info->fix.id, info->fix.smem_len);
    return 0;

fail_register:
    fb_deferred_io_cleanup(info);
#ifdef USE_DMA
    dma_free_coherent(&dev->client->dev, info->fix.smem_len, info->screen_base, info->fix.smem_start);
#endif
fail_alloc:
    framebuffer_release(info);
    return ret;
}

static void oled_fb_exit(struct oled_device *dev)
{
    cancel_delayed_work_sync(&dev->fb_info->deferred_work);
    
    unregister_framebuffer(dev->fb_info);
    
    fb_deferred_io_cleanup(dev->fb_info);

#ifdef USE_DMA
    dma_free_coherent(&dev->client->dev, dev->fb_info->fix.smem_len, dev->fb_info->screen_base, dev->fb_info->fix.smem_start);
#endif

    framebuffer_release(dev->fb_info);
}

static int i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    struct device_node *np = client->dev.of_node;
    struct oled_device *dev;
    int ret = 0;

    dev = devm_kzalloc(&client->dev, sizeof(*dev), GFP_KERNEL);
    if (!dev)
        return -ENOMEM;
    
    if (device_property_read_u32(&client->dev, "solomon,width", &dev->width))
        dev->width = OLED_WIDTH;
    
    if (device_property_read_u32(&client->dev, "solomon,height", &dev->height))
        dev->height = OLED_HEIGHT;

    if (device_property_read_u32(&client->dev, "solomon,page-offset", &dev->page_offset))
        dev->page_offset = 0;
    
    if (device_property_read_u32(&client->dev, "solomon,com-offset", &dev->com_offset))
        dev->com_offset = 0;
    
    dev->fb_size = dev->width * DIV_ROUND_UP(dev->height, 8);
    dev->fb_array = devm_kzalloc(&client->dev, sizeof(struct oled_fb_array) + dev->fb_size, GFP_KERNEL);
    if (!dev->fb_array)
        return -ENOMEM;

    dev->fb_array->type = OLED_DATA;
    dev->client = client;
    i2c_set_clientdata(client, dev);

    ret = oled_bsp_init(dev);
    if (ret){
        pr_err("OLED bsp init failed:%d\n", ret);
        return ret;
    }
    
    return oled_fb_init(dev);
}

static int i2c_remove(struct i2c_client *client)
{
    struct oled_device *dev = i2c_get_clientdata(client);

    oled_fb_exit(dev);
    return 0;
}


static const struct of_device_id i2c_of_match[] = {
    { .compatible = "oled,ssd1306-i2c", },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, i2c_of_match);

static struct i2c_driver i2c_driver = {
    .probe = i2c_probe,
    .remove = i2c_remove,
    .driver = {
        .name = "oled,ssd1306-i2c",
        .of_match_table = of_match_ptr(i2c_of_match),
    },
};

module_i2c_driver(i2c_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RainbowSeeker");
MODULE_DESCRIPTION("I2C OLED driver");