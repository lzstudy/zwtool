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
#include <linux/backing-dev.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include "ckerr.h"

typedef struct _xxx_priv {
	struct miscdevice misc;														/**@ misc设备. */
}xxx_priv;

/* IOC define */
#define	IOC_TEST				_IOR(0x21, 0, int)								/**< 等待事件. */

/* 上下文宏 */
#define to_xxx_priv(x)			container_of(x, xxx_priv, misc)					/**< 获取设备信息. */

static xxx_priv xxx;	
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
 * @brief  : xxx lseek
 * @param  : inode
 * @param  : 文件信息
**************************************************************************************************/
static loff_t xxx_lseek(struct file *file, loff_t offset, int orig)
{
	loff_t ret;

	inode_lock(file_inode(file));
	switch (orig) {
	case SEEK_CUR:
		offset += file->f_pos;
		/* fall through */
	case SEEK_SET:
		/* to avoid userland mistaking f_pos=-9 as -EBADF=-9 */
		if ((unsigned long long)offset >= -MAX_ERRNO) {
			ret = -EOVERFLOW;
			break;
		}
		file->f_pos = offset;
		ret = file->f_pos;
		force_successful_syscall_return();
		break;
	default:
		ret = -EINVAL;
	}
	inode_unlock(file_inode(file));
	return ret;
}

/**************************************************************************************************
 * @brief  : mmap
 * @param  : inode
 * @param  : 文件信息
**************************************************************************************************/
static int xxx_mmap(struct file *file, struct vm_area_struct *vma)
{
	size_t size = vma->vm_end - vma->vm_start;
	phys_addr_t offset = (phys_addr_t)vma->vm_pgoff << PAGE_SHIFT;

	/* Does it even fit in phys_addr_t? */
	if (offset >> PAGE_SHIFT != vma->vm_pgoff)
		return -EINVAL;

	/* It's illegal to wrap around the end of the physical address space. */
	if (offset + (phys_addr_t)size - 1 < offset)
		return -EINVAL;

	vma->vm_page_prot = phys_mem_access_prot(file, vma->vm_pgoff,
						 size,
						 vma->vm_page_prot);

	// vma->vm_ops = &mmap_mem_ops;

	/* Remap-pfn-range will mark the range VM_IO */
	if (remap_pfn_range(vma,
			    vma->vm_start,
			    vma->vm_pgoff,
			    size,
			    vma->vm_page_prot)) {
		return -EAGAIN;
	}
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
	.llseek	 = xxx_lseek,
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
 * @brief  : 模块初始化
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static int __init xxx_init(void)
{
	int ret;
	xxx_priv *priv = &xxx;

	/* 初始化MISC设备 */
	priv->misc.name   = "xxx";
	priv->misc.minor  = MISC_DYNAMIC_MINOR;
	priv->misc.fops   = &xxx_fops;
	priv->misc.groups = xxx_attribute_groups;
	
	/* 注册MISC设备 */
	ret = misc_register(&priv->misc);
	CI_RET_U(ret, misc_register);

	LOG_I("osmd init");
	return 0;
}


/**************************************************************************************************
 * @brief  : 模块退出
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static void __exit xxx_exit(void)
{
	xxx_priv *priv = &xxx;

	misc_deregister(&priv->misc);
	LOG_I("xxx exit.");
}

module_init(xxx_init);
module_exit(xxx_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZW");
MODULE_DESCRIPTION("kmod - xxx");
