#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "codestd.h"

#define MAP_SZIE                    (1024 * 1024)
#define PHY_ADDR                    0x50000000
 
int main(void) 
{
    int ret;

    /* 打开文件 */
    int fd = open("/dev/xxx", O_RDWR | O_SYNC);
    CI_RET_U(fd, open);

    /* 映射到特定的物理内存 */
    ret = lseek(fd, PHY_ADDR, SEEK_SET);
    CI_RET_U(ret, lseek);

    /* 映射物理内存 */
    void *virt_addr = mmap(NULL, MAP_SZIE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, PHY_ADDR);
    CI_RET(virt_addr == MAP_FAILED, -1, "main mmap fail");
 
    /* 拷贝数据 */
    uint8_t data[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    memcpy(virt_addr, data, sizeof(data));

    while(1) { sleep(1);}
 
    /* 清理映射 */
    munmap(virt_addr, MAP_SZIE);
    close(fd);
    return 0;
}
