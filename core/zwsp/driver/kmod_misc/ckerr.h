#ifndef __CKERR_H__
#define __CKERR_H__
#include <linux/device.h>
#define PRINT pr_err

/* 标准打印 */
#define LOG_I(fmt, ...)          pr_info(fmt"\n", ##__VA_ARGS__);

/* 判断返回值 - return */
#define CI_RET(cond, ret, fmt, ...) \
    do {if(cond) {PRINT(fmt"\n", ##__VA_ARGS__); return ret;}} while(0)

#define CK_RET(cond, ret) \
    do {if(ret) {return ret;}} while(0)

#define CK_RET_P(ret)               CK_RET(!ret, -1);
#define CI_RET_P(ret, fmt)          CI_RET(!ret, -1, #fmt " fail");
#define CK_RET_U(ret)               CK_RET(ret < 0, ret);  
#define CI_RET_U(ret, fmt)          CI_RET(ret < 0, ret, #fmt " fail (%d)", ret);


/* 判断返回值 - no return */
#define _CI_RET(cond, fmt, ...) \
    do {if(cond) {PRINT(fmt"\n", ##__VA_ARGS__); return;}} while(0)

#define _CK_RET(cond) \
    do {if(cond) {return;}} while(0)

#define _CK_RET_P(ret)              _CK_RET(!ret);
#define _CI_RET_P(ret, fmt)         _CI_RET(!ret, #fmt " fail");
#define _CK_RET_U(ret)              _CK_RET(ret < 0);  
#define _CI_RET_U(ret, fmt)         _CI_RET(ret < 0, #fmt " fail (%d)", ret);

/* 条件判断 - GOTO */
#define CI_GOTO(cond, lable, fmt, ...) \
    do {if(cond) {PRINT(fmt"\n", ##__VA_ARGS__); goto lable;}} while(0)

#define CK_GOTO(cond, lable) \
    do {if(cond) {goto lable;}} while(0)

#define CI_GOTO_P(ret, label, fmt)  CI_GOTO(!ret, lable, #fmt " fail");
#define CK_GOTO_P(ret, label)       CK_GOTO(!ret, lable);
#define CI_GOTO_U(ret, label, fmt)  CI_GOTO(ret < 0, lable, #fmt " fail (%d)", ret);
#define CK_GOTO_U(ret, label)       CK_GOTO(ret < 0, lable);

/* 判断参数 */
#define CI_ARG  CI_RET
#define CK_ARG  CK_RET


#endif
