#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include "codestd.h"

/**************************************************************************************************
 * @brief  : 打印hex数据
 * @param  : 提示字符
 * @param  : 数据
 * @param  : 数据长度
 * @return : 串口句柄, NULL - 失败
 **************************************************************************************************/
void print_hex_value(const char *prompt, void *data, int datalen)
{
    uint8_t *val = data;

    printf("%s(%d bytes): ", prompt, datalen);
    for(uint8_t i = 0 ; i < datalen ; i++)
    {
        printf("0x%02x ", val[i]);
    }
    printf("\n");
}

/**************************************************************************************************
 * @brief  : 断点
 * @param  : None
 * @return : None
 **************************************************************************************************/
void *break_line(const char *promt)
{
    char buf[5];
    printf("============= break line [%s] =============\n", promt);
    return fgets(buf, sizeof(buf), stdin);
}

void *break_line_stop(void)
{
    char buf[5];
    printf("--------------- STOP --------------------\n");
    return fgets(buf, sizeof(buf), stdin);
}
