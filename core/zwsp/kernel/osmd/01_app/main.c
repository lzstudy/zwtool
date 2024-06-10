#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "codestd.h"

#define MAP_SZIE                    (1024 * 1024)


/**************************************************************************************************
 * @brief  : 属性 - r - dgb3
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static int load_bin(void *virt, const char *file)
{
    FILE *fp;
    long file_size;

    /* 打开文件 */
    fp = fopen(file, "r");
    CI_RET_P(fp, fopen);

    /* 获取文件大小 */
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    LOG_I("file [%s] size = %ld", file, file_size);

    /* 读取文件 */
    fread(virt, sizeof(uint8_t), file_size, fp);
    fclose(fp);
    return 0;
}

/**************************************************************************************************
 * @brief  : 主函数
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
int main(int argc, char const *argv[])
{
    int ret;
    void *virt_addr;
    uint32_t phy_addr;

    if(argc < 3) {
        LOG_I("usage : ./main <bin_file> <phy_addr>");
        return 0;
    }
    /* 物理地址 */
    phy_addr = strtoul(argv[2], NULL, 0);

    /* 打开文件 */
    int fd = open("/dev/osmd", O_RDWR | O_SYNC);
    CI_RET_U(fd, open);

    /* 映射到特定的物理内存 */
    ret = lseek(fd, phy_addr, SEEK_SET);
    CI_RET_U(ret, lseek);

    /* 映射物理内存 */
    virt_addr = mmap(NULL, MAP_SZIE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, phy_addr);
    CI_RET(virt_addr == MAP_FAILED, -1, "main mmap fail");
 
    // uint8_t data[8] = {0x01, 0x01, 0x01, 0x01, 0x05, 0x06, 0x07, 0x08};
    // memcpy(virt_addr, data, sizeof(data));
    // printf("Memory mapped to %p\n", virt_addr);

    /* 拷贝数据 */
    load_bin(virt_addr, argv[1]);
 
    /* 清理映射 */
    munmap(virt_addr, MAP_SZIE);
    close(fd);
    LOG_I("load file success. ");
    return 0;
}
