
/*================================================================= 1 - malloc */
priv = devm_kzalloc(dev, sizeof(xxx_priv), GFP_KERNEL);
DI_RET(dev, !priv, -ENOMEM, "devm_kzalloc fail");

/*================================================================= 2 - 原子操作 */
atomic_t v = ATOMIC_INIT(i)
atomic_set(&v, i);                          // 设置i
atomic_add(&v, i);                          // +i
atomic_sub(&v, i);                          // -i
atomic_inc(&v);                             // ++
atomic_dev(&v);                             // --
atomic_dec_and_test(&v);                    // +1并测试是否为0
atomic_inc_and_test(&v);                    // +1并测试是否为0
/*================================================================= 3 - 自旋锁, 锁类不需要判断返回值 */
spinlock_t lock;
pin_lock_init(&lock);                       // 初始化自旋锁
spin_lock(&lock);                           // 上锁
spin_unlock(&lock);                         // 解锁
/*================================================================= 4 - 读写锁 */
rwlock_t rwlock;
rwlock_init(&rwlock);                       // 初始化读写锁
read_lock_irqsave(&rwlock,flag);            // 读锁
read_unlock_irqrestore(&rwlock,flag);       // 读解锁
read_lock_bh(&rwlock);                      // 关闭下半部, 并获取读锁
read_unlock_bh(&rwlock);                    // 打开下版本, 并释放读锁

write_lock_irqsave(&rwlock,flag);           // 写锁
write_unlock_irqrestore(&rwlock,flag);      // 写解锁
write_lock_bh(&rwlock);                     // 关闭下半部, 并获取写锁
write_unlock_bh(&rwlock);                   // 打开下版本, 并释放写锁
/*================================================================= 5 - 顺序锁 */
seqlock_t sqlock;
seqlock_init(&sqlock);                      // 初始化顺序所
read_seqlock_irqsave(&rwlock,flag);         // 读锁
read_sequnlock_irqrestore(&rwlock,flag);    // 读解锁
read_seqlock_bh(&rwlock);                   // 关闭下半部, 并获取读锁
read_sequnlock_bh(&rwlock);                 // 打开下版本, 并释放读锁

write_seqlock_irqsave(&rwlock,flag);        // 写锁
write_sequnlock_irqrestore(&rwlock,flag);   // 写解锁
write_seqlock_bh(&rwlock);                  // 关闭下半部, 并获取写锁
write_sequnlock_bh(&rwlock);                // 打开下版本, 并释放写锁
/*================================================================= 6 - 信号量 */
struct semaphore sem;
sema_init(&sem,val);                        // 初始化信号量
down_interruptible(&sem);                   // 获取信号量
up(&sem);                                   // 释放信号量
/*================================================================= 7 - 互斥体 */
struct mutex lock;
mutex_init(&lock);
 mutex_lock_interruptible
/*================================================================= 2 - gpio接口 */
#include <linux/of_gpio.h>
/* 解析GPIO */
priv->pin = of_get_named_gpio(np, "gpios", 0);
DI_RET(dev, priv->pin < 0, priv->pin, "get gpio num fail");

/* 申请GPIO */
ret = devm_gpio_request(dev, priv->pin, "zw-led");
CI_RET(ret < 0, ret, "request fail");

/* 设置输出 */
ret = gpio_direction_output(priv->pin, 0);
CI_RET(ret < 0, ret, "direction output fail");

/* 读取GPIO值 */
val = gpio_get_value(priv->pin);