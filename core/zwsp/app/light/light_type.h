#ifndef __LIGHT_TYPE_H__
#define __LIGHT_TYPE_H__
__BEGIN_DECLS

typedef enum _light_id {
    /* pwm light */
    LIGHT_M_CAM_WHITE,                                                              /**< 00' 主摄白光灯. */
    LIGHT_M_CAM_IR,                                                                 /**< 01' 主摄红外灯. */
    LIGHT_S_CAM_IR,                                                                 /**< 02' 辅摄红外灯. */

    /* gpio light */
    LIGHT_TEST,                                                                     /**< 03' 测试. */
}light_id;


__END_DECLS
#endif
