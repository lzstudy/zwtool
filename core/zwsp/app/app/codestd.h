#ifndef __CODESTD_H__
#define __CODESTD_H__
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define PRINT printf
#define FILE_NAME(x)            (strrchr(x, '/') ? strrchr(x, '/') + 1 : x)

/* 标准打印 */
#define LOG_I(fmt, ...)                 PRINT(fmt"\n", ##__VA_ARGS__);

/* 提示信息 */
#define CI_RET(func, cond, ret)                                                                                          \
    if(cond) {                                                                                                           \
        PRINT("[%s:%s:%u - %s] [fail:%s] [ret:%d] \n", FILE_NAME(__FILE__), __FUNCTION__, __LINE__, #func, #cond, ret);  \
        return ret;                                                                                                      \
    }

#define _CI_RET(func, cond)                                                                               \
    if(cond) {                                                                                            \
        PRINT("[%s:%s:%u - %s] [fail:%s]\n", FILE_NAME(__FILE__), __FUNCTION__, __LINE__, #func, #cond);  \
        return;                                                                                           \
    }

#define CI_GOTO(func, cond, label)                                                                        \
    if(cond) {                                                                                            \
        PRINT("[%s:%s:%u - %s] [fail:%s]\n", FILE_NAME(__FILE__), __FUNCTION__, __LINE__, #func, #cond);  \
        goto label;                                                                                       \
    }

/* 无提示信息 */
#define CK_RET(cond, ret)               do {if(ret) {return val;}} while(0)
#define _CK_RET(cond)                   do {if(cond) {return;}} while(0)
#define CK_GOTO(cond, lable)            do {if(cond) {goto label;}} while(0)

/* 检测 - 参数 */
#define CI_ARG  CI_RET
#define CK_ARG  CK_RET

/* 32位大小端转换 */
#define BLC32(x)                ((((x) & 0xff) << 24) | (((x) & 0xff00) << 8) \
                                    | (((x) & 0xff0000) >> 8) | (((x) >> 24) & 0xff))

/* 16位大小端转换 */
#define BLC16(x)                ((((x) & 0xff) << 8) | (((x) >> 8) & 0xff))

/* 计算数组程成员数量 */
#define ARRAY_SIZE(arr)         (sizeof(arr) / sizeof((arr)[0]))

/* 常用接口 */
void print_hex_value(const char *prompt, void *data, int datalen);              /**# 按16进制打印. */

/* 调试接口 */
void *break_line(const char *promt);                                            /**# 断点. */
void *break_line_stop(void);                                                    /**# 停止. */
#endif


