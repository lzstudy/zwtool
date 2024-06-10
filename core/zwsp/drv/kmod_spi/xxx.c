#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/of_platform.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/spi/spi.h>
#include "ckerr.h"

typedef struct _xxx_priv {
	int pin;																	/**@ 引脚. */
	int irq;																	/**@ 中断号. */
	struct miscdevice misc;														/**@ misc设备. */
}xxx_priv;

/* IOC define */
#define	IOC_TEST				_IOR(0x21, 0, int)								/**< 等待事件. */
#define IRQ_TRIGGER_BOTH        (IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING)	/**< 双边沿. */	
#define to_xxx_priv(x)			container_of(x, xxx_priv, misc)					/**< 获取设备信息. */

/**************************************************************************************************
 * @brief  : 接口 - open
 * @param  : inode
 * @param  : 文件信息
 * @return : 0成功, 其他失败
**************************************************************************************************/
static int xxx_open(struct inode *inode, struct file *filp)
{
	xxx_priv *priv = spi_get_drvdata(filp->private_data);

	LOG_I("priv addr = %p", priv);
	LOG_I("xxx open");
	return 0;
}

/**************************************************************************************************
 * @brief  : 接口 - release
 * @param  : inode
 * @param  : 文件信息
 * @return : 0成功, 其他失败
**************************************************************************************************/
static int xxx_release(struct inode *inode, struct file *filp)
{
	LOG_I("xxx release\n");
	return 0;
}

/**************************************************************************************************
 * @brief  : 接口 - ioctl
 * @param  : 文件信息
 * @param  : 命令码
 * @param  : 参数
 * @return : 0成功, -1失败
**************************************************************************************************/
static long xxx_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	switch(cmd)
	{
	case IOC_TEST:
		break;

	default:
		return -ENOIOCTLCMD;
	}

	return 0;
}

static const struct file_operations xxx_fops = {
	.owner	 = THIS_MODULE,
	.open	 = xxx_open,
	.release = xxx_release,
	.unlocked_ioctl = xxx_ioctl,
};

/**************************************************************************************************
 * @brief  : 属性 - w - dgb1
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static ssize_t xxx_dbg1_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int val;
	xxx_priv *priv = to_xxx_priv(dev->driver_data);

    if(kstrtoint(buf, 0, &val))
        return -EINVAL;
	
	LOG_I("priv addr = %p, val = %d", priv, val);
	return count;
}

/**************************************************************************************************
 * @brief  : 属性 - r - dgb2
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static ssize_t xxx_dbg2_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	int val = 0;
	xxx_priv *priv = to_xxx_priv(dev->driver_data);

	LOG_I("priv addr = %p, val = %d", priv, val);
	return sprintf(buf, "%d\n", val);
}

/**************************************************************************************************
 * @brief  : 属性 - r - dgb3
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static ssize_t xxx_dbg3_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	int val = 0;
	xxx_priv *priv = to_xxx_priv(dev->driver_data);

	LOG_I("priv addr = %p, val = %d", priv, val);
	return sprintf(buf, "%d\n", val);
}

/**************************************************************************************************
 * @brief  : 属性 - w - dgb3
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static ssize_t xxx_dbg3_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int val;
	xxx_priv *priv = to_xxx_priv(dev->driver_data);

    if(kstrtoint(buf, 0, &val))
        return -EINVAL;
	
	LOG_I("priv addr = %p, val = %d", priv, val);
	return count;
}

static DEVICE_ATTR(dbg1, 0220, NULL, xxx_dbg1_store);
static DEVICE_ATTR(dbg2, 0440, xxx_dbg2_show, NULL);
static DEVICE_ATTR(dbg3, 0640, xxx_dbg3_show, xxx_dbg3_store);

static struct attribute *xxx_attr[] = {
	&dev_attr_dbg1.attr,
	&dev_attr_dbg2.attr,
	&dev_attr_dbg3.attr,
	NULL
};

static struct attribute_group xxx_attr_group = {
	.name = "attr",
	.attrs = xxx_attr,
};

/**************************************************************************************************
 * @brief  : 解析设备树
 * @param  : 私有数据
 * @param  : 设备节点
 * @return : 0成功, -1失败
**************************************************************************************************/
static int parse_dtb(xxx_priv *priv, struct device *dev, struct device_node *np)
{
#if 0
	/* 获取引脚 */
	priv->pin = of_get_named_gpio(np, "gpios", 0);
	DS_RET(priv->pin, of_get_named_gpio);
#endif
	return 0;
}

/**************************************************************************************************
 * @brief  : 设置硬件属性
 * @param  : 设备
 * @return : 0成功, -1失败
**************************************************************************************************/
static int set_hardware(xxx_priv *priv, struct device *dev)
{
#if 0
	int ret;

	/* 申请GPIO */
	ret = devm_gpio_request(dev, priv->pin, "xxx-pin");
	DS_RET(ret, devm_gpio_request);

	/* 设置输出 */
	ret = gpio_direction_input(priv->pin);
	DS_RET(ret, gpio_direction_input);
#endif 
	return 0;
}

/**************************************************************************************************
 * @brief  : 驱动匹配
 * @param  : 平台设备
 * @return : 0成功, -1失败
**************************************************************************************************/
static int xxx_probe(struct spi_device *spi)
{
	int ret;
	xxx_priv *priv;
	struct device *dev = &spi->dev;

	/* 申请内存空间 */
	priv = devm_kzalloc(dev, sizeof(xxx_priv), GFP_KERNEL);
	DP_RET(priv, devm_kzalloc);

	/* 初始化MISC设备 */
	priv->misc.name  = "xxx";
	priv->misc.minor = MISC_DYNAMIC_MINOR;
	priv->misc.fops  = &xxx_fops;

	/* 解析设备树 */
	ret = parse_dtb(priv, dev, dev->of_node);
	CK_RET(ret < 0, ret);

	/* 设置硬件 */
	ret = set_hardware(priv, dev);
	CK_RET(ret < 0, ret);
	
	/* 注册MISC设备 */
	ret = misc_register(&priv->misc);
	DS_RET(ret, misc_register);

	/* 创建属性文件 */
	ret = sysfs_create_group(&dev->kobj, &xxx_attr_group);
	DS_RET(ret, sysfs_create_group);

	/* 保存私有数据 */
	spi_set_drvdata(spi, priv);
	LOG_I("xxx init");
	return 0;
}

/**************************************************************************************************
 * @brief  : 驱动删除
 * @param  : 设备
 * @return : 0成功, -1失败
**************************************************************************************************/
static int xxx_remove(struct spi_device *spi)
{
	xxx_priv *priv = spi_get_drvdata(spi);

	/* 删除属性, 并注销设备 */
	sysfs_remove_group(&spi->dev.kobj, &xxx_attr_group);
	misc_deregister(&priv->misc);
	LOG_I("xxx remove");
	return 0;
}

static const struct of_device_id xxx_match[] = {
	{ .compatible = "xxx-spi" },
	{},
};

static struct spi_driver xxx_driver = {
    .probe  = xxx_probe,
    .remove = xxx_remove,
	.driver	= {
		.owner = THIS_MODULE,
		.name = "xxx-spi",
		.of_match_table = xxx_match,
	},
};

module_spi_driver(xxx_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZW");
MODULE_DESCRIPTION("dirver - xxx");
