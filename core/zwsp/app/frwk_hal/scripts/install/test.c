#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "codestd.h"

/* 所有的测试例子 */
#undef TEST_LED
#undef TEST_PWM
#undef TEST_UART

/* 设置要定义的测试 */
#define TEST_PWM


/*----------------------------------- 01 LED 测试  ---------------------------------*/
#ifdef TEST_LED
#include "hf_led.h"
/**************************************************************************************************
 * @brief  : 程序入口
 * @param  : None
 * @return : None
**************************************************************************************************/
int main(int argc, char const *argv[])
{
    for(;;)
    {
        get_hf_led_ops()->set("sys-led", 1);
        sleep(1);
        get_hf_led_ops()->set("sys-led", 0);
        sleep(1);
    }
}
#endif

/*----------------------------------- 2 PWM 测试  ---------------------------------*/
#ifdef TEST_PWM
#include "hf_pwm.h"
/**************************************************************************************************
 * @brief  : 程序入口
 * @param  : None
 * @return : None
**************************************************************************************************/
int main(int argc, char const *argv[])
{
    /* 导出 */
    get_hf_pwm_ops()->export(1, 0);

    /* 设置参数 */
    get_hf_pwm_ops()->set(1, 0, 10000, 5000);

    return 0;
}
#endif

/*----------------------------------- 3 UART 测试  ---------------------------------*/
#ifdef TEST_UART
#include "hf_uart.h"
/**************************************************************************************************
 * @brief  : 串口中断
 * @param  : 数据
 * @param  : 数据长度
 * @return : None
**************************************************************************************************/
static void uart_isr(uint8_t *data, uint16_t datalen)
{
    LOG_I("data = %s, datalen = %d", data, datalen);
}

/**************************************************************************************************
 * @brief  : 程序入口
 * @param  : None
 * @return : None
**************************************************************************************************/
int main(int argc, char const *argv[])
{
    int ret;
    uart_handle *handle;

    /* 初始化串口 */
    handle = get_hf_uart_ops()->init("/dev/ttymxc0", uart_isr);
    CK_RET(handle == NULL, -1);

    /* 写入数据 */
    ret = get_hf_uart_ops()->send(handle, "hello test\n", 15);
    LOG_I("ret = %d", ret);

    sleep(3);
    LOG_I("This is end");

    while(1)
    {
        sleep(1);
    }

    return 0;
}
#endif

