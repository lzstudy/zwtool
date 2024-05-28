#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "hf_pin.h"
#include "codestd.h"

/**************************************************************************************************
 * @brief  : 写入引脚文件
 * @param  : 文件路径
 * @param  : 值
 * @return : 0成功, 其他失败
 **************************************************************************************************/
static int _write_pin_file(const char *path, const char *val)
{
    int fd, ret;

    /* 打开引脚属性文件 */
    fd = open(path, O_WRONLY);
    CI_RET(fd < 0, fd, "write pin file open %s fail %d", path, fd);

    /* 写入数据 */
    ret = write(fd, val, strlen(val) + 1);
    close(fd);
    return ret;
}

/**************************************************************************************************
 * @brief  : 读引脚文件
 * @param  : 文件路径
 * @param  : 值
 * @return : 0成功, 其他失败
 **************************************************************************************************/
static inline int read_pin_file(const char *path, char *val)
{
    int fd, ret;

    /* 打开引脚属性文件 */
    fd = open(path, O_RDONLY);
    CI_RET(fd < 0, fd, "read pin file open %s fail %d", path, fd);

    /* 读取数据 */
    ret = read(fd, val, 16);
    close(fd);
    return ret;
}

/**************************************************************************************************
 * @brief  : 写入整形数据
 * @param  : 文件路径
 * @param  : 值
 * @return : 0成功, 其他失败
 **************************************************************************************************/
static inline int write_pin_file(const char *path, int val)
{
    char str[10];

    sprintf(str, "%d", val);
    return _write_pin_file(path, str);
}

/**************************************************************************************************
 * @brief  : 引脚初始化
 * @param  : 引脚ID
 * @param  : 引脚方向
 * @param  : 引脚状态
 * @return : 0成功, 其他失败
 **************************************************************************************************/
static int hf_pin_init(int pin, pin_dir dir, pin_state state)
{
    int ret;
    char buf[100];

    /* 导出设备 */
    write_pin_file("/sys/class/gpio/export", pin);

    /* 设置方向 */
    sprintf(buf, "/sys/class/gpio/gpio%d/direction", pin);
    ret = _write_pin_file(buf, (dir ? "out" : "in"));
    CK_RET(ret < 0, ret);

    /* 设置初始电平 */
    sprintf(buf, "/sys/class/gpio/gpio%d/value", pin);
    ret = write_pin_file(buf, state);
    CK_RET(ret < 0, ret);

    return 0;
}

/**************************************************************************************************
 * @brief  : 引脚退出
 * @param  : 引脚ID
 * @return : 0成功, 其他失败
 **************************************************************************************************/
static inline int hf_pin_exit(int pin)
{
    return write_pin_file("/sys/class/gpio/unexport", pin);
}

/**************************************************************************************************
 * @brief  : 设置引脚状态
 * @param  : 引脚ID
 * @param  : 引脚状态
 * @return : 0成功, 其他失败
 **************************************************************************************************/
static inline int hf_pin_set(int pin, pin_state state)
{
    char buf[100];

    sprintf(buf, "sys/class/gpio/gpio%d/value", pin);
    return write_pin_file(buf, state);   
}

/**************************************************************************************************
 * @brief  : 获取引脚状态
 * @param  : 引脚ID
 * @return : 0成功, 其他失败
 **************************************************************************************************/
static inline int hf_pin_get(int pin)
{
    int ret;
    char buf[100], val[16];

    /* 读取文件 */
    sprintf(buf, "sys/class/gpio/gpio%d/value", pin);
    ret = read_pin_file(buf, val);
    CK_RET(ret < 0, ret);

    return atoi(val);
}

static hf_pin_ops _hf_pin_ops = {
    .init = hf_pin_init,
    .exit = hf_pin_exit,
    .set  = hf_pin_set,
    .get  = hf_pin_get,
};

/**************************************************************************************************
 * @brief  : 获取引脚操作函数
 * @param  : None
 * @return : 0成功, 其他失败
 **************************************************************************************************/
hf_pin_ops *get_hf_pin_ops(void)
{
    return &_hf_pin_ops;
}

