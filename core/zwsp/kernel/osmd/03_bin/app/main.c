#include <stdint.h>
#include "test.h"

int add_a = 2;
int add_b = 4;

int zw_1;
int zw_2;
int zw_3;

int main(void)
{
    write_mem(2);
    write_mem(add_a + add_b);
    return 0;
}
