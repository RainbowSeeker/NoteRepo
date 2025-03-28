#include <linux/module.h>
#include <linux/delay.h>
#include <linux/spi/spi.h>
#include <linux/of_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#define W25Q64_JEDEC_ID             0xEF4017

// w25q64 command
#define W25Q64_CMD_WRITE_ENABLE     0x06
#define W25Q64_CMD_WRITE_DISABLE    0x04
#define W25Q64_CMD_READ_STATUS      0x05
#define W25Q64_CMD_READ_DATA        0x03
#define W25Q64_CMD_PAGE_PROGRAM     0x02
#define W25Q64_CMD_SECTOR_ERASE     0x20
#define W25Q64_CMD_BLOCK_ERASE      0xD8
#define W25Q64_CMD_CHIP_ERASE       0xC7
#define W25Q64_CMD_READ_JEDEC_ID    0x9F


// w25q64 parameters
#define W25Q64_PAGE_SIZE            256
#define W25Q64_SECTOR_SIZE          4096
#define W25Q64_FLASH_SIZE           (8 * 1024 * 1024)

struct w25q64_platform_data {
    struct spi_device *spi;
    struct mtd_info mtd;
    struct mutex lock;
};

static int w25q64_read_reg(struct spi_device *spi, u8 code, u8 *buf, int len)
{
    return spi_write_then_read(spi, &code, 1, buf, len);
}

static int w25q64_write_cmd(struct spi_device *spi, u8 cmd) {
    return spi_write(spi, &cmd, 1);
}

static int w25q64_wait_ready(struct spi_device *spi, int timeout)
{
    unsigned long deadline;
    u8 status;
    int ret;

    deadline = jiffies + msecs_to_jiffies(timeout);

    do {
        ret = w25q64_read_reg(spi, W25Q64_CMD_READ_STATUS, &status, 1);
        if (ret)
            return ret;

        if (!(status & 0x01))
            return 0;

        cond_resched();
    } while (time_before(jiffies, deadline));

    return -ETIMEDOUT;
}

static u32 w25q64_read_id(struct spi_device *spi)
{
    u8 buf[3];
    w25q64_read_reg(spi, W25Q64_CMD_READ_JEDEC_ID, buf, 3);

    return (buf[0] << 16) | (buf[1] << 8) | buf[2];
}

static int w25q64_read_data(struct spi_device *spi, u32 addr, u8 *buf, u32 len)
{
    u8 cmd[4] = { W25Q64_CMD_READ_DATA, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF };
    return spi_write_then_read(spi, cmd, sizeof(cmd), buf, len);
}

static int w25q64_write_page(struct spi_device *spi, u32 addr, const u8 *buf, u32 len)
{
    struct w25q64_platform_data *pdata = spi_get_drvdata(spi);
    size_t page_offset, chunk;
    int ret, retlen;
    u8 cmd[4];
    struct spi_transfer t[2] = { 0 };

    mutex_lock(&pdata->lock);

    retlen = 0;
    while (len) {
        page_offset = addr % W25Q64_PAGE_SIZE;
        chunk = min_t(size_t, W25Q64_PAGE_SIZE - page_offset, len);

        cmd[0] = W25Q64_CMD_PAGE_PROGRAM;
        cmd[1] = (addr >> 16) & 0xFF;
        cmd[2] = (addr >> 8) & 0xFF;
        cmd[3] = addr & 0xFF;

        ret = w25q64_write_cmd(spi, W25Q64_CMD_WRITE_ENABLE);
        if (ret)
            goto out;
        
        t[0].tx_buf = cmd;
        t[0].len = sizeof(cmd);
        t[1].tx_buf = buf;
        t[1].len = chunk;
        ret = spi_sync_transfer(spi, t, 2);
        if (ret)
            goto out;
        
        ret = w25q64_wait_ready(spi, 10);
        if (ret)
            goto out;
        
        addr += chunk;
        buf += chunk;
        len -= chunk;
        retlen += chunk;
    }

out:
    mutex_unlock(&pdata->lock);
    return ret < 0 ? ret : retlen;
}

static int w25q64_erase_sector(struct spi_device *spi, u32 addr)
{
    struct w25q64_platform_data *pdata = spi_get_drvdata(spi);
    u8 cmd[4];
    int ret;

    cmd[0] = W25Q64_CMD_SECTOR_ERASE;
    cmd[1] = (addr >> 16) & 0xFF;
    cmd[2] = (addr >> 8) & 0xFF;
    cmd[3] = addr & 0xFF;

    mutex_lock(&pdata->lock);

    ret = w25q64_write_cmd(spi, W25Q64_CMD_WRITE_ENABLE);
    if (ret)
        goto out;

    ret = spi_write(spi, cmd, sizeof(cmd));
    if (ret)
        goto out;
    
    ret = w25q64_wait_ready(spi, 400);
    if (ret)
        goto out;

out:
    mutex_unlock(&pdata->lock);
    return ret;
}


/* ---------- MTD ----------*/
static int w25q64_mtd_read(struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen, u_char *buf)
{
    struct w25q64_platform_data *pdata = container_of(mtd, struct w25q64_platform_data, mtd);
    struct spi_device *spi = pdata->spi;
    int ret;

    ret = w25q64_read_data(spi, from, buf, len);
    if (ret)
        return ret;

    *retlen = len;
    return 0;
}

static int w25q64_mtd_write(struct mtd_info *mtd, loff_t to, size_t len, size_t *retlen, const u_char *buf)
{
    struct w25q64_platform_data *pdata = container_of(mtd, struct w25q64_platform_data, mtd);
    struct spi_device *spi = pdata->spi;
    int ret;

    if (to + len > mtd->size) {
        dev_err(&spi->dev, "write out of range\n");
        return -EINVAL;
    }

    ret = w25q64_write_page(spi, to, buf, len);
    if (ret < 0)
        return ret;
    
    *retlen = ret;
    return 0;
}

static int w25q64_mtd_erase(struct mtd_info *mtd, struct erase_info *instr)
{
    struct w25q64_platform_data *pdata = container_of(mtd, struct w25q64_platform_data, mtd);
    struct spi_device *spi = pdata->spi;
    u32 addr, end;
    int ret;

    addr = instr->addr;
    end = addr + instr->len;

    if (addr % mtd->erasesize || end % mtd->erasesize)
        return -EINVAL;
    
    
    while (addr < end) {
        ret = w25q64_erase_sector(spi, addr);
        if (ret)
            return ret;
        
        addr += mtd->erasesize;
    }

    return 0;
}

static int spi_probe(struct spi_device *spi)
{
    struct w25q64_platform_data *pdata;
    u32 devid;
    int ret;

    pdata = devm_kzalloc(&spi->dev, sizeof(*pdata), GFP_KERNEL);
    if (!pdata)
        return -ENOMEM;

    mutex_init(&pdata->lock);
    pdata->spi = spi;
    spi_set_drvdata(spi, pdata);

    // validate the spi device
    devid = w25q64_read_id(spi);
    if (devid != W25Q64_JEDEC_ID) {
        dev_err(&spi->dev, "invalid jedec id: %06x\n", devid);
        return -ENODEV;
    }

    dev_info(&spi->dev, "jedec id: %06x\n", devid);

    // initialize mtd
    pdata->mtd.name = "w25q64";
    pdata->mtd.type = MTD_NORFLASH;
    pdata->mtd.flags = MTD_CAP_NORFLASH;
    pdata->mtd.size = W25Q64_FLASH_SIZE;
    pdata->mtd.writesize = W25Q64_PAGE_SIZE;
    pdata->mtd.writebufsize = W25Q64_PAGE_SIZE;
    pdata->mtd.erasesize = W25Q64_SECTOR_SIZE;
    pdata->mtd._read = w25q64_mtd_read;
    pdata->mtd._write = w25q64_mtd_write;
    pdata->mtd._erase = w25q64_mtd_erase;
    pdata->mtd.owner = THIS_MODULE;

    ret = mtd_device_register(&pdata->mtd, NULL, 0);
    if (ret < 0) {
        dev_err(&spi->dev, "failed to register mtd device\n");
        return ret;
    }

    return 0;
}

static int spi_remove(struct spi_device *spi)
{
    struct w25q64_platform_data *pdata = spi_get_drvdata(spi);

    mtd_device_unregister(&pdata->mtd);
    
    return 0;
}

static const struct of_device_id spi_of_match[] = {
    { .compatible = "winbond,w25q64-spi", },
    { },
};

MODULE_DEVICE_TABLE(of, spi_of_match);

static struct spi_driver spi_driver = {
    .probe = spi_probe,
    .remove = spi_remove,
    .driver = {
        .name = "winbond,w25q64-spi",
        .of_match_table = of_match_ptr(spi_of_match),
    },
};

module_spi_driver(spi_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RainbowSeeker");