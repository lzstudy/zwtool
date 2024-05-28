#ifndef __HF_PWM_H__
#define __HF_PWM_H__
__BEGIN_DECLS
#include <stdint.h>

typedef struct _hf_pwm_ops {
    void (*export)(uint8_t chip, uint8_t port);                                 /**@ 导出PWM */
    void (*unexport)(uint8_t chip, uint8_t port);                               /**@ 取消导出 */
    int (*set)(uint8_t chip, uint8_t port, uint32_t period, uint32_t duty);     /**@ 设置参数 */
    int (*enable)(uint8_t chip, uint8_t port, uint8_t state);                   /**@ 是否是能 */
}hf_pwm_ops;

hf_pwm_ops *get_hf_pwm_ops(void);                                               /**# 获取PIN操作函数. */

__END_DECLS
#endif
