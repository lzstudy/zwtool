#ifndef __CODESTD_H__
#define __CODESTD_H__
#include <stdio.h>
#include <stdint.h>

/* 参数判断 - 带返回值 */
#define CI_ARG(cond, val, fmt, ...) \
    do {if(cond) {printf(fmt"\n", ##__VA_ARGS__); return val;}} while(0)

#define CK_ARG(cond, val) \
    do {if(cond) {return val;}} while(0)

/* 参数判断 - 不带返回值 */
#define CI_NARG(cond, fmt, ...) \
     do {if(cond) {printf(fmt"\n", ##__VA_ARGS__); return;}} while(0)

#define CK_NARG(cond) \
    do {if(cond) {return;}} while(0)

/* 条件判断 - 带返回值 */
#define CI_RET(cond, val, fmt, ...) \
    do {if(cond) {printf(fmt"\n", ##__VA_ARGS__); return val;}} while(0)

#define CK_RET(cond, val) \
    do {if(cond) {return val;}} while(0)

/* 条件判断 - 无返回值 */
#define CI_NRET(cond, fmt, ...) \
    do {if(cond) {printf(fmt"\n", ##__VA_ARGS__); return;}} while(0)

#define CK_NRET(cond) \
    do {if(cond) {return;}} while(0)

/* 条件判断 - GOTO */
#define CI_GOTO(cond, val, fmt, ...) \
    do {if(cond) {printf(fmt"\n", ##__VA_ARGS__); goto val;}} while(0)

#define CK_GOTO(cond, val) \
    do {if(cond) {goto val;}} while(0)


/* 打印信息 */
#define LOG_I(fmt, ...)          printf(fmt"\n", ##__VA_ARGS__);

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
