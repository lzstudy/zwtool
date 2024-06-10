#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include "codestd.h"

typedef struct _xxx_priv {
	dev_t devid;																/**@ 设备ID. */
	struct cdev cdev;															/**@ 设备. */
	struct class *class;														/**@ class. */
	struct device *device;														/**@ 设备. */
}xxx_priv;

static xxx_priv *priv;

/**************************************************************************************************
 * @brief  : 打卡设备
 * @param  : 节点
 * @param  : 文件
 * @return : 0成功, -1失败
**************************************************************************************************/
static int xxx_open(struct inode *inode, struct file *filp)
{
	LOG_I("xxx open");
	return 0;
}

/**************************************************************************************************
 * @brief  : 释放设备
 * @param  : 节点
 * @param  : 文件
 * @return : 0成功, -1失败
**************************************************************************************************/
static int xxx_release(struct inode *inode, struct file *filp)
{
	LOG_I("xxx release");
	return 0;
}

/**************************************************************************************************
 * @brief  : ioctl
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static long xxx_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	LOG_I("xxx ioctl");
	return 0;
}

static const struct file_operations xxx_fops = {
	.owner = THIS_MODULE,
	.open = xxx_open,
	.release = xxx_release,
	.unlocked_ioctl = xxx_ioctl,
} ;

/**************************************************************************************************
 * @brief  : 模块初始化
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static int __init xxx_init(void)
{
	int ret;

	/* 申请私有数据 */
	priv = kcalloc(1, sizeof(*priv), GFP_KERNEL);
	CI_RET(priv == NULL, -ENOMEM, "xxx kcalloc priv fail");

	/* 申请主设备号 */
	ret = alloc_chrdev_region(&priv->devid, 0, 1, "xxx");
	CI_GOTO(ret < 0, out_kfree, "xxx alloc chrdev fail %d", ret);

	/* 初始化字符设备 */
	cdev_init(&priv->cdev, &xxx_fops);
	priv->cdev.owner = THIS_MODULE;

	/* 注册字符设备 */
	ret = cdev_add(&priv->cdev, priv->devid, 1);
	CI_GOTO(ret < 0, out_kfree, "xxx cdev add fail %d", ret);

	/* 创建分类 */
	priv->class = class_create(THIS_MODULE, "xxx");
	CI_GOTO(!priv->class, out_cdev_del, "xxx create class fail");

	/* 创建设备 */
	priv->device = device_create(priv->class, NULL, priv->devid, NULL, "xxx");
	CI_GOTO(!priv->device, out_class_destory, "xxx device create fail");

	LOG_I("xxx init");
	return 0;

out_class_destory:
	class_destroy(priv->class);
out_cdev_del:
	cdev_del(&priv->cdev);
out_kfree:
	kfree(priv);
	return ret;
}

/**************************************************************************************************
 * @brief  : 模块退出
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static void __exit xxx_exit(void)
{
	device_destroy(priv->class, priv->devid);
	class_destroy(priv->class);
	cdev_del(&priv->cdev);
	kfree(priv);
	LOG_I("xxx exit");
}

module_init(xxx_init);
module_exit(xxx_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZW>");
MODULE_DESCRIPTION("dirver - xxx");
