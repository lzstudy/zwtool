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

#define MASK_ALIGN_4K               0xFFFFF000          /**< 4K对齐掩码 */
#define SIZE_4K                     0x1000              /**< 4K大小 */

typedef enum _line_fmt{
    LINE_16 = 16,
    LINE_32 = 32,
}line_fmt;

typedef enum _cell_fmt {
    CELL_1 = 1,
    CELL_2 = 2,
    CELL_4 = 4,
}cell_fmt;

typedef struct _format_info {
    uint8_t cell;                                       /**@ 打印单位 1/2/4 */
    uint8_t line;                                       /**@ 一行默认数量 16/32 */
}format_info;

typedef struct _rw_info {
    uint8_t is_read;                                    /**@ 1读/0写 */
    void *vaddr;                                        /**@ 虚拟起始地址 - 非4K对齐 */
    uint32_t paddr;                                     /**@ 要读写的物理地址 - 真实地址 */
    uint32_t size;                                      /**@ 要读写的大小 = (传入) */
    char val[16];                                       /**@ 要写入的值 */
}rw_info;

typedef struct _mmap_info {
    int fd;                                             /**@ 文件描述符 */
    void *vaddr;                                        /**@ 映射的虚拟地址 - 4K对齐后的地址 */
    uint32_t paddr;                                     /**@ 映射的物理地址 - 4K对齐后的地址 */
    uint32_t size;                                      /**@ 映射大小 = 偏移 + 传入 */
    uint32_t offs;                                      /**@ 传入偏移 */
}mmap_info;


typedef struct _xxmem_cxt {
    format_info fmt;                                    /**@ 格式化信息 */
    rw_info rw;                                         /**@ 读写信息 */
    mmap_info mmap;                                     /**@ mmap信息 */
}xxmem_cxt;

/**************************************************************************************************
 * @brief  : 读取内存
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static void print_mem_cell(void *addr, uint8_t width)
{
	uint8_t  val8;
	uint16_t val16;
	uint32_t val32;

	switch(width)
	{
	case 1:
        val8 = *(uint8_t *)addr;
		printf("%02x ", val8);
		break;

	case 2:
		val16 = *(uint16_t *)addr;
		printf("%04x ", val16);
		break;

	case 4:
		val32 = *(uint32_t *)addr;
		printf("%08x ", val32);
		break;
	}
}

/**************************************************************************************************
 * @brief  : 属性 - r - dgb3
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static int mem_dump(void *virt_addr, uint32_t dump_addr, uint32_t dump_size, uint8_t cell, uint8_t line)
{
    void *addr;
	uint8_t count = 0;
	uint32_t phyaddr;
    
    phyaddr = dump_addr;

	LOG_I("");
	
	for(addr = virt_addr ; addr <= virt_addr + dump_size; addr += cell, phyaddr += cell)
	{
		/* 对齐计数为0, 则打印 */
		if(count == 0)
			printf("%08x : ", phyaddr);

		/* 打印元素 */
        print_mem_cell(addr, cell);

		/* 到了换行位置 */
		if(++count == line / cell) {
			LOG_I("");
			count = 0;
		}
	}

	LOG_I("");
    return 0;
}

/**************************************************************************************************
 * @brief  : 读内存
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static void mem_read(void *virt_addr, uint32_t addr, uint32_t size)
{
    uint8_t v8;
    uint16_t v16;
    uint32_t v32;

    switch (size)
    {
    case 1:
        v8 = *(uint8_t *)virt_addr;
        LOG_I("read 0x%x val 0x%x(%d)", addr, v8, v8);
        break;

    case 2:
        v16 = *(uint16_t *)virt_addr;
        LOG_I("read 0x%x val 0x%04x(%d)", addr, v16, v16);
        break;

    case 4:
        v32 = *(uint32_t *)virt_addr;
        LOG_I("read 0x%x val 0x%08x(%d)", addr, v32, v32);
        break;
    
    default:
        mem_dump(virt_addr, addr, size, 1, 16);
        break;
    }
}

/**************************************************************************************************
 * @brief  : 读内存
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static void xxmem_read(xxmem_cxt *cxt)
{
    format_info *fmt = &cxt->fmt;
    rw_info *rw = &cxt->rw;

    if(rw->size <= 4)
        mem_read(rw->vaddr, rw->paddr, rw->size);
    else
        mem_dump(rw->vaddr, rw->paddr, rw->size, fmt->cell, fmt->line);
}


/**************************************************************************************************
 * @brief  : 打印用法
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static void print_usage(void)
{
    LOG_I("usage r : ./xxmem <addr> <size>");
    LOG_I("usage r : ./xxmem <addr> <size> -c <1/2/4> -l <16/32>");
    LOG_I("usage w : ./xxmem <addr> <size> <val>");
    exit(0);
}

/**************************************************************************************************
 * @brief  : 设置格式信息
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static void xxmem_format_init(format_info *fmt, int argc, char const *argv[])
{
    /* 如果是4个参数则为读 */
    if(argc == 4)
        return;

    if(argc == 3)
    {
        fmt->cell = CELL_4;
        fmt->line = LINE_32;
        return;
    }

    fmt->cell = strtoul(argv[4], NULL, 0);
    fmt->line = strtoul(argv[6], NULL, 0);
}

/**************************************************************************************************
 * @brief  : xxmem映射
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static int xxmem_mmap(xxmem_cxt *cxt)
{
    int ret;
    rw_info *rw = &cxt->rw;
    mmap_info *map = &cxt->mmap;

    /* 打开文件 */
    map->fd = open("/dev/osmd", O_RDWR | O_SYNC);
    CI_RET_U(map->fd, open);

    /* 设置偏移 */
    ret = lseek(map->fd, rw->paddr, SEEK_SET);
    CI_RET_U(ret, lseek);

    /* 内存映射 */
    map->vaddr = mmap(NULL, map->size, PROT_READ | PROT_WRITE, MAP_SHARED, map->fd, map->paddr);
    CI_RET(map->vaddr == MAP_FAILED, -1, "main mmap fail");

    /* 设置虚拟地址 */
    rw->vaddr = map->vaddr + map->offs;
    return 0;
}

/**************************************************************************************************
 * @brief  : 设置上下文
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static void xxmem_init(xxmem_cxt *cxt, int argc, char const *argv[])
{
    rw_info *rw = &cxt->rw;
    format_info *fmt = &cxt->fmt;
    mmap_info *mmap = &cxt->mmap;

    /* 设置读写信息 */
    rw->paddr   = strtoul(argv[1], NULL, 0);
    rw->is_read = (argc == 4) ? 0 : 1;
    rw->size    = (argc == 2) ? 1 : strtoul(argv[2], NULL, 0);

    /* 如果4个参数, 保存第4个*/
    if(argc == 4) {
        rw->is_read = 0;
        strncpy(rw->val, argv[3], sizeof(rw->val)); }
    else
        rw->is_read = 1;

    /* 判断size */
    if(rw->size == 0)
        print_usage();

    /* 设置映射信息, mmap需要4K对齐, 否则映射不成功 */
    mmap->paddr = rw->paddr & MASK_ALIGN_4K;
    mmap->offs = rw->paddr % SIZE_4K;
    mmap->size = rw->size + mmap->offs;

    /* 设置格式信息 */
    xxmem_format_init(fmt, argc, argv);
    xxmem_mmap(cxt);
}

/**************************************************************************************************
 * @brief  : 写内存
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static void mem_write_brust(uint32_t paddr, void *vaddr, uint32_t size, char *val)
{
    uint8_t value = strtol(val, NULL, 0);
    memset(vaddr, value, size);

    LOG_I("memset [0x%x - 0x%x] val 0x%02x(%d)", paddr, paddr + size, value, value);
}

/**************************************************************************************************
 * @brief  : 写内存
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static void mem_write(uint32_t paddr, void *vaddr, uint8_t size, char *val)
{
    uint8_t v8;
    uint16_t v16;
    uint32_t v32;

    switch(size)
    {
    case 1:
        v8 = strtol(val, NULL, 0);
        *(uint8_t *)vaddr = v8;
        LOG_I("write [0x%x] val 0x%02x(%d)", paddr, v8, v8);
        break;

    case 2:
        v16 = strtol(val, NULL, 0);
        *(uint16_t *)vaddr = v16;
        LOG_I("write [0x%x] val 0x%04x(%d)", paddr, v16, v16);
        break;

    case 4:
        v32 = strtoul(val, NULL, 0);
        *(uint32_t *)vaddr = v32;
        LOG_I("write [0x%x] val 0x%x(%d)", paddr, v32, v32);
        break;

    default:
        mem_write_brust(paddr, vaddr, size, val);
        break;
    }
}

/**************************************************************************************************
 * @brief  : 写内存
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static void xxmem_write(xxmem_cxt *cxt)
{
    rw_info *rw = &cxt->rw;

    if(rw->size <= 4)
        mem_write(rw->paddr, rw->vaddr, rw->size, rw->val);
    else
        mem_write_brust(rw->paddr, rw->vaddr, rw->size, rw->val);
}

/**************************************************************************************************
 * @brief  : 运行
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static void xxmem_run(xxmem_cxt *cxt)
{
    if(cxt->rw.is_read)
        xxmem_read(cxt);
    else
        xxmem_write(cxt);
}

/**************************************************************************************************
 * @brief  : 关闭映射
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static void xxmem_exit(xxmem_cxt *cxt)
{
    mmap_info *mmap = &cxt->mmap;

    munmap(mmap->vaddr, mmap->size);
    close(mmap->fd);
}

/**************************************************************************************************
 * @brief  : 主函数
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
int main(int argc, char const *argv[])
{
    xxmem_cxt cxt = {0};

    if(argc <= 1)
        print_usage();

    xxmem_init(&cxt, argc, argv);
    xxmem_run(&cxt);
    xxmem_exit(&cxt);
    return 0;
}
