#ifndef __CODESTD_H__
#define __CODESTD_H__
#include <stdio.h>
#include <stdint.h>

#define PRINT printf

/* 标准打印 */
#define LOG_I(fmt, ...)          PRINT(fmt"\n", ##__VA_ARGS__);

/* 判断返回值 - return */
#define CI_RET(cond, ret, fmt, ...) \
    do {if(cond) {printf(fmt"\n", ##__VA_ARGS__); return ret;}} while(0)

#define CK_RET(cond, ret) \
    do {if(ret) {return val;}} while(0)

#define CK_RET_P(ret)               CK_RET(!ret, -1);
#define CI_RET_P(ret, fmt)          CI_RET(!ret, -1, #fmt " fail");
#define CK_RET_U(ret)               CK_RET(ret < 0, ret);  
#define CI_RET_U(ret, fmt)          CI_RET(ret < 0, ret, #fmt " fail (%d)", ret);


/* 判断返回值 - no return */
#define _CI_RET(cond, fmt, ...) \
    do {if(cond) {printf(fmt"\n", ##__VA_ARGS__); return;}} while(0)

#define _CK_RET(cond) \
    do {if(cond) {return;}} while(0)

#define _CK_RET_P(ret)              _CK_RET(!ret);
#define _CI_RET_P(ret, fmt)         _CI_RET(!ret, #fmt " fail");
#define _CK_RET_U(ret)              _CK_RET(ret < 0);  
#define _CI_RET_U(ret, fmt)         _CI_RET(ret < 0, #fmt " fail (%d)", ret);

/* 条件判断 - GOTO */
#define CI_GOTO(cond, lable, fmt, ...) \
    do {if(cond) {printf(fmt"\n", ##__VA_ARGS__); goto lable;}} while(0)

#define CK_GOTO(cond, lable) \
    do {if(cond) {goto lable;}} while(0)

#define CI_GOTO_P(ret, label, fmt)  CI_GOTO(!ret, lable, #fmt " fail");
#define CK_GOTO_P(ret, label)       CK_GOTO(!ret, lable);
#define CI_GOTO_U(ret, label, fmt)  CI_GOTO(ret < 0, lable, #fmt " fail (%d)", ret);
#define CK_GOTO_U(ret, label)       CK_GOTO(ret < 0, lable);

/* 判断参数 */
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


