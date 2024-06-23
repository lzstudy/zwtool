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
 * Description: mmu init
 */
#include "prt_base.h"

extern void print_asm(uintptr_t arg0);
extern void printu32_asm(U32 val);

#define TRANSLATION_GRANULE_SIZE_4KB 0 /* 4KB Byte */
#define TRANSLATION_GRANULE_SIZE_64KB 1 /* 64KB Byte */
#define TRANSLATION_GRANULE_SIZE_16KB 2 /* 16KB Byte */

#define SCTLR_EL1_MMU_ENABLE 0x1004U /* set I(bit12), C(bit2), M(bit0) */
#define TTBR_REGION_SIZE 28U /* The region size is 2(64-T0SZ) bytes */

/* MMU配置的内存共享属性 */
enum MmuShareable {
    MMU_LPAE_NON_SHAREABLE = 0x0,
    MMU_LPAE_OUTER_SHAREABLE = 0x2,
    MMU_LPAE_INNER_SHAREABLE = 0x3,
};

/* TCR寄存器的PS域 */
#define TCR_PS_4G 0x00 /* 32 bits, 4GB */
#define TCR_PS_64G 0x01 /* 36 bits, 64GB */
#define TCR_PS_1T 0x02 /* 40 bits, 1TB */
#define TCR_PS_4T 0x03 /* 42 bits, 4TB */
#define TCR_PS_16T 0x04 /* 44 bits, 16TB */
#define TCR_PS_256T 0x05 /* 48 bits, 256TB */

/* MMU配置的内存属性 */
#define MEMATTRI_DEVEICE_NGNRNE 0x00UL /* Device-nGnRnE memory */
#define MEMATTRI_DEVEICE_NGNRE 0x04UL /* Device-nGnRE memory */
#define MEMATTRI_DEVEICE_GRE 0x0CUL /* Device-GRE memory */
#define MEMATTRI_NORMAL_WRITEBACK 0xFFUL /* Normal Memory,non-transient */
#define MEMATTRI_NORMAL_NONCACHEABLE 0x44UL /* Normal Memory,Non-Cacheable */
#define MEMATTRI_NORMAL_WRITETHROUGH 0xBBUL /* Normal Memory,Write-through,non-transient */

#define MMU_ATTR_AP_EL0 0x1
#define MMU_ATTR_AP_WRITE 0x2
#define MMU_GRANULE_SIZE 12

union TcrEL1 {
    struct {
        U64 t0sz : 6; /* [0..5] The size offset of the memory region */
        U64 res0 : 1; /* [6] Reserved, RES0. */
        U64 epd0 : 1; /* [7] Translation table walk disable using TTBR0_EL1 */
        U64 irgn0 : 2; /* [8..9] Inner cacheability attribute using TTBR0_EL1 */
        U64 orgn0 : 2; /* [10..11] Outer cacheability attribute using TTBR0_EL1 */
        U64 sh0 : 2; /* [12..13] Shareability attribute using TTBR0_EL1 */
        U64 tg0 : 2; /* [14..15] TTBR0_EL1 granule size */
        U64 t1sz : 6; /* [16..21] Size offset of the memory region addressed by TTBR1_EL1. */
        U64 a1 : 1; /* [22] Selects whether TTBR0_EL1 or TTBR1_EL1 defines the ASID */
        U64 epd1 : 1; /* [23] Translation table walk disable for translations using TTBR1_EL1 */
        U64 irgn1 : 2; /* [24:25] Inner cacheability attribute using TTBR1_EL1. */
        U64 orgn1 : 2; /* [26:27] Outer cacheability attribute using TTBR1_EL1 */
        U64 sh1 : 2; /* [28:29] Shareability attribute using TTBR1_EL1 */
        U64 tg1 : 2; /* [30:31] TTBR1_EL1 granule size */
        U64 ips : 3; /* [32:34] Intermediate Physical Address Size */
        U64 res1 : 1; /* [35] Reserved, RES0. */
        U64 as : 1; /* [36] ASID size */
        U64 tbi0 : 1; /* [37] for the TTBR0_EL1 region. */
        U64 tbi1 : 1; /* [38] for the TTBR1_EL1 region */
        U64 res2 : 25; /* [39:63] Reserved, RES0. */
    } bits;
    U64 value;
};

union MairEL1 {
    struct {
        U64 attr0 : 8; /* bit[7..0] */
        U64 attr1 : 8; /* bit[15..8] */
        U64 attr2 : 8; /* bit[23..16] */
        U64 attr3 : 8; /* bit[31..24] */
        U64 attr4 : 8; /* bit[39..32] */
        U64 attr5 : 8; /* bit[47..40] */
        U64 attr6 : 8; /* bit[55..48] */
        U64 attr7 : 8; /* bit[63..56] */
    } bits;
    U64 value;
};

#define L1_WAY_NUM                      4
#define L1_SET_NUM                      128
#define L1_WAY_BIT_SHIFT                30  // 32 - log2(L1_WAY_NUM)
#define L1_SET_BIT_SHIFT                6   // log2(CACHE_LINE_SIZE)

/*
 * 描述: 使能MMU之前，需要无效L1 DCache。
 */
void L1DCacheInvAll(void)
{
    U32 wayId;
    U32 setId;
    U64 tmp;

    for (wayId = 0; wayId < L1_WAY_NUM; wayId++) {
        for (setId = 0; setId < L1_SET_NUM; setId++) {
            tmp = (wayId << L1_WAY_BIT_SHIFT) | (setId << L1_SET_BIT_SHIFT);
            OS_EMBED_ASM("DC ISW, %0      \n"
                         : : "r"(tmp) : "memory");
        }
    }
    OS_EMBED_ASM("DSB    SY        \n");
}

/*
 * 描述: 配置TCR_EL1。
 */
void OsMmuCfgTcrEL1(void)
{
    union TcrEL1 tcrEL1;
    U32 tcr;
    tcrEL1.value = 0;
    OS_EMBED_ASM("MRS    %0, TCR_EL1         \n"
                 : "=&r"(tcrEL1.value) : : "memory");

    tcrEL1.bits.t0sz = TTBR_REGION_SIZE;
    tcrEL1.bits.irgn0 = 1; /* 1:Normal memory, Inner Write-Back Write-Allocate Cacheable */
    tcrEL1.bits.orgn0 = 1; /* 1:Normal memory, Outer Write-Back Write-Allocate Cacheable */
    tcrEL1.bits.sh0 = MMU_LPAE_INNER_SHAREABLE;
    tcrEL1.bits.tg0 = TRANSLATION_GRANULE_SIZE_4KB;
    tcrEL1.bits.ips = TCR_PS_1T; /* cortex-a53, 4G/64GB/1TB only, other values are reserved */

    tcrEL1.bits.t1sz = TTBR_REGION_SIZE;
    tcrEL1.bits.irgn1 = 1; /* 1:Normal memory, Inner Write-Back Write-Allocate Cacheable */
    tcrEL1.bits.orgn1 = 1; /* 1:Normal memory, Outer Write-Back Write-Allocate Cacheable */
    tcrEL1.bits.sh1 = MMU_LPAE_INNER_SHAREABLE;
    tcrEL1.bits.tg1 = TRANSLATION_GRANULE_SIZE_4KB;

    OS_EMBED_ASM("MSR TCR_EL1, %0            \n"
                 : : "r"(tcrEL1.value) : "memory");
    
    OS_EMBED_ASM("MRS    %0, TCR_EL1         \n"
                 : "=&r"(tcr) : : "memory");
}

/*
 * 描述: 配置MAIR_EL1。
 */
void OsMmuCfgMairEl1(void)
{
    union MairEL1 mairEl1;
    U32 mair;
    /* 0: Device(NGNRE)  1: Noncache  2: Writeback  3:Device(NGNRNE)  4: Device(GRE)  5:WriteThrough */
    mairEl1.value = 0;
    mairEl1.bits.attr0 = MEMATTRI_DEVEICE_NGNRNE; /* MEMATTRI_DEVEICE_NGNRE */
    mairEl1.bits.attr1 = MEMATTRI_NORMAL_NONCACHEABLE;
    mairEl1.bits.attr2 = MEMATTRI_NORMAL_WRITEBACK;
    mairEl1.bits.attr3 = MEMATTRI_DEVEICE_NGNRNE;
    mairEl1.bits.attr4 = MEMATTRI_DEVEICE_GRE;
    mairEl1.bits.attr5 = MEMATTRI_NORMAL_WRITETHROUGH;

    OS_EMBED_ASM("MSR MAIR_EL1, %0            \n"
                 : : "r"(mairEl1.value) : "memory");
    
    OS_EMBED_ASM("MRS  %0, MAIR_EL1            \n"
                 : "=&r"(mair) : : "memory");
    
    printu32_asm(mair);
}

/*
 * 描述: 配置TTBR0_EL1。
 */
void OsMmuCfgTtbr0El1(uintptr_t ttbr)
{
    U32 ttbr0 = 0;
    U32 ttbr1 = 0;

    OS_EMBED_ASM("MSR TTBR0_EL1, %0            \n"
                 : : "r"(ttbr) : "memory");
    OS_EMBED_ASM("MSR TTBR1_EL1, %0            \n"
                 : : "r"(ttbr) : "memory");
    
    OS_EMBED_ASM("MRS  %0, TTBR0_EL1            \n"
                 : "=&r"(ttbr0) : : "memory");
    OS_EMBED_ASM("MRS %0, TTBR1_EL1            \n"
                 : "=&r"(ttbr1) : : "memory");
}

/*
 * 描述: 使能EL1的MMU。
 */
void OsMmuEnableEl1(void)
{
    U64 tmp = 0;
    L1DCacheInvAll();
    OS_EMBED_ASM("IC  IALLU          \n"
                 "DSB NSH            \n"
                 "ISB                \n");

    OS_EMBED_ASM("MRS    %0, SCTLR_EL1         \n"
                 : "=&r"(tmp) : : "memory");

    tmp |= SCTLR_EL1_MMU_ENABLE;  /* 0x1005: set I(bit12), C(bit2), M(bit0) */
    OS_EMBED_ASM("TLBI VMALLE1                 \n"
                 "MSR  SCTLR_EL1, %0           \n"
                 "IC   IALLU                   \n"
                 "DSB  SY                      \n"
                 "ISB                          \n"
                 : : "r"(tmp));
}

/*
 * 描述: 初始化EL1 MMU。
 */
void OsMmuInitEl1(uintptr_t baseAddr)
{
    OsMmuEnableEl1();
}

/*
 * 描述: 初始化MMU。
 */
U32 OsMmuInit(void)
{
    OsMmuInitEl1(0x30f80000);
    return OS_OK;
}
