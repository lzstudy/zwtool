#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include "codestd.h"
#include "hf_i2c.h"

# if 0
#define I2C_ADAPTER             "/dev/i2c-3"                                    /**< i2c适配器. */
#define I2C_DEV_ADDR            0x0B                                            /**< 设备从机地址. */

#define TEST_REG_R              0x0D                                            /**< 要测试的读取寄存器. */
#define TEST_READ_SIZE          2                                               /**< 读取寄存器字节数. */ 



static i2c_dev_handle i2cdev_handle;

/**************************************************************************************************
 * @brief  : 测试程序
 * @param  : None
 * @return : None
**************************************************************************************************/
static int i2c_read(i2c_dev_handle *handle, uint8_t reg, uint8_t *val, uint8_t len)
{
    struct i2c_msg msg[2];
    struct i2c_rdwr_ioctl_data rid;

    /* 设置要读的寄存器 */
    msg[0].addr  = handle->addr;
    msg[0].flags = 0;
    msg[0].len   = 1;
    msg[0].buf   = &reg;

    /* 设置数据保存信息 */
    msg[1].addr  = handle->addr;
    msg[1].flags = I2C_M_RD;
    msg[1].len   = len;
    msg[1].buf   = val;

    /* 设置消息数量 */
    rid.msgs  = msg;
    rid.nmsgs = 2;

    return ioctl(handle->fd, I2C_RDWR, &rid);
}

/**************************************************************************************************
 * @brief  : i2c初始化
 * @param  : i2c设备句柄
 * @return : None
**************************************************************************************************/
static void i2c_init(i2c_dev_handle *handle)
{
    handle->addr = I2C_DEV_ADDR;
    handle->fd = open(I2C_ADAPTER, O_RDWR);
    CK_NRET_INFO(handle->fd < 0, "%s dev not exist : %d", I2C_ADAPTER, handle->fd);
}

/**************************************************************************************************
 * @brief  : i2c测试读取
 * @param  : None
 * @return : None
**************************************************************************************************/
static void i2c_test_read(i2c_dev_handle *handle)
{
    char buf[100];
    uint8_t val[TEST_READ_SIZE];

    /* 读取寄存器 */
    i2c_read(handle, TEST_REG_R, val, sizeof(val));

    /* 打印数据 */
    snprintf(buf, sizeof(buf), "read 0x%02X reg", TEST_REG_R);
    print_hex_value(buf, val, TEST_READ_SIZE);
}

/**************************************************************************************************
 * @brief  : 测试程序
 * @param  : None
 * @return : None
**************************************************************************************************/
int main(int argc, char const *argv[])
{
    i2c_init(&i2cdev_handle);

    while(1)
    {
        i2c_test_read(&i2cdev_handle);
        sleep(1);
    }
    return 0;
}

#endif

/**************************************************************************************************
 * @brief  : 创建i2c
 * @param  : 设备路径
 * @param  : 从机设备地址
 * @param  : 寄存器长度, 单位:字节
 * @return : None
**************************************************************************************************/
static hf_i2c_handle *hf_i2c_init(const char *dev, uint8_t dev_addr, uint8_t reg_len)
{
    /* 创建句柄 */
    hf_i2c_handle *handle = malloc(sizeof(*handle));
    CK_RET(handle == NULL, NULL);

    /* 打开设备 */
    handle->fd = open(dev, O_RDWR);
    CK_GOTO(handle->fd < 0, err_open);

    /* 保存寄存器参数 */
    handle->dev_addr = dev_addr;
    handle->reg_len  = reg_len;

    return handle;

err_open:
    free(handle);
    return NULL;
}

/**************************************************************************************************
 * @brief  : 销毁句柄
 * @param  : 句柄
 * @return : None
**************************************************************************************************/
static void hf_i2c_exit(hf_i2c_handle *handle)
{
    close(handle->fd);
    free(handle);
}

/**************************************************************************************************
 * @brief  : i2c读取
 * @param  : 句柄
 * @param  : 寄存器
 * @param  : 读取值保存位置
 * @param  : 读取长度
 * @return : None
**************************************************************************************************/
static int hf_i2c_read(hf_i2c_handle *handle, uint32_t reg, uint8_t *val, uint8_t len)
{
    struct i2c_msg msg[2];
    struct i2c_rdwr_ioctl_data rid;

    /* 设置要读的寄存器 */
    msg[0].addr  = handle->dev_addr;
    msg[0].flags = 0;
    msg[0].len   = handle->reg_len;
    msg[0].buf   = (uint8_t *)&reg;

    /* 设置数据保存信息 */
    msg[1].addr  = handle->dev_addr;
    msg[1].flags = I2C_M_RD;
    msg[1].len   = len;
    msg[1].buf   = val;

    /* 设置消息数量 */
    rid.msgs  = msg;
    rid.nmsgs = 2;

    return ioctl(handle->fd, I2C_RDWR, &rid);
}

static hf_i2c_ops _hf_i2c_ops = {
    .init = hf_i2c_init,
    .exit = hf_i2c_exit, 
    .read = hf_i2c_read,
};

/**************************************************************************************************
 * @brief  : 获取操作函数
 * @param  : None
 * @return : 0成功, 其他失败
 **************************************************************************************************/
hf_i2c_ops *get_hf_i2c_ops(void)
{
    return &_hf_i2c_ops;
}
