#ifndef __CKERR_H__
#define __CKERR_H__
#include <linux/device.h>
#define PRINT pr_info

/*================================================================== pr_info */
/* 参数判断 - 带返回值 */
#define CI_ARG(cond, val, fmt, ...) \
    do {if(cond) {PRINT(fmt"\n", ##__VA_ARGS__); return val;}} while(0)

#define CK_ARG(cond, val) \
    do {if(cond) {return val;}} while(0)

/* 参数判断 - 不带返回值 */
#define CI_NARG(cond, fmt, ...) \
     do {if(cond) {PRINT(fmt"\n", ##__VA_ARGS__); return;}} while(0)

#define CK_NARG(cond) \
    do {if(cond) {return;}} while(0)

/* 条件判断 - 带返回值 */
#define CI_RET(cond, val, fmt, ...) \
    do {if(cond) {PRINT(fmt"\n", ##__VA_ARGS__); return val;}} while(0)

#define CK_RET(cond, val) \
    do {if(cond) {return val;}} while(0)

/* 条件判断 - 无返回值 */
#define CI_NRET(cond, fmt, ...) \
    do {if(cond) {PRINT(fmt"\n", ##__VA_ARGS__); return;}} while(0)

#define CK_NRET(cond) \
    do {if(cond) {return;}} while(0)

/* 条件判断 - GOTO */
#define CI_GOTO(cond, val, fmt, ...) \
    do {if(cond) {PRINT(fmt"\n", ##__VA_ARGS__); goto val;}} while(0)

#define CK_GOTO(cond, val) \
    do {if(cond) {goto val;}} while(0)

/* 打印信息 */
#define LOG_I(fmt, ...)          PRINT(fmt"\n", ##__VA_ARGS__);

/*================================================================== dev_info */
static struct device *dbgdev;

/* 设置调试设备 */
#define set_ckerr_dev(dev)  (dbgdev = dev)

/* 参数判断 - 带返回值 */
#define DI_ARG(dev, cond, val, fmt, ...) \
    do {if(cond) {dev_info(dev, fmt"\n", ##__VA_ARGS__); return val;}} while(0)

/* 参数判断 - 不带返回值 */
#define DI_NARG(dev, cond, fmt, ...) \
     do {if(cond) {dev_info(dev, fmt"\n", ##__VA_ARGS__); return;}} while(0)

/* 条件判断 - 带返回值 */
#define DI_RET(dev, cond, val, fmt, ...) \
    do {if(cond) {dev_info(dev, fmt"\n", ##__VA_ARGS__); return val;}} while(0)

/* 条件判断 - 无返回值 */
#define DI_NRET(dev, cond, fmt, ...) \
    do {if(cond) {dev_info(dev, fmt"\n", ##__VA_ARGS__); return;}} while(0)

/* 条件判断 - GOTO */
#define DI_GOTO(dev, cond, val, fmt, ...) \
    do {if(cond) {dev_info(dev, fmt"\n", ##__VA_ARGS__); goto val;}} while(0)

#define DS_RET(cond, func) \
    do {if(cond < 0) {dev_err(dbgdev, #func" fail"); return cond;}} while(0)

#define DP_RET(cond, func) \
    do {if(!cond) {dev_err(dbgdev, #func" fail\n"); return -1;}} while(0)

#define LOG_DI(dev, fmt, ...)          dev_info(dev, fmt"\n", ##__VA_ARGS__);


#endif
