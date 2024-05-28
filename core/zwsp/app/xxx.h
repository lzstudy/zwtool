#ifndef __XXX_H__
#define __XXX_H__
 #include <sys/cdefs.h>
__BEGIN_DECLS
#include <stdint.h>

#if 0
typedef struct _xxx_handle {
    int fd;                                                                     /**@ 设备描述符. */
}xxx_handle;
#endif

typedef struct _xxx_ops {
    int (*init)(uint8_t arg);                                                   /**@ 初始化 */
    void (*exit)(void);                                                         /**@ 推出 */
}xxx_ops;

xxx_ops *get_xxx_ops(void);                                                     /**# 获取I2C操作函数. */

__END_DECLS
#endif
