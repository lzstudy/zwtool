#include <stdio.h>
#include "cmpt_init.h"

static int init_start(void){return 0;}
INIT_EXPORT(init_start, "0");

static int init_end(void){return 0;}
INIT_EXPORT(init_end, "6.end");

/**************************************************************************************************
 * @brief  : 组件初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
void cmpt_init(void)
{
    const init_fn_t *fn_ptr;

    for (fn_ptr = &__cmpt_init_init_start ; fn_ptr < &__cmpt_init_init_end ; fn_ptr++)
    {
        (*fn_ptr)();
    }
}

