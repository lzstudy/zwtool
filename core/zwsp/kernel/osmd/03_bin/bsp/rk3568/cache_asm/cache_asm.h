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
 * Description: cache头文件
 */
#ifndef __CACHE_H__
#define __CACHE_H__
#include "prt_base.h"

extern void os_asm_flush_dcache_all(void);
extern void os_asm_invalidate_dcache_all(void);
extern void os_asm_flush_dcache_range(U64 start, U64 end);
extern void os_asm_invalidate_dcache_range(U64 start, U64 end);
extern void os_asm_clean_dcache_all(void);
extern void os_asm_clean_dcache_range(U64 start, U64 end);
extern void os_asm_clean_dcache_to_pou(U64 start, U64 end);
extern void os_asm_invalidate_icache_all(void);
extern void os_asm_invalidate_tlb_all(void);

#endif
