#ifndef __HF_PIN_H__
#define __HF_PIN_H__
__BEGIN_DECLS
#include <stdint.h>

typedef struct _hf_led_ops {
    int (*set)(const char *name, uint8_t bright);                               /**@ 设置亮度 */
}hf_led_ops;

hf_led_ops *get_hf_led_ops(void);                                               /**# 获取PIN操作函数. */

__END_DECLS
#endif
