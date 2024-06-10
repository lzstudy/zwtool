
/*================================================================= 01 - malloc */
priv = devm_kzalloc(dev, sizeof(xxx_priv), GFP_KERNEL);
DI_RET(dev, !priv, -ENOMEM, "devm_kzalloc fail");

/*================================================================= 02 - 原子操作 */
atomic_t v = ATOMIC_INIT(i)
atomic_set(&v, i);                          // 设置i
atomic_add(&v, i);                          // +i
atomic_sub(&v, i);                          // -i
atomic_inc(&v);                             // ++
atomic_dev(&v);                             // --
atomic_dec_and_test(&v);                    // +1并测试是否为0
atomic_inc_and_test(&v);                    // +1并测试是否为0
/*================================================================= 03 - 自旋锁, 锁类不需要判断返回值 */
spinlock_t lock;
pin_lock_init(&lock);                       // 初始化自旋锁
spin_lock(&lock);                           // 上锁
spin_unlock(&lock);                         // 解锁
/*================================================================= 04 - 读写锁 */
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
/*================================================================= 05 - 顺序锁 */
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
/*================================================================= 06 - 信号量 */
struct semaphore sem;
sema_init(&sem,val);                        // 初始化信号量
down_interruptible(&sem);                   // 获取信号量
up(&sem);                                   // 释放信号量
/*================================================================= 07 - 互斥体 */
struct mutex lock;
mutex_init(&lock);
mutex_lock_interruptible

/*================================================================= 08 - 中断 - 上半部 */
#include <linux/of_irq.h>
#include <linux/interrupt.h>
#define IRQ_TRIGGER_BOTH 			(IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING)

static irqreturn_t xxx_irq_handler(int irq, void *data)
{
    return IRQ_RETVAL(IRQ_HANDLED);
}

/* 获取中断号 */
priv->irq = gpio_to_irq(priv->pin);
DS_RET(priv->pin, gpio_to_irq);

/* 设置中断 */
ret = devm_request_irq(dev, priv->irq, xxx_irq_handler, IRQ_TRIGGER_BOTH, "xxx-irq", input);
DS_RET(ret, devm_request_irq);
/*================================================================= 09 - timer */
#include <linux/timer.h>
struct timer_list timer;

static void timer_isr(unsigned long data)
{

}

/* 初始化定时器 */
init_timer(&timer);
priv->timer.function = timer_isr;
priv->timer.data = (unsigned long)input;

/* 修改定时器, 一般在中断中修改 */
mod_timer(&timer, jiffies + msecs_to_jiffies(10));

/*================================================================= 10 - gpio接口 */
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

/*================================================================= 11 - input子系统 */
#include <linux/input.h>
struct input_dev *input;

/* 申请设备 */
input = devm_input_allocate_device(dev);
DP_RET(input, devm_input_allocate_device);

/* 注册input设备 */
input->name = "xxx";
input->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);
input_set_capability(input, EV_KEY, KEY_0);

ret = input_register_device(input);
DS_RET(ret, input_register_device);

input_set_drvdata(input, priv);

/* 汇报按键 */
input_report_key(input, KEY_0, !val);
input_sync(input);
}