#ifndef __LIGHT_CONFIG_H__
#define __LIGHT_CONFIG_H__
__BEGIN_DECLS
#include "light_type.h"

typedef enum light_type {
    LIGHT_TYPE_GPIO,                                                            /**< 00' GPIO灯. */
    LIGHT_TYPE_PWM,                                                             /**< 01' PWM灯. */
}light_type;

typedef struct _pwm_light_relation {
    light_id id;                                                                /**@ 灯ID. */
    const char *port;                                                           /**@ 端口. */
    uint32_t period;                                                            /**@ 周期. */
    uint8_t max_bright;                                                         /**@ 最大亮度. */
}pwm_light_relation;

typedef struct _gpio_light_relation {
    light_id id;                                                                /**@ 灯ID. */
    uint16_t pin;                                                               /**@ 引脚. */
}gpio_light_relation;

typedef struct light_relation {
    light_id id;                                                                /**@ 灯ID. */
    light_type type;                                                            /**@ 灯类型. */
}light_relation;

static const pwm_light_relation pwm_light_rlt[] = {
    {LIGHT_M_CAM_WHITE, "pwmchip1", 100000, 40},
    {LIGHT_M_CAM_IR,    "pwmchip2", 100000, 40},
    {LIGHT_S_CAM_IR,    "pwmchip3", 100000, 40},
};

static const gpio_light_relation gpio_light_rlt[] = {
    {LIGHT_TEST, 12},
};

static const light_relation light_rlt[] = {
    {LIGHT_M_CAM_WHITE, LIGHT_TYPE_PWM},
    {LIGHT_M_CAM_IR,    LIGHT_TYPE_PWM},
    {LIGHT_S_CAM_IR,    LIGHT_TYPE_PWM},
    {LIGHT_TEST,        LIGHT_TYPE_GPIO},
};

#define PWM_LIGHT_COUNT         ARRAY_SIZE(pwm_light_rlt)                       /**< PWM灯数量. */
#define GPIO_LIGHT_COUNT        ARRAY_SIZE(gpio_light_rlt)                      /**< GPIO灯数量. */
#define LIGHT_COUNT             (PWM_LIGHT_COUNT + GPIO_LIGHT_COUNT)            /**< 灯数量. */
#define GPIO_LIGHT_START        PWM_LIGHT_COUNT                                 /**< GPIO灯开始INDEX. */

__END_DECLS
#endif
