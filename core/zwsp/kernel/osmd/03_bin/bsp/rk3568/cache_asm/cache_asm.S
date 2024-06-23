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
 * Description: cache based on armv8
 */
.global os_asm_clean_dcache_all
.global os_asm_flush_dcache_all
.global os_asm_invalidate_dcache_all
.global os_asm_flush_dcache_range
.global os_asm_invalidate_dcache_range
.global os_asm_clean_dcache_range
.global os_asm_invalidate_icache_all
.global os_asm_clean_dcache_to_pou
.global os_asm_invalidate_tlb_all

/*
 * void os_asm_dcache_level(level)
 *
 * flush or invalidate one level cache.
 *
 * x0: cache level
 * x1: 0 clean & invalidate, 1 invalidate only
 * x2~x9: clobbered
 */
    .type os_asm_dcache_level, "function"
os_asm_dcache_level:
    lsl    x12, x0, #1
    msr    csselr_el1, x12      /* select cache level */
    isb                         /* sync change of cssidr_el1 */
    mrs    x6, ccsidr_el1       /* read the new cssidr_el1 */
    and    x2, x6, #7           /* x2 <- log2(cache line size)-4 */
    add    x2, x2, #4           /* x2 <- log2(cache line size) */
    mov    x3, #0x3ff
    and    x3, x3, x6, lsr #3   /* x3 <- max number of #ways */
    clz    w5, w3               /* bit position of #ways */
    mov    x4, #0x7fff
    and    x4, x4, x6, lsr #13  /* x4 <- max number of #sets */
    /* x12 <- cache level << 1 */
    /* x2 <- line length offset */
    /* x3 <- number of cache ways - 1 */
    /* x4 <- number of cache sets - 1 */
    /* x5 <- bit position of #ways */

loop_set:
    mov    x6, x3               /* x6 <- working copy of #ways */
loop_way:
    lsl    x7, x6, x5
    orr    x9, x12, x7          /* map way and level to cisw value */
    lsl    x7, x4, x2
    orr    x9, x9, x7           /* map set number to cisw value */
    tbz    w1, #1, 3f           /* x1[1] == 0, jump to 3 */
    dc     csw, x9              /* x[1] == 1, clean */
    b      2f
3:  tbz    w1, #0, 1f           /* x[0] == 0, jump to 1 */
    dc     isw, x9
    b      2f
1:  dc     cisw, x9             /* clean & invalidate by set/way */
2:  subs   x6, x6, #1           /* decrement the way */
    b.ge   loop_way
    subs   x4, x4, #1           /* decrement the set */
    b.ge   loop_set

    ret


/*
 * void os_asm_flush_dcache_all(int invalidate_only)
 *
 * x0: 0 clean & invalidate, 1 invalidate only
 *
 * flush or invalidate all data cache by SET/WAY.
 */
    .type os_asm_dcache_all, "function"
os_asm_dcache_all:
    mov    x1, x0
    dsb    sy
    mrs    x10, clidr_el1       /* read clidr_el1 */
    lsr    x11, x10, #24
    and    x11, x11, #0x7       /* x11 <- loc */
    cbz    x11, finished        /* if loc is 0, exit */
    mov    x15, x30
    mov    x0, #0               /* start flush at cache level 0 */
    /* x0  <- cache level */
    /* x10 <- clidr_el1 */
    /* x11 <- loc */
    /* x15 <- return address */

loop_level:
    lsl    x12, x0, #1
    add    x12, x12, x0         /* x0 <- tripled cache level */
    lsr    x12, x10, x12
    and    x12, x12, #7         /* x12 <- cache type */
    cmp    x12, #2
    b.lt   skip                 /* skip if no cache or icache */
    bl     os_asm_dcache_level   /* x1 = 0 flush, 1 invalidate */
skip:
    add    x0, x0, #1           /* increment cache level */
    cmp    x11, x0
    b.gt   loop_level

    mov    x0, #0
    msr    csselr_el1, x0       /* restore csselr_el1 */
    dsb    sy
    isb
    mov    x30, x15

finished:
    ret


    .type os_asm_flush_dcache_all, "function"
os_asm_flush_dcache_all:
    mov    x0, #0
    b    os_asm_dcache_all


    .type os_asm_invalidate_dcache_all, "function"
os_asm_invalidate_dcache_all:
    mov    x0, #0x1
    b    os_asm_dcache_all


    .type os_asm_clean_dcache_all, "function"
os_asm_clean_dcache_all:
    mov    x0, #0x2
    b    os_asm_dcache_all


/*
 * void os_asm_flush_dcache_range(start, end)
 *
 * clean & invalidate data cache in the range
 *
 * x0: start address
 * x1: end address
 */
    .type os_asm_flush_dcache_range, "function"
os_asm_flush_dcache_range:
    mrs    x3, ctr_el0
    lsr    x3, x3, #16
    and    x3, x3, #0xf
    mov    x2, #4
    lsl    x2, x2, x3           /* cache line size */

    /* x2 <- minimal cache line size in cache system */
    sub    x3, x2, #1
    bic    x0, x0, x3
1:  dc     civac, x0            /* clean & invalidate data or unified cache */
    add    x0, x0, x2
    cmp    x0, x1
    b.lo   1b
    dsb    sy
    ret


/*
 * void os_asm_invalidate_dcache_range(start, end)
 *
 * invalidate data cache in the range
 *
 * x0: start address
 * x1: end address
 */
    .type os_asm_invalidate_dcache_range, "function"
os_asm_invalidate_dcache_range:
    mrs    x3, ctr_el0
    lsr    x3, x3, #16
    and    x3, x3, #0xf
    mov    x2, #4
    lsl    x2, x2, x3           /* cache line size */

    /* x2 <- minimal cache line size in cache system */
    sub    x3, x2, #1
    bic    x0, x0, x3
1:  dc     ivac, x0             /* invalidate data or unified cache */
    add    x0, x0, x2
    cmp    x0, x1
    b.lo    1b
    dsb    sy
    ret


/*
 * void os_asm_clean_dcache_range(start, end)
 *
 * clean data cache in the range
 *
 * x0: start address
 * x1: end address
 */
    .type os_asm_clean_dcache_range, "function"
os_asm_clean_dcache_range:
    mrs    x3, ctr_el0
    lsr    x3, x3, #16
    and    x3, x3, #0xf
    mov    x2, #4
    lsl    x2, x2, x3           /* cache line size */

    /* x2 <- minimal cache line size in cache system */
    sub    x3, x2, #1
    bic    x0, x0, x3
1:  dc     cvac, x0             /* clean data or unified cache */
    add    x0, x0, x2
    cmp    x0, x1
    b.lo   1b
    dsb    sy
    ret


    .type os_asm_clean_dcache_to_pou, "function"
os_asm_clean_dcache_to_pou:
    mrs    x3, ctr_el0
    lsr    x3, x3, #16
    and    x3, x3, #0xf
    mov    x2, #4
    lsl    x2, x2, x3           /* cache line size */

    /* x2 <- minimal cache line size in cache system */
    sub    x3, x2, #1
    bic    x0, x0, x3
1:  dc     cvau, x0             /* clean data or unified cache */
    add    x0, x0, x2
    cmp    x0, x1
    b.lo   1b
    dsb    sy
    ret


/*
 * void os_asm_invalidate_icache_all(void)
 *
 * invalidate all tlb entries.
 */
    .type os_asm_invalidate_icache_all, "function"
os_asm_invalidate_icache_all:
    ic    ialluis
    isb   sy
    ret

/*
 * void os_asm_invalidate_tlb_all(void)
 *
 * invalidate all tlb entries.
 */
    .type os_asm_invalidate_tlb_all, "function"
os_asm_invalidate_tlb_all:
    tlbi  vmalle1
    dsb sy
    isb
    ret
