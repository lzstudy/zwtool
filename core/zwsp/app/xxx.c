#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include "codestd.h"
#include "xxx.h"

/**************************************************************************************************
 * @brief  : xxx 初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static int xxx_init(uint8_t arg)
{
    return 0;
}


/**************************************************************************************************
 * @brief  : xxx 推出
 * @param  : None
 * @return : None
**************************************************************************************************/
static void xxx_exit(uint8_t arg)
{
}



static xxx_ops _xxx_ops = {
    .init = xxx_init,
    .exit = xxx_exit, 
};

/**************************************************************************************************
 * @brief  : 获取操作函数
 * @param  : None
 * @return : 0成功, 其他失败
 **************************************************************************************************/
xxx_ops *get_xxx_ops(void)
{
    return &_xxx_ops;
}
