/*
 * Copyright (c) 2022-2022 Huawei Technologies Co., Ltd. All rights reserved.
 *
 * UniProton is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 * Create: 2022-12-11
 * Description: mmu
 */
#include "prt_base.h"
#include "mmu.h"
#include "cache_asm.h"
#include "prt_sys.h"
#include "prt_task.h"
#include "cpu_config.h"
#include "securec.h"

extern U64 g_mmu_page_begin;
extern U64 g_mmu_page_end;

static mmu_mmap_region_s g_mem_map_info_each[] = {
    {// openamp共享内存基址
        .virt  = MMU_SHM_BASE_ADDR,
        .phys  = MMU_SHM_BASE_ADDR,
        .size  = 0x100000,
        .max_level = 0x2,
        .attrs = MMU_ATTR_DEVICE_NGNRNE | MMU_ACCESS_RWX,
    }, {// ck镜像地址
        .virt  = MMU_IMAGE_BASE_ADDR,
        .phys  = MMU_IMAGE_BASE_ADDR,
        .size  = 0x1000000,
        .max_level = 0x2,
        .attrs = MMU_ATTR_CACHE_SHARE | MMU_ACCESS_RWX,
    }, {// gic寄存器地址
        .virt  = MMU_GIC_ADDR,
        .phys  = MMU_GIC_ADDR,
        .size  = 0x1000000,
        .max_level = 0x2,
        .attrs = MMU_ATTR_DEVICE_NGNRNE | MMU_ACCESS_RWX,
    }, {// UART地址
        .virt  = MMU_UART_ADDR,
        .phys  = MMU_UART_ADDR,
        .size  =  0x2000000,
        .max_level = 0x2,
        .attrs = MMU_ATTR_DEVICE_NGNRNE | MMU_ACCESS_RWX,
    }, {// dtb地址
        .virt  = MMU_DTB_ADDR,
        .phys  = MMU_DTB_ADDR,
        .size  = 0xd000,
        .max_level = 0x2,
        .attrs = MMU_ATTR_UNCACHE_SHARE | MMU_ACCESS_RWX,
    }, {// 打桩地址
        .virt  = 0x7e000000,
        .phys  = 0x7e000000,
        .size  = 0x1000,
        .max_level = 0x2,
        .attrs = MMU_ATTR_UNCACHE_SHARE | MMU_ACCESS_RWX,
    }
};

/*
 *  With 4k page granule, a virtual address is split into 4 lookup parts
 *  spanning 9 bits each
 *
 *    _______________________________________________
 *   |       |       |       |       |       |       |
 *   |   0   |  Lv0  |  Lv1  |  Lv2  |  Lv3  |  off  |
 *   |_______|_______|_______|_______|_______|_______|
 *     63-48   47-39   38-30   29-21   20-12   11-00
 *
 *             mask        page size
 *
 *    Lv0: FF8000000000       --
 *    Lv1:   7FC0000000       1G
 *    Lv2:     3FE00000       2M
 *    Lv3:       1FF000       4K
 *    off:          FFF
 *
 *    With 64k page granule, a virtual address is split into 4 lookup parts
 *    spanning 9 bits each
 *
 *     _______________________________________________
 *    |       |       |       |       |       |       |
 *    |   0   |   0   |  Lv1  |  Lv2  |  Lv3  |  off  |
 *    |_______|_______|_______|_______|_______|_______|
 *      63-48   63-48   47-42   41-29   28-16   15-00
 *
 *              mask        page size
 *
 *    Lv1: FC0000000000       --
 *    Lv2:  3FFE0000000       512M
 *    Lv3:     1FFF0000       64K
 *    off:         FFFF
 */
static U64 mmu_get_tcr(U32 *pips, U32 *pva_bits, U32 granule, mmu_mmap_region_s *mem_map, U32 mem_region_num)
{
    U64 max_addr = 0;
    U64 ips, va_bits;
    U64 tcr;
    U32 i;

    /* Find the largest address we need to support */
    for (i = 0; i < mem_region_num; i++) {
        max_addr = MAX(max_addr, mem_map[i].virt + mem_map[i].size);
    }

    /* Calculate the maximum physical (and thus virtual) address */
    if (max_addr > (1ULL << MMU_BITS_44)) {
        /* Physical Address Size, 5: 48 bits, 256TB. */
        ips = MMU_PHY_ADDR_LEVEL_5;
        va_bits = MMU_BITS_48;
    } else if (max_addr > (1ULL << MMU_BITS_42)) {
        /* Physical Address Size, 4: 44 bits, 16TB. */
        ips = MMU_PHY_ADDR_LEVEL_4;
        va_bits = MMU_BITS_44;
    } else if (max_addr > (1ULL << MMU_BITS_40)) {
        /* Physical Address Size, 3: 42 bits, 4TB. */
        ips = MMU_PHY_ADDR_LEVEL_3;
        va_bits = MMU_BITS_42;
    } else if (max_addr > (1ULL << MMU_BITS_36)) {
        /* Physical Address Size, 2: 40 bits, 1TB. */
        ips = MMU_PHY_ADDR_LEVEL_2;
        va_bits = MMU_BITS_40;
    } else if (max_addr > (1ULL << MMU_BITS_32)) {
        /* Physical Address Size, 1: 36 bits, 64GB. */
        ips = MMU_PHY_ADDR_LEVEL_1;
        va_bits = MMU_BITS_36;
    } else {
        /* Physical Address Size, 0: 32 bits, 4GB. */
        ips = MMU_PHY_ADDR_LEVEL_0;
        va_bits = MMU_BITS_32;
    }

    tcr = TCR_EL1_RSVD | TCR_IPS(ips);

    if (granule == MMU_GRANULE_4K) {
        /* PTWs cacheable, inner/outer WBWA and inner shareable */
        tcr |= TCR_TG0_4K | TCR_SHARED_INNER | TCR_ORGN_WBWA | TCR_IRGN_WBWA;
    } else {
        /* PTWs cacheable, inner/outer WBWA and inner shareable */
        tcr |= TCR_TG0_64K | TCR_SHARED_INNER | TCR_ORGN_WBWA | TCR_IRGN_WBWA;
    }

    tcr |= TCR_T0SZ(va_bits);

    if (pips != NULL) {
        *pips = ips;
    }

    if (pva_bits != NULL) {
        *pva_bits = va_bits;
    }
    return tcr;
}

static U32 mmu_get_pte_type(U64 const *pte)
{
    return (U32)(*pte & PTE_TYPE_MASK);
}

/* Returns the LSB number for a PTE on level <level> */
static U32 mmu_level2shift(U32 level, U32 granule)
{
    /* 4K:  Page is 12 bits wide, 每 level 转换  9 bits */
    /* 64K: Page is 16 bits wide, 每 level 转换 13 bits */
    if (granule == MMU_GRANULE_4K) {
        return (U32)(MMU_BITS_12 + MMU_BITS_9 * (MMU_LEVEL_3 - level));
    } else {
        return (U32)(MMU_BITS_16 + MMU_BITS_13 * (MMU_LEVEL_3 - level));
    }
}

static U64 *mmu_find_pte(U64 addr, U32 level, mmu_ctrl_s *mmu_ctrl)
{
    U64 *pte = NULL;
    U64 idx;
    U32 i;

    if (level < mmu_ctrl->start_level) {
        return NULL;
    }

    /* Walk through all page table levels to find our PTE */
    pte = (U64 *)mmu_ctrl->tlb_addr;

    for (i = mmu_ctrl->start_level; i < MMU_LEVEL_MAX; ++i) {
        if (mmu_ctrl->granule == MMU_GRANULE_4K) {
            idx = (addr >> mmu_level2shift(i, mmu_ctrl->granule)) & 0x1FF;
        } else {
            idx = (addr >> mmu_level2shift(i, mmu_ctrl->granule)) & 0x1FFF;
        }

        pte += idx;

        /* Found it */
        if (i == level) {
            return pte;
        }

        /* PTE is no table (either invalid or block), can't traverse */
        if (mmu_get_pte_type(pte) != PTE_TYPE_TABLE) {
            return NULL;
        }

        /* Off to the next level */
        if (mmu_ctrl->granule == MMU_GRANULE_4K) {
            pte = (U64 *)(*pte & PTE_TABLE_ADDR_MARK_4K);
        } else {
            pte = (U64 *)(*pte & PTE_TABLE_ADDR_MARK_64K);
        }
    }

    /* Should never reach here */
    return NULL;
}

/* Returns and creates a new full table (512 entries) */
static U64 *mmu_create_table(mmu_ctrl_s *mmu_ctrl)
{
    U32 pt_len;
    U64 *new_table = (U64 *)mmu_ctrl->tlb_fillptr;

    if (mmu_ctrl->granule == MMU_GRANULE_4K) {
        pt_len = MAX_PTE_ENTRIES_4K * sizeof(U64);
    } else {
        pt_len = MAX_PTE_ENTRIES_64K * sizeof(U64);
    }

    /* Allocate MAX_PTE_ENTRIES pte entries */
    mmu_ctrl->tlb_fillptr += pt_len;

    if (mmu_ctrl->tlb_fillptr - mmu_ctrl->tlb_addr > mmu_ctrl->tlb_size) {
        return NULL;
    }

    /* Mark all entries as invalid */
    (void)memset_s((void *)new_table, MAX_PTE_ENTRIES_64K * sizeof(U64), 0, pt_len);

    return new_table;
}

static void mmu_set_pte_table(U64 *pte, U64 *table)
{
    /* Point *pte to the new table */
    *pte = PTE_TYPE_TABLE | (U64)table;
}

static S32 mmu_add_map_pte_process(mmu_mmap_region_s const *map, U64 *pte, U64 phys, U32 level, mmu_ctrl_s *mmu_ctrl)
{
    U64 *new_table = NULL;

    if (level < map->max_level) {
        if (mmu_get_pte_type(pte) == PTE_TYPE_FAULT) {
            new_table = mmu_create_table(mmu_ctrl); /* Page doesn't fit, create subpages */
            if (new_table == NULL) {
                return -1;
            }
            mmu_set_pte_table(pte, new_table);
        }
    } else if (level == MMU_LEVEL_3) {  /* 根据协议要求, level 3不能配置为BLOCK */
        *pte = phys | map->attrs | PTE_TYPE_PAGE;
    } else {
        *pte = phys | map->attrs | PTE_TYPE_BLOCK;
    }

    return 0;
}

static S32 mmu_add_map(mmu_mmap_region_s const *map, mmu_ctrl_s *mmu_ctrl)
{
    U64 virt = map->virt;
    U64 phys = map->phys;
    U64 max_level = map->max_level;
    U64 start_level = mmu_ctrl->start_level;
    U64 block_size = 0;
    U64 map_size = 0;
    U32 level;
    U64 *pte = NULL;
    S32 ret;

    if (map->max_level <= start_level) { /* 4K页面至少2级映射 */
        return -2;
    }

    while (map_size < map->size) {
        for (level = start_level; level <= max_level; level++) {
            pte = mmu_find_pte(virt, level, mmu_ctrl);
            if (pte == NULL) {
                return -3;
            }

            ret = mmu_add_map_pte_process(map, pte, phys, level, mmu_ctrl);
            if (ret) {
                return ret;
            }

            if (level != start_level) { /* 第0级不能配置为BLOCK, 特殊处理 */
                block_size = 1ULL << mmu_level2shift(level, mmu_ctrl->granule);
            }
        }

        virt += block_size;
        phys += block_size;
        map_size += block_size;
    }

    return 0;
}

static inline void mmu_set_ttbr_tcr_mair(U64 table, U64 tcr, U64 attr)
{
    OS_EMBED_ASM("dsb sy");

    OS_EMBED_ASM("msr ttbr0_el1, %0" : : "r" (table) : "memory");
    OS_EMBED_ASM("msr ttbr1_el1, %0" : : "r" (table) : "memory");
    OS_EMBED_ASM("msr tcr_el1, %0" : : "r" (tcr) : "memory");
    OS_EMBED_ASM("msr mair_el1, %0" : : "r" (attr) : "memory");

    OS_EMBED_ASM("isb");
}

static U32 mmu_setup_pgtables(mmu_mmap_region_s *mem_map, U32 mem_region_num, U64 tlb_addr, U64 tlb_len, U32 granule)
{
    U32 i;
    U32 ret;
    U64 tcr;
    U64 *new_table = NULL;
    mmu_ctrl_s mmu_ctrl = { tlb_addr, tlb_len, tlb_addr, granule, 0, 0 };

    tcr = mmu_get_tcr(NULL, &mmu_ctrl.va_bits, mmu_ctrl.granule, mem_map, mem_region_num);

    if (mmu_ctrl.granule == MMU_GRANULE_4K) {
        /* 4K: Page is 12 bits wide, 最多4级, 每level转换9bits, 小于39从第1级页面开始即可. */
        if (mmu_ctrl.va_bits < MMU_BITS_39) {
            mmu_ctrl.start_level = MMU_LEVEL_1;
        } else {
            mmu_ctrl.start_level = MMU_LEVEL_0;
        }
    } else {
        /* 64K: Page is 16 bits wide, 最多3级, 每level转换13bits, 小于36从第2级页面开始即可. */
        if (mmu_ctrl.va_bits <= MMU_BITS_36) {
            mmu_ctrl.start_level = MMU_LEVEL_2;
        } else {
            mmu_ctrl.start_level = MMU_LEVEL_1;
            return 3;
        }
    }

    /* 创建顶级页表 */
    new_table = mmu_create_table(&mmu_ctrl);
    if (new_table == NULL) {
        return 1;
    }

    /* 创建后续页表 */
    for (i = 0; i < mem_region_num; i++) {
        ret = mmu_add_map(&mem_map[i], &mmu_ctrl);
        if (ret) {
            return ret;
        }
    }

    mmu_set_ttbr_tcr_mair(mmu_ctrl.tlb_addr, tcr, MEMORY_ATTRIBUTES);

    return 0;
}

static S32 mmu_setup(void)
{
    S32 ret;
    U64 page_addr;
    U64 page_len;

    page_addr = (U64)&g_mmu_page_begin;
    page_len = (U64)&g_mmu_page_end - (U64)&g_mmu_page_begin;

    /* 配置各级页面 */
    ret = mmu_setup_pgtables(g_mem_map_info_each, (sizeof(g_mem_map_info_each) / sizeof(g_mem_map_info_each[0])),
                             page_addr, page_len, MMU_GRANULE_4K);
    if (ret) {
        return ret;
    }

    return 0;
}

/* mmu初始化主入口 */
S32 mmu_init(void)
{
    S32 ret;
    /* 配置使能mmu */
    ret = mmu_setup();
    if (ret) {
        return ret;
    }

    os_asm_flush_dcache_all();
    /* 无效&使能DChache */
    os_asm_invalidate_dcache_all();
    os_asm_invalidate_icache_all();
    os_asm_invalidate_tlb_all();

    set_sctlr(get_sctlr() | CR_C | CR_M | CR_I);

    return 0;
}
