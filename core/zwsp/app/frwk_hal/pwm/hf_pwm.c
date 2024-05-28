#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "hf_pwm.h"
#include "hf_utils.h"
#include "codestd.h"

#define PWM_BASE_PATH           "/sys/class/pwm/pwmchip"                       /**< 具体根据平台修改 */
#define PWM_ATTR_EXPORT         "export"                                        /**< 属性 - 导出 */
#define PWM_ATTR_UNEXPORT       "unexport"                                      /**< 属性 - 关闭 */
#define PWM_ATTR_POLARITY       "polarity"                                      /**< 属性 - 极性 */
#define PWM_ATTR_DUTY_CYCLE     "duty_cycle"                                    /**< 属性 - 占空比 */
#define PWM_ATTR_PERIOD         "period"                                        /**< 属性 - 周期 */
#define PWM_ATTR_ENABLE         "enable"                                        /**< 属性 - 是能 */



/**************************************************************************************************
 * @brief  : pwm导出
 * @param  : 芯片ID, 
 * @param  : 端口
 * @return : 0成功, 其他失败
 **************************************************************************************************/
static void hf_pwm_export(uint8_t chip, uint8_t port)
{
    char attr[64], val[16];

    /* 设置属性和属性值 */
    snprintf(attr, sizeof(attr), "%s%d/%s", PWM_BASE_PATH, chip, PWM_ATTR_EXPORT);
    snprintf(val, sizeof(val), "%d", port);
    attr_file_set(attr, val);
}

/**************************************************************************************************
 * @brief  : pwm 取消导出
 * @param  : 芯片ID, 
 * @param  : 端口
 * @return : 0成功, 其他失败
 **************************************************************************************************/
static void hf_pwm_unexport(uint8_t chip, uint8_t port)
{
    char attr[64], val[16];

    /* 设置属性和属性值 */
    snprintf(attr, sizeof(attr), "%s%d/%s", PWM_BASE_PATH, chip, PWM_ATTR_UNEXPORT);
    snprintf(val, sizeof(val), "%d", port);
    attr_file_set(attr, val);
}

/**************************************************************************************************
 * @brief  : pwm设置参数
 * @param  : 芯片ID, 
 * @param  : 端口
 * @return : 0成功, 其他失败
 **************************************************************************************************/
static int hf_pwm_set(uint8_t chip, uint8_t port, uint32_t period, uint32_t duty)
{
    int ret;
    char attr[64], val[16];

    /* 设置period */
    snprintf(attr, sizeof(attr), "%s%d/pwm%d/%s", PWM_BASE_PATH, chip, port, PWM_ATTR_PERIOD);
    snprintf(val, sizeof(val), "%d", period); 
    ret = attr_file_set(attr, val);
    CK_RET(ret < 0, ret);

    /* 设置duty */
    snprintf(attr, sizeof(attr), "%s%d/pwm%d/%s", PWM_BASE_PATH, chip, port, PWM_ATTR_DUTY_CYCLE);
    snprintf(val, sizeof(val), "%d", duty); 
    ret = attr_file_set(attr, val);
    CK_RET(ret < 0, ret);

    return 0;
}

/**************************************************************************************************
 * @brief  : 使能PWM
 * @param  : 芯片ID, 
 * @param  : 端口
 * @return : 0成功, 其他失败
 **************************************************************************************************/
static int hf_pwm_enable(uint8_t chip, uint8_t port, uint8_t state)
{
    int ret;
    char attr[64], val[16];

    snprintf(attr, sizeof(attr), "%s%d/pwm%d/%s", PWM_BASE_PATH, chip, port, PWM_ATTR_DUTY_CYCLE);
    snprintf(val, sizeof(val), "%d", state); 
    ret = attr_file_set(attr, val);
    CK_RET(ret < 0, ret);

    return 0;
}

static hf_pwm_ops _hf_pwm_ops = {
    .export   = hf_pwm_export,
    .unexport = hf_pwm_unexport,
    .set      = hf_pwm_set,
    .enable   = hf_pwm_enable,

};

/**************************************************************************************************
 * @brief  : 获取引脚操作函数
 * @param  : None
 * @return : 0成功, 其他失败
 **************************************************************************************************/
hf_pwm_ops *get_hf_pwm_ops(void)
{
    return &_hf_pwm_ops;
}
