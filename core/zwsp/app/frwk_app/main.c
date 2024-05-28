#include <stdio.h>
#include "common.h"

extern void cmpt_init(void);

int main(int argc, char const *argv[])
{
    cmpt_init();
    return 0;
}

static int my_init1(void)
{
    LOG_I("%s", __func__);
    return 0;
}
INIT_EXPORT_LEVEL1(my_init1);

static int my_init3(void)
{
    LOG_I("%s", __func__);
    return 0;
}
INIT_EXPORT_LEVEL3(my_init3);

static int my_init2(void)
{
    LOG_I("%s", __func__);
    return 0;
}
INIT_EXPORT_LEVEL2(my_init2);

