#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include "hf_uart.h"
#include "codestd.h"

/**************************************************************************************************
* @brief  : 串口接收
* @param  : None
* @return : None
**************************************************************************************************/
static void *hf_uart_recv(void *arg)
{
    int ret;
    uart_handle *handle = arg;

    for(;;)
    {
        ret = read(handle->fd, handle->buf, sizeof(handle->buf));
        if(ret)
            handle->cb(handle->buf, ret);
    }

    return NULL;
}

/**************************************************************************************************
 * @brief  : 串口初始化
 * @param  : 设备
 * @param  : 回调
 * @return : 0成功, 其他失败
 **************************************************************************************************/
static uart_handle *hf_uart_init(const char *device, uart_cb cb)
{
    CK_ARG(cb == NULL, NULL);

    int ret;
    struct termios new_cfg;

    uart_handle *handle = malloc(sizeof(uart_handle));
    CK_RET(handle == NULL, NULL);

    /* 打开tty设备 */
    handle->fd = open(device, O_RDWR | O_NOCTTY);
    CK_RET(handle->fd < 0, NULL);

    /* 保存旧tty属性 */
    ret = tcgetattr(handle->fd, &new_cfg);
    CK_GOTO(ret < 0, err_uart);

    /* 设置原始模式 */
    cfmakeraw(&new_cfg);

    /* 使能接收 */
    new_cfg.c_cflag |= CREAD;

    /* 设置波特率 */
    ret = cfsetspeed(&new_cfg, B115200);
    CK_RET(handle->fd < 0, NULL);

    /* 设置数据位 */
    new_cfg.c_cflag &= ~CSIZE;
    new_cfg.c_cflag |= CS8;

    /* 设置奇偶校验位 */
    new_cfg.c_cflag &= ~PARENB;
    new_cfg.c_iflag &= ~INPCK;

    /* 设置停止位 */
    new_cfg.c_cflag &= ~CSTOPB;

    /* 将MIN和TIME设置为0 */
    new_cfg.c_cc[VTIME] = 0;
    new_cfg.c_cc[VMIN] = 0;

    /* 清空缓冲区 */
    ret = tcflush(handle->fd, TCIOFLUSH);
    CK_GOTO(ret < 0, err_uart);

    /* 写入配置 */
    ret = tcsetattr(handle->fd, TCSANOW, &new_cfg);
    CK_GOTO(ret < 0, err_uart);

    /* 保存回调 */
    handle->cb = cb;

    /* 配置串口接收线程 */
    ret = pthread_create(&handle->pid, NULL, hf_uart_recv, handle);
    CK_GOTO(ret < 0, err_uart);

    /* 分离线程, 返回句柄 */
    pthread_detach(handle->pid);
    return handle;

err_uart:
    close(handle->fd);
    return NULL;
}

/**************************************************************************************************
 * @brief  : 串口发送
 * @param  : 串口句柄
 * @param  : 要发送的数据
 * @param  : 数据长度
 * @return : 0成功, 其他失败
 **************************************************************************************************/
static int hf_uart_send(uart_handle *handle, void *data, uint16_t datalen)
{
    return write(handle->fd, data, datalen);
}

static hf_uart_ops _hf_uart_ops = {
    .init = hf_uart_init,
    .send = hf_uart_send,
};

/**************************************************************************************************
 * @brief  : 获取引脚操作函数
 * @param  : None
 * @return : 0成功, 其他失败
 **************************************************************************************************/
hf_uart_ops *get_hf_uart_ops(void)
{
    return &_hf_uart_ops;
}

