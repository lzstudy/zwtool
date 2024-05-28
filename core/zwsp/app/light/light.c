#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include "codestd.h"
#include "fw_pwm.h"
#include "fw_pin.h"
#include "light_config.h"
#include "light.h"

typedef struct _light_context {
    pwm_handle *handle[PWM_LIGHT_COUNT];
}light_context;

static light_context light_cxt;

/**************************************************************************************************
 * @brief  : pwm light 销毁
 * @param  : 灯控上下文
 * @param  : 注销的数量
 * @return : None
**************************************************************************************************/
static inline void pwm_light_exit(uint8_t size)
{
    light_context *cxt = &light_cxt;

    for(uint8_t i = 0 ; i < size ; i++)
    {   
        get_pwm_ops()->exit(cxt->handle[i]);
    }
}

/**************************************************************************************************
 * @brief  : pwm灯初始化
 * @param  : None
 * @return : 0 - 成功, 其他 - 失败
**************************************************************************************************/
static int pwm_light_init(void)
{
    uint8_t i = 0;
    const pwm_light_relation *rlt;
    light_context *cxt = &light_cxt;

    for(i = 0 ; i < PWM_LIGHT_COUNT ; i++)
    {
        rlt = &pwm_light_rlt[i];

        cxt->handle[i] = get_pwm_ops()->init(rlt->port, 0, rlt->period);
        CHECK_GOTO_EX(cxt->handle[i] == NULL, err_pwm_led_init);
    }

    return 0;

err_pwm_led_init:
    pwm_light_exit(i - 1);
    return -1;
}

/**************************************************************************************************
 * @brief  : gpio light 注销
 * @param  : 映射表
 * @param  : 注销的数量
 * @return : None
**************************************************************************************************/
static inline void gpio_light_exit(uint8_t size)
{
    const gpio_light_relation *rlt;

    for(uint8_t i = 0 ; i < size ; i++)
    {
        rlt = &gpio_light_rlt[i];
        get_pin_ops()->exit(rlt->pin);
    }
}

/**************************************************************************************************
 * @brief  : GPIO灯初始化
 * @param  : None
 * @return : 0 - 成功, 其他 - 失败
**************************************************************************************************/
static int gpio_light_init(void)
{
    int ret;
    uint8_t i = 0;
    const gpio_light_relation *rlt;

    for(i = 0 ; i < GPIO_LIGHT_COUNT ; i++)
    {
        rlt = &gpio_light_rlt[i];

        ret = get_pin_ops()->init(rlt->pin, PIN_DIR_OUT, PIN_LOW);
        CHECK_GOTO_EX(ret < 0, err_gpio_led_init);
    }

    return ret;

err_gpio_led_init:
    gpio_light_exit(GPIO_LIGHT_COUNT);
    return ret;
}

/**************************************************************************************************
 * @brief  : 设置PWM灯灯
 * @param  : 灯ID
 * @param  : 灯亮度
 * @return : 0 成功, 其他失败
**************************************************************************************************/
static inline int pwm_light_set(light_context *cxt, light_id id, uint8_t bright)
{
    pwm_handle *handle = cxt->handle[id];
    const pwm_light_relation *rlt = &pwm_light_rlt[id];

    CHECK_PARAM(bright > rlt->max_bright, -1, "%d light set %d bright > max %d", id, bright, rlt->max_bright);
    
    get_pwm_ops()->ctrl(handle, bright);
    return 0;
}

/**************************************************************************************************
 * @brief  : 设置GPIO灯
 * @param  : 灯ID
 * @param  : 灯状态
 * @return : 0 成功, 其他失败
**************************************************************************************************/
static inline int gpio_light_set(light_id id, pin_state state)
{
    const gpio_light_relation *rlt = &gpio_light_rlt[id - GPIO_LIGHT_START];
    get_pin_ops()->set(rlt->pin, state);
    return 0;
}

/**************************************************************************************************
 * @brief  : 灯初始化函数
 * @param  : None
 * @return : 0 成功, 其他失败
**************************************************************************************************/
static int light_init(void)
{
    int ret;

    /* 初始化PWM灯 */
    ret = pwm_light_init();
    _CHECK_RET(ret < 0, ret);

    /* 初始化GPIO灯 */
    ret = gpio_light_init();
    CHECK_GOTO_EX(ret < 0, err_light_init);

    return ret;

err_light_init:
    pwm_light_exit(PWM_LIGHT_COUNT);
    return ret;
}

/**************************************************************************************************
 * @brief  : 灯去初始化函数
 * @param  : None
 * @return : None
**************************************************************************************************/
static void light_exit(void)
{
    /* 注销PWM灯 */
    pwm_light_exit(PWM_LIGHT_COUNT);

    /* 注销GPIO灯 */
    gpio_light_exit(GPIO_LIGHT_COUNT);
}

/**************************************************************************************************
 * @brief  : 设置灯亮度
 * @param  : 灯ID
 * @param  : 亮度
 * @return : None
**************************************************************************************************/
static int light_set_bright(light_id id, uint8_t bright)
{
    CHECK_PARAM(id > LIGHT_COUNT, -1, "light id(%d) > max(%ld)", id, LIGHT_COUNT);

    int ret;

    if(id < GPIO_LIGHT_START)
        ret = pwm_light_set(&light_cxt, id, bright);
    else
        ret = gpio_light_set(id, !!bright);

    return ret;
}

static light_ops _light_ops = {
    .init = light_init,
    .exit = light_exit,
    .set_bright = light_set_bright,
};

/**************************************************************************************************
 * @brief  : 获取操作函数
 * @param  : None
 * @return : None
**************************************************************************************************/
light_ops *get_light_ops(void)
{
    return &_light_ops;
}

#if 1
/**************************************************************************************************
 * @brief  : 测试程序
 * @param  : None
 * @return : None
**************************************************************************************************/
int main(int argc, char const *argv[])
{
    get_light_ops()->init();

    break_line("set pwm1 40");
    get_light_ops()->set_bright(LIGHT_M_CAM_WHITE, 30);

    break_line("set pwm1 50");
    get_light_ops()->set_bright(LIGHT_M_CAM_WHITE, 50);

    break_line("exit");
    get_light_ops()->exit();

    return 0;
}
#endif

