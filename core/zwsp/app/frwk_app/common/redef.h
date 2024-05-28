#ifndef __REDEF_H__
#define __REDEF_H__
__BEGIN_DECLS

#define SECTION(x)                  __attribute__((section(x)))
#define APP_USED                    __attribute__((used))

typedef int (*init_fn_t)(void);
#define INIT_EXPORT(fn, level)    \
            APP_USED const init_fn_t __cmpt_init_##fn SECTION(".init_fn." level) = fn

__END_DECLS
#endif