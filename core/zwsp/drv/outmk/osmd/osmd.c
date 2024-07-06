#define pr_fmt(fmt) "osmd: " fmt 
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/of_platform.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/backing-dev.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/poll.h>
#include <linux/acpi.h>
#include <linux/cpumask.h>
#include <linux/arm-smccc.h>
#include "ckret.h"



#define PHY_ADDR				0x60000000

typedef struct _osmd_priv {
	struct miscdevice misc;														/**@ misc设备. */
}osmd_priv;

/* IOC define */
#define	IOC_TEST				_IOR(0x21, 0, int)								/**< 等待事件. */

/* 上下文宏 */
#define to_osmd_priv(x)			container_of(x, osmd_priv, misc)					/**< 获取设备信息. */

static osmd_priv osmd;	
/**************************************************************************************************
 * @brief  : 接口 - open
 * @param  : inode
 * @param  : 文件信息
 * @return : 0成功, 其他失败
**************************************************************************************************/
static int osmd_open(struct inode *inode, struct file *filp)
{
	// osmd_priv *priv = to_osmd_priv(filp->private_data);
	// LOG_I("osmd open priv = %p", priv);
	return 0;
}

/**************************************************************************************************
 * @brief  : osmd lseek
 * @param  : inode
 * @param  : 文件信息
**************************************************************************************************/
static loff_t osmd_lseek(struct file *file, loff_t offset, int orig)
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
static int osmd_mmap(struct file *file, struct vm_area_struct *vma)
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
static int osmd_release(struct inode *inode, struct file *filp)
{
	// LOG_I("osmd release\n");
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
	switch(cmd)
	{
	case IOC_TEST:
		break;

	default:
		return -ENOIOCTLCMD;
	}

	return 0;
}

static const struct file_operations osmd_fops = {
	.owner	 = THIS_MODULE,
	.llseek	 = osmd_lseek,
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
static ssize_t osmd_func(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int val;
	// osmd_priv *priv = to_osmd_priv(dev->driver_data);
	struct arm_smccc_res res;

    if(kstrtoint(buf, 0, &val)) {
        return -EINVAL;
	}

	switch(val)
	{
	case 1:
		arm_smccc_smc(0xC4000003, 0x300, PHY_ADDR, 0, 0, 0, 0, 0, &res);
		LOG_I("start cpu addr = 0x%lx smcc ret = %d",  PHY_ADDR, res.a0);
		break;

	case 2:
		arm_smccc_smc(0x84000002, 0x300, PHY_ADDR, 0, 0, 0, 0, 0, &res);
		LOG_I("smcc ret = %d",  res.a0);
		break;

	}
	
	return count;
}

/**************************************************************************************************
 * @brief  : 属性 - w - dgb1
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static ssize_t osmd_get_cpuid(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	// u32 mpidr;
	int val;
	// osmd_priv *priv = to_osmd_priv(dev->driver_data);

    if(kstrtoint(buf, 0, &val)) {
        return -EINVAL;
	}


	return count;
}



static DEVICE_ATTR(func, 0220, NULL, osmd_func);
static DEVICE_ATTR(cpuid, 0220, NULL, osmd_get_cpuid);


static struct attribute *osmd_attr[] = {
	&dev_attr_func.attr,
	&dev_attr_cpuid.attr,
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
	osmd_priv *priv = &osmd;

	/* 初始化MISC设备 */
	priv->misc.name   = "osmd";
	priv->misc.minor  = MISC_DYNAMIC_MINOR;
	priv->misc.fops   = &osmd_fops;
	priv->misc.groups = osmd_attribute_groups;
	
	/* 注册MISC设备 */
	ret = misc_register(&priv->misc);
	CI_RET(misc_register, ret < 0, ret);

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
	osmd_priv *priv = &osmd;

	misc_deregister(&priv->misc);
	LOG_I("osmd exit.");
}

module_init(osmd_init);
module_exit(osmd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZW");
MODULE_DESCRIPTION("kmod - osmd");
