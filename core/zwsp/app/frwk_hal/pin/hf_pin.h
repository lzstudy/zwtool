#ifndef __HF_PIN_H__
#define __HF_PIN_H__
__BEGIN_DECLS

typedef enum _pin_dir {
    PIN_DIR_IN,                                                                 /**< 00' 引脚方向输入. */
    PIN_DIR_OUT,                                                                /**< 01' 引脚方向输出. */
}pin_dir;

typedef enum _pin_state {
    PIN_LOW,                                                                    /**< 00' 引脚拉低. */
    PIN_HIGH,                                                                   /**< 01' 引脚拉高. */
}pin_state;

typedef struct _hf_pin_ops {
    int (*init)(int pin, pin_dir dir, pin_state state);                         /**@ 引脚初始化. */
    int (*exit)(int pin);                                                       /**@ 引脚退出. */
    int (*set)(int pin, pin_state state);                                       /**@ 设置引脚状态. */
    int (*get)(int pin);                                                        /**@ 读取引脚值. */
}hf_pin_ops;

hf_pin_ops *get_hf_pin_ops(void);                                               /**# 获取PIN操作函数. */

__END_DECLS
#endif
