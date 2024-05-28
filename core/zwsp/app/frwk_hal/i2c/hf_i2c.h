#ifndef __HF_I2C_H__
#define __HF_I2C_H__
__BEGIN_DECLS
#include <stdint.h>

typedef struct _hf_i2c_handle {
    int fd;                                                                     /**@ 设备描述符. */
    uint8_t dev_addr;                                                           /**@ 设备地址. */
    uint8_t reg_len;                                                            /**@ 寄存器长度. */
}hf_i2c_handle;

typedef struct _hf_i2c_ops {
    hf_i2c_handle *(*init)(const char *dev, uint8_t dev_addr, uint8_t reg_len); /**@ 创建句柄. */
    void (*exit)(hf_i2c_handle *handle);                                        /**@ 销毁句柄. */
    int (*read)(hf_i2c_handle *handle, uint32_t reg, uint8_t *val, uint8_t len);/**@ 读取寄存器 - 8位. */
}hf_i2c_ops;

hf_i2c_ops *get_hf_i2c_ops(void);                                               /**# 获取I2C操作函数. */

__END_DECLS
#endif
