#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "codestd.h"

#define FORMAT_CELL                 4                   // 2/4
#define FORMAT_LINE                 32                  // 16/32
/**************************************************************************************************
 * @brief  : 属性 - r - dgb3
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static int dump_mem(void *virt_addr, uint32_t dump_addr, uint32_t dump_size)
{
    void *addr;
	uint8_t count = 0;
	uint32_t phyaddr, val;
    
    phyaddr = dump_addr;

	LOG_I("");
	
	for(addr = virt_addr ; addr <= virt_addr + dump_size; addr += FORMAT_CELL, phyaddr += FORMAT_CELL)
	{
		/* 对齐计数为0, 则打印 */
		if(count == 0)
			printf("%08x : ", phyaddr);

        val = *(uint32_t *)addr;

		/* 打印元素 */
		printf("%08x ", val);

		/* 到了换行位置 */
		if(++count == FORMAT_LINE / FORMAT_CELL) {
			LOG_I("");
			count = 0;
		}
	}

	LOG_I("");
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
    uint32_t dump_addr, dump_size;

    if(argc < 3) {
        LOG_I("usage : ./xxmem <dump_addr> <dump_size>");
        return 0;
    }

    /* 物理地址 */
    dump_addr = strtoul(argv[1], NULL, 0);
    dump_size = strtoul(argv[2], NULL, 0);

    /* 打开文件 */
    int fd = open("/dev/osmd", O_RDWR | O_SYNC);
    CI_RET_U(fd, open);

    /* 映射到特定的物理内存 */
    ret = lseek(fd, dump_addr, SEEK_SET);
    CI_RET_U(ret, lseek);

    /* 映射物理内存 */
    virt_addr = mmap(NULL, dump_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, dump_addr);
    CI_RET(virt_addr == MAP_FAILED, -1, "main mmap fail");
 
    /* 拷贝数据 */
    dump_mem(virt_addr, dump_addr, dump_size);
 
    /* 清理映射 */
    munmap(virt_addr, dump_size);
    close(fd);
    return 0;
}
