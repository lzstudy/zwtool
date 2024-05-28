#ifndef __LIGHT_H__
#define __LIGHT_H__
__BEGIN_DECLS
#include <stdint.h>
#include "light_type.h"

typedef struct _light_ops {
    int (*init)(void);                                                          /**@ 初始化. */
    void (*exit)(void);                                                         /**@ 去初始化. */
    int (*set_bright)(light_id id, uint8_t bright);                             /**@ 设置亮度. */
}light_ops;

light_ops *get_light_ops(void);                                                 /**# 获取light操作接口. */

__END_DECLS
#endif
