#ifndef __HF_UART_H__
#define __HF_UART_H__
__BEGIN_DECLS
#include <stdint.h>
#include <pthread.h>

typedef void (*uart_cb)(uint8_t *data, uint16_t datalen);                       /**# 串口中断 */

typedef struct _uart_handle {
    int fd;                                                                     /**@ 串口描述符. */
    pthread_t pid;                                                              /**@ 接收线程ID */
    uart_cb cb;                                                                 /**@ 回调. */
    uint8_t buf[64];                                                            /**@ 接受数据缓存 */
}uart_handle;

typedef struct _hf_uart_ops {
    uart_handle *(*init)(const char *device, uart_cb cb);                       /**@ 初始化串口 */
    int (*send)(uart_handle *handle, void *data, uint16_t datalen);             /**@ 发送数据 */
}hf_uart_ops;

hf_uart_ops *get_hf_uart_ops(void);                                             /**# 获取PIN操作函数. */

__END_DECLS
#endif
