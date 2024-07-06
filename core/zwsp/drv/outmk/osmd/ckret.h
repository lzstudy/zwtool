#ifndef __CKERR_H__
#define __CKERR_H__
#include <linux/device.h>


#define PRINT pr_err
#define FILE_NAME(x)            (strrchr(x, '/') ? strrchr(x, '/') + 1 : x)

/* 标准打印 */
#define LOG_I(fmt, ...)                 pr_info(fmt"\n", ##__VA_ARGS__);

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

#endif
