#define pr_fmt(fmt) "xxx: " fmt 
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
#include <linux/mm.h>
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
	xxx_priv *priv = to_xxx_priv(filp->private_data);
	LOG_I("xxx open priv = %p", priv);
	return 0;
}

/**************************************************************************************************
 * @brief  : mmap
 * @param  : inode
 * @param  : 文件信息
**************************************************************************************************/
static int xxx_mmap(struct file *file, struct vm_area_struct *vma)
{
	int ret;
	uint32_t size;

	size = vma->vm_start - vma->vm_end;

	/* 修改权限为不缓存 */
	vma->vm_page_prot = phys_mem_access_prot(file, vma->vm_pgoff,
						 size,
						 vma->vm_page_prot);

	LOG_I("vmstart = %lx, vmend = %lx, phy = %lx", vma->vm_start, vma->vm_end, vma->vm_pgoff);

	/* 重新映射 */
	ret = remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff, size, vma->vm_page_prot);
	CI_RET_U(ret, remap_pfn_range);
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
	.mmap    = xxx_mmap,
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

static const struct attribute_group *xxx_attribute_groups[] = {
	&xxx_attr_group,
	NULL,
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
	CI_RET_U(priv->pin, of_get_named_gpio);

	/* 获取中断号 */
	priv->irq = gpio_to_irq(priv->pin);
	CI_RET_U(priv->pin, gpio_to_irq);
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
	CI_RET_U(ret, devm_gpio_request);

	/* 设置输出 */
	ret = gpio_direction_input(priv->pin);
	CI_RET_U(ret, gpio_direction_input);

	/* 设置中断 */
	ret = devm_request_irq(dev, priv->irq, xxx_irq_handler, IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING, "xxx-irq", priv);
	CI_RET_U(ret, devm_request_irq);

	return 0;
}

/**************************************************************************************************
 * @brief  : 驱动匹配
 * @param  : 平台设备
 * @return : 0成功, -1失败
**************************************************************************************************/
static int xxx_probe(struct platform_device *pdev)
{
	int ret;
	xxx_priv *priv;
	struct device *dev = &pdev->dev;

	/* 申请内存空间 */
	priv = devm_kzalloc(dev, sizeof(xxx_priv), GFP_KERNEL);
	CI_RET_P(priv, devm_kzalloc);

	/* 初始化MISC设备 */
	priv->dev.name   = "xxx";
	priv->dev.minor  = MISC_DYNAMIC_MINOR;
	priv->dev.fops   = &xxx_fops;
	priv->dev.groups = xxx_attribute_groups;

	/* 解析设备树 */
	ret = parse_dtb(priv, dev, dev->of_node);
	CK_RET_U(ret);

	/* 设置硬件 */
	ret = set_hardware(priv, dev);
	CK_RET_U(ret);
	
	/* 注册MISC设备 */
	ret = misc_register(&priv->dev);
	CI_RET_U(ret, misc_register);

	/* 保存私有数据 */
	platform_set_drvdata(pdev, priv);
	LOG_I("xxx init");
	return 0;
}

/**************************************************************************************************
 * @brief  : 驱动删除
 * @param  : 设备
 * @return : 0成功, -1失败
**************************************************************************************************/
static int xxx_remove(struct platform_device *pdev)
{
	xxx_priv *priv = platform_get_drvdata(pdev);

	/* 注销MISC设备 */
	misc_deregister(&priv->dev);
	LOG_I("xxx remove");
	return 0;
}

static const struct of_device_id xxx_match[] = {
	{ .compatible = "xxx" },
	{},
};
MODULE_DEVICE_TABLE(of, xxx_match);

static struct platform_driver xxx_driver = {
    .probe  = xxx_probe,
    .remove = xxx_remove,
	.driver	= {
		.name = "xxx",
		.of_match_table = of_match_ptr(xxx_match),
	},
};

module_platform_driver(xxx_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZW");
MODULE_DESCRIPTION("dirver - xxx");
