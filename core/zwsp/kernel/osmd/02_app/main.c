#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "codestd.h"

#define PATH_DEV                "/dev/osmd"                                     /**< 混和设备文件 */
#define MMAP_SZIE               1024                                            /**< 映射区域大小 */


/**************************************************************************************************
 * @brief  : 将文件拷贝到指定地址
 * @param  : None
 * @return : None
**************************************************************************************************/
static int load_bin(const char *file, uint32_t addr)
{
    int fd;
    uint8_t *p;

    LOG_I("pid = %d\n", getpid());

    printf("aafile = %s, addr = 0x%08x\n", file, addr);

    /* 打开文件 */
    fd = open("/dev/osmd", O_RDWR);
    CI_RET(fd < 0, fd, "open /dev/osmd fail (%d)", fd);

    /* 地址映射 */
    p = mmap(NULL, MMAP_SZIE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    CI_RET(p == MAP_FAILED, -1, "load mmap fail");

    sleep(1);
    LOG_I(" app addr = %p", p);


    close(fd);
    return 0;
}


/**************************************************************************************************
 * @brief  : 测试程序
 * @param  : None
 * @return : None
**************************************************************************************************/
int main(int argc, char const *argv[])
{
    load_bin(argv[1], strtoul(argv[2], NULL, 0));

    while(1)
    {
        sleep(1);
    }
    return 0;
}

