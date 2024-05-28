#include <linux/kernel.h>
#include <linux/module.h>
#include "codestd.h"

/**************************************************************************************************
 * @brief  : 模块初始化
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static int __init xxx_init(void)
{
	LOG_I("xxx init");
	return 0;
}

/**************************************************************************************************
 * @brief  : 模块退出
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static void __exit xxx_exit(void)
{
	LOG_I("xxx exit");
}

module_init(xxx_init);
module_exit(xxx_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("LiZheng");
MODULE_DESCRIPTION("dirver - xxx");
