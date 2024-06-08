#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/of_platform.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/mm.h>
#include <linux/arm-smccc.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include "ckerr.h"

typedef struct _osmd_priv {
	struct miscdevice misc;														/**@ misc设备. */
}osmd_priv;

static osmd_priv osmd;															/**# 私有数据 */

/* IOC define */
#define	IOC_GET_MEM				_IOR(0x21, 0, int)								/**< 等待事件. */

/* 上下文宏 */
#define to_osmd_priv(x)			container_of(x, osmd_priv, misc)				/**< 获取设备信息. */


/**************************************************************************************************
 * @brief  : 接口 - open
 * @param  : inode
 * @param  : 文件信息
 * @return : 0成功, 其他失败
**************************************************************************************************/
static int osmd_open(struct inode *inode, struct file *filp)
{
	osmd_priv *priv = to_osmd_priv(filp->private_data);
	LOG_I("priv addr = %p", priv);
	LOG_I("osmd open");
	return 0;
}

/**************************************************************************************************
 * @brief  : mmap
 * @param  : inode
 * @param  : 文件信息
**************************************************************************************************/
static int osmd_mmap(struct file *file, struct vm_area_struct *vma)
{
	int ret;
	size_t size = vma->vm_end - vma->vm_start;

	// vma->vm_page_prot = mcs_phys_mem_access_prot(file, vma->vm_pgoff, size, vma->vm->page_port);
	
	
	void __iomem *virt = ioremap(0x70000000, 1000);
	uint32_t addr = virt_to_phys(virt);
	LOG_I("remap = %lx, phy = %lx", virt, addr);


	void __iomem *virt1 = phys_to_virt(0x70000000);
	uint32_t addr1 = virt_to_phys(virt1);
	LOG_I("pv = %lx, phy = %lx", virt1, addr1);



	/* 修改权限为不缓存 */
	__pgprot_modify(vma->vm_page_prot, PTE_ATTRINDX_MASK, PTE_ATTRINDX(MT_NORMAL_NC) | PTE_PXN | PTE_UXN);

	/* 重新映射 */
	ret = remap_pfn_range(vma, virt, vma->vm_pgoff, size, vma->vm_page_prot);
	// ret = remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff, size, vma->vm_page_prot);
	CI_RET(ret < 0, -EAGAIN, "remap pfn range (%d)", ret);

	iounmap(virt);
	return 0;
}

/**************************************************************************************************
 * @brief  : 接口 - release
 * @param  : inode
 * @param  : 文件信息
 * @return : 0成功, 其他失败
**************************************************************************************************/
static int osmd_release(struct inode *inode, struct file *filp)
{
	LOG_I("osmd release\n");
	return 0;
}

/**************************************************************************************************
 * @brief  : 接口 - ioctl
 * @param  : 文件信息
 * @param  : 命令码
 * @param  : 参数
 * @return : 0成功, -1失败
**************************************************************************************************/
static long osmd_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	void __iomem *virt;

	switch(cmd)
	{
	case IOC_GET_MEM:

		pr_info("ioc get mem");
		break;

	default:
		return -ENOIOCTLCMD;
	}

	return 0;
}

static const struct file_operations osmd_fops = {
	.owner	 = THIS_MODULE,
	.open	 = osmd_open,
	.mmap    = osmd_mmap,
	.release = osmd_release,
	.unlocked_ioctl = osmd_ioctl,
};

/**************************************************************************************************
 * @brief  : 属性 - w - dgb1
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static ssize_t osmd_dbg1_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int val;
	osmd_priv *priv = to_osmd_priv(dev->driver_data);

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
static ssize_t osmd_dbg2_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	int val = 0;
	osmd_priv *priv = to_osmd_priv(dev->driver_data);

	LOG_I("priv addr = %p, val = %d", priv, val);
	return sprintf(buf, "%d\n", val);
}

/**************************************************************************************************
 * @brief  : 属性 - r - dgb3
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static ssize_t osmd_dbg3_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	int val = 0;
	osmd_priv *priv = to_osmd_priv(dev->driver_data);

	LOG_I("priv addr = %p, val = %d", priv, val);
	return sprintf(buf, "%d\n", val);
}

/**************************************************************************************************
 * @brief  : 属性 - w - dgb3
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static ssize_t osmd_dbg3_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int val;
	osmd_priv *priv = to_osmd_priv(dev->driver_data);

    if(kstrtoint(buf, 0, &val))
        return -EINVAL;
	
	LOG_I("priv addr = %p, val = %d", priv, val);
	return count;
}

static DEVICE_ATTR(dbg1, 0220, NULL, osmd_dbg1_store);
static DEVICE_ATTR(dbg2, 0440, osmd_dbg2_show, NULL);
static DEVICE_ATTR(dbg3, 0640, osmd_dbg3_show, osmd_dbg3_store);

static struct attribute *osmd_attr[] = {
	&dev_attr_dbg1.attr,
	&dev_attr_dbg2.attr,
	&dev_attr_dbg3.attr,
	NULL
};

static struct attribute_group osmd_attr_group = {
	.name = "attr",
	.attrs = osmd_attr,
};

static const struct attribute_group *osmd_attribute_groups[] = {
	&osmd_attr_group,
	NULL,
};

/**************************************************************************************************
 * @brief  : 模块初始化
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static int __init osmd_init(void)
{
	int ret;
	osmd_priv *op = &osmd;

	/* 初始化MISC设备 */
	op->misc.name   = "osmd";
	op->misc.minor  = MISC_DYNAMIC_MINOR;
	op->misc.fops   = &osmd_fops;
	op->misc.groups = osmd_attribute_groups;
	
	/* 注册MISC设备 */
	ret = misc_register(&op->misc);
	CI_RET(ret < 0, ret, "misc register fail (%d)", ret);

	LOG_I("osmd init");
	return 0;
}

/**************************************************************************************************
 * @brief  : 模块退出
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static void __exit osmd_exit(void)
{
	osmd_priv *op = &osmd;

	/* 注销MISC设备 */
	misc_deregister(&op->misc);
	LOG_I("osmd exit.");
}

module_init(osmd_init);
module_exit(osmd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZW");
MODULE_DESCRIPTION("dirver - osmd");
