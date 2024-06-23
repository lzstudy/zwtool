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
 * Description: mmu头文件
 */
#ifndef __MMU_H__
#define __MMU_H__

#include "prt_typedef.h"
/*
 * block/section address mask and size definitions.
 */
#define CACHE_POS           0x2
#define CACHE_MASK          0x7
#define CACHE_ENABLE        0x4

/***************************************************************/
/*
 * Memory types
 */
#define MT_DEVICE_NGNRNE    0
#define MT_DEVICE_NGNRE     1
#define MT_DEVICE_GRE       2
#define MT_NORMAL_NC        3
#define MT_NORMAL           4

#define MEMORY_ATTRIBUTES   ((0x00 << (MT_DEVICE_NGNRNE * 8)) | \
                             (0x04 << (MT_DEVICE_NGNRE * 8))   | \
                             (0x0c << (MT_DEVICE_GRE * 8))     | \
                             (0x44 << (MT_NORMAL_NC * 8))      | \
                             (0xffUL << (MT_NORMAL * 8)))

/*
 * Hardware page table definitions.
 *
 */
#define PTE_TYPE_MASK           (3 << 0)
#define PTE_TYPE_FAULT          (0 << 0)
#define PTE_TYPE_TABLE          (3 << 0)
#define PTE_TYPE_PAGE           (3 << 0)
#define PTE_TYPE_BLOCK          (1 << 0)
#define PTE_TYPE_VALID          (1 << 0)

#define PTE_TABLE_PXN           (1UL << 59)
#define PTE_TABLE_XN            (1UL << 60)
#define PTE_TABLE_AP            (1UL << 61)
#define PTE_TABLE_NS            (1UL << 63)

/*
 * Block
 */
#define PTE_BLOCK_MEMTYPE(x)    ((x) << 2)
#define PTE_BLOCK_NS            (1 << 5)
#define PTE_BLOCK_AP_R          (2 << 6)
#define PTE_BLOCK_AP_RW         (0 << 6)
#define PTE_BLOCK_NON_SHARE     (0 << 8)
#define PTE_BLOCK_OUTER_SHARE   (2 << 8)
#define PTE_BLOCK_INNER_SHARE   (3 << 8)
#define PTE_BLOCK_AF            (1 << 10)
#define PTE_BLOCK_NG            (1 << 11)
#define PTE_BLOCK_PXN           (1UL << 53)
#define PTE_BLOCK_UXN           (1UL << 54)

/*
 * AttrIndx[2:0]
 */
#define PMD_ATTRINDX(t)     ((t) << 2)
#define PMD_ATTRINDX_MASK   (7 << 2)
#define PMD_ATTRMASK        (PTE_BLOCK_PXN      | \
                            PTE_BLOCK_UXN       | \
                            PMD_ATTRINDX_MASK   | \
                            PTE_TYPE_VALID)

/*
 * TCR flags.
 */
#define TCR_IPS(x)          ((x) << 32) 
#define TCR_T0SZ(x)         ((64 - (x)) << 0)
#define TCR_IRGN_NC         (0 << 8)
#define TCR_IRGN_WBWA       (1 << 8)
#define TCR_IRGN_WT         (2 << 8)
#define TCR_IRGN_WBNWA      (3 << 8)
#define TCR_IRGN_MASK       (3 << 8)
#define TCR_ORGN_NC         (0 << 10)
#define TCR_ORGN_WBWA       (1 << 10)
#define TCR_ORGN_WT         (2 << 10)
#define TCR_ORGN_WBNWA      (3 << 10)
#define TCR_ORGN_MASK       (3 << 10)
#define TCR_SHARED_NON      (0 << 12)
#define TCR_SHARED_OUTER    (2 << 12)
#define TCR_SHARED_INNER    (3 << 12)
#define TCR_TG0_4K          (0 << 14)
#define TCR_TG0_64K         (1 << 14)
#define TCR_TG0_16K         (2 << 14)
#define TCR_EPD1_DISABLE    (1 << 23)

#define TCR_EL1_RSVD        (1UL << 31)
#define TCR_EL2_RSVD        (1UL << 31 | 1UL << 23)
#define TCR_EL3_RSVD        (1UL << 31 | 1UL << 23)

#define MAX_PTE_ENTRIES_4K      512
#define MAX_PTE_ENTRIES_64K     8192

#define PTE_TABLE_ADDR_MARK_4K  0x0000FFFFFFFFF000ULL
#define PTE_TABLE_ADDR_MARK_64K 0x0000FFFFFFFF0000ULL

typedef enum {
    MMU_LEVEL_0 = 0,
    MMU_LEVEL_1,
    MMU_LEVEL_2,
    MMU_LEVEL_3,
    MMU_LEVEL_MAX
} mmu_level_e;

typedef enum {
    MMU_PHY_ADDR_LEVEL_0 = 0,   /* 32 bits, 4GB */
    MMU_PHY_ADDR_LEVEL_1,       /* 36 bits, 64GB */
    MMU_PHY_ADDR_LEVEL_2,       /* 40 bits, 1TB */
    MMU_PHY_ADDR_LEVEL_3,       /* 42 bits, 4TB */
    MMU_PHY_ADDR_LEVEL_4,       /* 44 bits, 16TB */
    MMU_PHY_ADDR_LEVEL_5        /* 48 bits, 256TB */
} mmu_physical_addr_size_e;

typedef enum {
    MMU_BITS_9 = 9,
    MMU_BITS_12 = 12,
    MMU_BITS_13 = 13,
    MMU_BITS_16 = 16,
    MMU_BITS_32 = 32,
    MMU_BITS_36 = 36,
    MMU_BITS_39 = 39,
    MMU_BITS_40 = 40,
    MMU_BITS_42 = 42,
    MMU_BITS_44 = 44,
    MMU_BITS_48 = 48
} mmu_bits_e;

typedef struct {
    U64 tlb_addr;
    U64 tlb_size;
    U64 tlb_fillptr;        // 当前表项指针
    U32 granule;
    U32 start_level;
    U32 va_bits;
} mmu_ctrl_s;

/*
Attr<n>[7:4]        Meaning
0000                Device memory. See encoding of Attr<n>[3:0] for the type of Device memory.
00RW, RW not 00     Normal Memory, Outer Write-Through transient
0100                Normal Memory, Outer Non-cacheable
01RW, RW not 00     Normal Memory, Outer Write-Back transient
10RW                Normal Memory, Outer Write-Through non-transient
11RW                Normal Memory, Outer Write-Back non-transient
      R = Outer Read-Allocate policy, W = Outer Write-Allocate policy.

Attr<n>[3:0]       Meaning when Attr<n>[7:4] is 0000        Meaning when Attr<n>[7:4] is not 0000
0000               Device-nGnRnE memory                     UNPREDICTABLE
00RW, RW not 00    UNPREDICTABLE                            Normal Memory, Inner Write-Through transient
0100               Device-nGnRE memory                      Normal memory, Inner Non-cacheable
01RW, RW not 00    UNPREDICTABLE                            Normal Memory, Inner Write-Back transient
1000               Device-nGRE memory                       Normal Memory, Inner Write-Through non-transient (RW=00)
10RW, RW not 00    UNPREDICTABLE                            Normal Memory, Inner Write-Through non-transient
1100               Device-GRE memory                        Normal Memory, Inner Write-Back non-transient (RW=00)
11RW, RW not 00    UNPREDICTABLE                            Normal Memory, Inner Write-Back non-transient
*/

/* 用户可见的配置 */
#define MMU_ATTR_DEVICE_NGNRNE      (PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE))
#define MMU_ATTR_DEVICE             (PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRE))
#define MMU_ATTR_UNCACHE_UNSHARE    (PTE_BLOCK_MEMTYPE(MT_NORMAL_NC) | PTE_BLOCK_NON_SHARE)
#define MMU_ATTR_UNCACHE_SHARE      (PTE_BLOCK_MEMTYPE(MT_NORMAL_NC) | PTE_BLOCK_INNER_SHARE)
#define MMU_ATTR_CACHE_UNSHARE      (PTE_BLOCK_MEMTYPE(MT_NORMAL)    | PTE_BLOCK_NON_SHARE)
#define MMU_ATTR_CACHE_SHARE        (PTE_BLOCK_MEMTYPE(MT_NORMAL)    | PTE_BLOCK_INNER_SHARE)
#define MMU_ATTR_MASK               0x31CULL

#define MMU_ACCESS_NONE         (PTE_BLOCK_AP_RW)                   /* PTE_BLOCK_AF不设置, 不可访问 不可执行 */
#define MMU_ACCESS_R            (PTE_BLOCK_AF | PTE_BLOCK_UXN | PTE_BLOCK_PXN | PTE_BLOCK_AP_R)  /* 只读 */
#define MMU_ACCESS_RW           (PTE_BLOCK_AF | PTE_BLOCK_UXN | PTE_BLOCK_PXN | PTE_BLOCK_AP_RW) /* 可读可写不可执行 */
#define MMU_ACCESS_RWX          (PTE_BLOCK_AF | PTE_BLOCK_AP_RW)    /* 可读可写可执行 */
#define MMU_ACCESS_RX           (PTE_BLOCK_AF | PTE_BLOCK_AP_R)     /* 可读可执行 */
#define MMU_ACCESS_MASK         0x600000000004C0ULL

#define MMU_GRANULE_4K   0
#define MMU_GRANULE_64K  1

typedef struct {
    U64 virt;
    U64 phys;
    U64 size;
    U64 max_level;
    U64 attrs;
} mmu_mmap_region_s;

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/*
 * SCTLR_EL1/SCTLR_EL2/SCTLR_EL3 bits definitions
 */
#define CR_M                (1 << 0)  /* MMU enable */
#define CR_A                (1 << 1)  /* Alignment abort enable */
#define CR_C                (1 << 2)  /* Dcache enable */
#define CR_SA               (1 << 3)  /* Stack Alignment Check Enable */
#define CR_I                (1 << 12) /* Icache enable */
#define CR_WXN              (1 << 19) /* Write Permision Imply XN */
#define CR_EE               (1 << 25) /* Exception (Big) Endian */

/*
 * SPSR_EL3/SPSR_EL2 bits definitions
 */
#define SPSR_EL_END_LE      (0 << 9)  /* Exception Little-endian */
#define SPSR_EL_DEBUG_MASK  (1 << 9)  /* Debug exception masked */
#define SPSR_EL_ASYN_MASK   (1 << 8)  /* Asynchronous data abort masked */
#define SPSR_EL_SERR_MASK   (1 << 8)  /* System Error exception masked */
#define SPSR_EL_IRQ_MASK    (1 << 7)  /* IRQ exception masked */
#define SPSR_EL_FIQ_MASK    (1 << 6)  /* FIQ exception masked */
#define SPSR_EL_T_A32       (0 << 5)  /* AArch32 instruction set A32 */
#define SPSR_EL_M_AARCH64   (0 << 4)  /* Exception taken from AArch64 */
#define SPSR_EL_M_AARCH32   (1 << 4)  /* Exception taken from AArch32 */
#define SPSR_EL_M_SVC       (0x3)     /* Exception taken from SVC mode */
#define SPSR_EL_M_HYP       (0xa)     /* Exception taken from HYP mode */
#define SPSR_EL_M_EL1H      (5)       /* Exception taken from EL1h mode */
#define SPSR_EL_M_EL2H      (9)       /* Exception taken from EL2h mode */

/*
 * CPUECTLR_EL1 bits definitions
 */
#define CPUECTLR_EL1_L1PCTL_MASK (7 << 13) /* L1 Data prefetch control */
#define CPUECTLR_EL1_L3PCTL_MASK (7 << 10) /* L3 Data prefetch control */

static inline unsigned long get_sctlr(void)
{
    unsigned long val;

    __asm__ __volatile__("mrs %0, sctlr_el1" : "=r" (val) : : "cc");

    return val;
}

static inline unsigned long get_cpuectr(void)
{
    unsigned long val;

    __asm__ __volatile__("mrs %0, S3_1_C15_C2_1" : "=r" (val) : : "cc");

    return val;
}

static inline void set_sctlr(unsigned long val)
{
    __asm__ __volatile__("dsb sy");
    __asm__ __volatile__("msr sctlr_el1, %0" : : "r" (val) : "cc");
    __asm__ __volatile__("dsb sy");
    __asm__ __volatile__("isb");
}

extern S32 mmu_init(void);

#endif /* __MMU_H__ */
