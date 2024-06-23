#include <stdint.h>
#include "test.h"

void write_mem(int val)
{
    uint8_t *p = (uint8_t *)0x50000000;
    *p = val;
}