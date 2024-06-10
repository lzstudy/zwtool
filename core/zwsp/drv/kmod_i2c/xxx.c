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
#include <linux/timer.h>
#include <linux/i2c.h>
#include "ckerr.h"

typedef struct _xxx_priv {
	int pin;																	/**@ 引脚. */
	int irq;																	/**@ 中断号. */
	struct miscdevice dev;														/**@ misc设备. */
}xxx_priv;

/* IOC define */
#define	IOC_TEST				_IOR(0x21, 0, int)								/**< 等待事件. */

/* 上下文宏 */
#define to_xxx_priv(x)			container_of(x, xxx_priv, dev)					/**< 获取设备信息. */

/**************************************************************************************************
 * @brief  : 接口 - open
 * @param  : inode
 * @param  : 文件信息
 * @return : 0成功, 其他失败
**************************************************************************************************/
static int xxx_open(struct inode *inode, struct file *filp)
{
#if 0
	/* flip->private_data 已经存放了misc */
	xxx_priv *priv = to_xxx_priv(filp->private_data);
	LOG_I("priv addr = %p", priv);
#endif
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
 * @brief  : i2c读取
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static int xxx_i2c_read(struct i2c_client *client, uint8_t reg, void *val, int len)
{
    struct i2c_msg msg[2];

    /* 设置要读的寄存器 */
    msg[0].addr  = client->addr;
    msg[0].flags = 0;
    msg[0].len   = 1;
    msg[0].buf   = &reg;

    /* 设置数据保存信息 */
    msg[1].addr  = client->addr;
    msg[1].flags = I2C_M_RD;
    msg[1].len   = len;
    msg[1].buf   = val;

	return i2c_transfer(client->adapter, msg, 2);
}

/**************************************************************************************************
 * @brief  : i2c编写
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static int xxx_i2c_write(struct i2c_client *client, uint8_t reg, void *val, int len)
{
	uint8_t buf[2];
	struct i2c_msg msg;

	buf[0] = reg;
	memcpy(&buf[1], buf, len);

	msg.addr  = client->addr;
	msg.flags = 0;
	msg.buf   = buf;
	msg.len   = len + 1;

	return i2c_transfer(client->adapter, &msg, 1);
}

/**************************************************************************************************
 * @brief  : 属性 - w - dgb1
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static ssize_t xxx_dbg1_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int val;
	
    if(kstrtoint(buf, 0, &val))
        return -EINVAL;
	
	LOG_I(" val = %d", val);
	return count;
}

/**************************************************************************************************
 * @brief  : 属性 - r - dgb2
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static ssize_t xxx_dbg2_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	uint8_t cmd[6];
	uint8_t val[5];
	struct i2c_client *client = to_i2c_client(dev);

	xxx_i2c_read(client, 0, val, 0x0A);
	cmd[0] = val[0];

	xxx_i2c_write(client, 0, val, 1);

#if 0
	val = i2c_smbus_read_word_data(client, 0);
	LOG_I("val = %d", val);
#endif
	return 0;
#if 0
	int val = 0;


	LOG_I("val = %d", val);
	return sprintf(buf, "%d\n", val);
#endif
}

/**************************************************************************************************
 * @brief  : 属性 - r - dgb3
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static ssize_t xxx_dbg3_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	int val = 0;

	LOG_I("val = %d", val);
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

    if(kstrtoint(buf, 0, &val))
        return -EINVAL;
	
	LOG_I("val = %d", val);
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
	/* 获取引脚 */
	priv->pin = of_get_named_gpio(np, "gpios", 0);
	DS_RET(priv->pin, of_get_named_gpio);

	/* 获取中断号 */
	priv->irq = gpio_to_irq(priv->pin);
	DS_RET(priv->pin, gpio_to_irq);
	return 0;
}

/**************************************************************************************************
 * @brief  : 中断处理函数
 * @param  : 中断号
 * @param  : 私有数据
 * @return : 0成功, -1失败
**************************************************************************************************/
static irqreturn_t xxx_irq_handler(int irq, void *data)
{
	xxx_priv *priv = (xxx_priv *)data;

	int val = gpio_get_value(priv->pin);
	LOG_I("xxx irq %d", val);
	return IRQ_RETVAL(IRQ_HANDLED);
}

/**************************************************************************************************
 * @brief  : 设置硬件属性
 * @param  : 设备
 * @return : 0成功, -1失败
**************************************************************************************************/
static int set_hardware(xxx_priv *priv, struct device *dev)
{
	int ret;

	/* 申请GPIO */
	ret = devm_gpio_request(dev, priv->pin, "xxx-pin");
	DS_RET(ret, devm_gpio_request);

	/* 设置输出 */
	ret = gpio_direction_input(priv->pin);
	DS_RET(ret, gpio_direction_input);

	/* 设置中断 */
	ret = devm_request_irq(dev, priv->irq, xxx_irq_handler, IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING, "xxx-irq", priv);
	DS_RET(ret, devm_request_irq);

	return 0;
}

/**************************************************************************************************
 * @brief  : 驱动匹配
 * @param  : 平台设备
 * @return : 0成功, -1失败
**************************************************************************************************/
static int xxx_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int ret;
	xxx_priv *priv;
	struct device *dev = &client->dev;

	/* 申请内存空间 */
	priv = devm_kzalloc(dev, sizeof(xxx_priv), GFP_KERNEL);
	DP_RET(priv, devm_kzalloc);

	/* 初始化misc设备 */
	priv->dev.name  = "xxx";
	priv->dev.minor = MISC_DYNAMIC_MINOR;
	priv->dev.fops  = &xxx_fops;

	/* 解析设备树 */
	ret = parse_dtb(priv, dev, dev->of_node);
	CK_RET(ret < 0, ret);

	/* 设置硬件 */
	ret = set_hardware(priv, dev);
	CK_RET(ret < 0, ret);

	/* 注册MISC设备 */
	ret = misc_register(&priv->dev);
	DS_RET(ret, misc_register);

	/* 创建属性文件 */
	ret = sysfs_create_group(&client->dev.kobj, &xxx_attr_group);
	DS_RET(ret, sysfs_create_group);

	/* 设置私有数据 */
	i2c_set_clientdata(client, priv);
	LOG_I("xxx init");
	return 0;
}

/**************************************************************************************************
 * @brief  : 驱动删除
 * @param  : 设备
 * @return : 0成功, -1失败
**************************************************************************************************/
static int xxx_remove(struct i2c_client *client)
{
	/* 移除属性 */
	sysfs_remove_group(&client->dev.kobj, &xxx_attr_group);
	LOG_I("xxx remove");
	return 0;
}

static const struct i2c_device_id xxx_dev_id[] = {
	{"i2c-xxx", 0},
	{}
};

static struct i2c_driver xxx_driver = {
    .probe  = xxx_probe,
    .remove = xxx_remove,
	.driver	= {
		.owner = THIS_MODULE,
		.name = "xxx",
	},
	.id_table = xxx_dev_id,
};

module_i2c_driver(xxx_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZW");
MODULE_DESCRIPTION("dirver - xxx");
