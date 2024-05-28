#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "sub1.h"
#include "codestd.h"

#define LED_BASE_PATH           "/sys/class/leds"                               /**< LED子系统基础路径 */
#define LED_ATTR_BRIGHTNESS     "brightness"                                    /**< LED亮度属性 */

/**************************************************************************************************
 * @brief  : 使用led子系统, 调整led亮度
 * @param  : led名字
 * @return : 0成功, 其他失败
 **************************************************************************************************/
static inline int hf_led_set(const char *name, uint8_t bright)
{
    CK_ARG(name == NULL, -1);

    int fd, ret;
    char attr[64], val[8];

    /* 拼接属性 */
    snprintf(attr, sizeof(attr), "%s/%s/%s", LED_BASE_PATH, name, LED_ATTR_BRIGHTNESS);
    snprintf(val, sizeof(val), "%d", bright);

    /* 打开属性文件 */
    fd = open(attr, O_WRONLY);
    CK_RET(fd < 0, fd);

    /* 写入属性 */
    ret = write(fd, val, sizeof(val));
    CK_RET(ret < 0, ret);

    /* 关闭文件 */
    close(fd);
    return 0;
}

static hf_led_ops _hf_led_ops = {
    .set = hf_led_set,
};

/**************************************************************************************************
 * @brief  : 获取引脚操作函数
 * @param  : None
 * @return : 0成功, 其他失败
 **************************************************************************************************/
hf_led_ops *get_hf_led_ops(void)
{
    return &_hf_led_ops;
}
