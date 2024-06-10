#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/of_platform.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/input.h>
#include <linux/timer.h>
#include "ckerr.h"

typedef struct _xxx_priv {
	int pin;																	/**@ 引脚. */
	int irq;																	/**@ 中断号. */
	struct timer_list timer;													/**@ 定时器. */
}xxx_priv;

#define IRQ_TRIGGER_BOTH 			(IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING)

/**************************************************************************************************
 * @brief  : 解析设备树
 * @param  : 私有数据
 * @param  : 设备节点
 * @return : 0成功, -1失败
**************************************************************************************************/
static int parse_dtb(xxx_priv *priv, struct device *dev, struct device_node *np)
{
	/* 获取引脚 */
	priv->pin = of_get_named_gpio(np, "gpios", 0);
	DS_RET(priv->pin, of_get_named_gpio);

	/* 获取中断号 */
	priv->irq = gpio_to_irq(priv->pin);
	DS_RET(priv->pin, gpio_to_irq);
	return 0;
}

/**************************************************************************************************
 * @brief  : 中断处理函数
 * @param  : 中断号
 * @param  : 私有数据
 * @return : 0成功, -1失败
**************************************************************************************************/
static irqreturn_t xxx_irq_handler(int irq, void *data)
{
	struct input_dev *input = (struct input_dev *)data;
	xxx_priv *priv = input_get_drvdata(input);

	mod_timer(&priv->timer, jiffies + msecs_to_jiffies(10));
	return IRQ_RETVAL(IRQ_HANDLED);
}

/**************************************************************************************************
 * @brief  : 定时器中断
 * @param  : 中断号
 * @param  : 私有数据
 * @return : 0成功, -1失败
**************************************************************************************************/
static void timer_isr(unsigned long data)
{
	int val;
	struct input_dev *input = (struct input_dev *)data;
	xxx_priv *priv = input_get_drvdata(input);

	/* 读取按键 */
	val = gpio_get_value(priv->pin);

	/* 上报key值 */
	input_report_key(input, KEY_0, !val);
	input_sync(input);
}

/**************************************************************************************************
 * @brief  : 设置硬件属性
 * @param  : 设备
 * @return : 0成功, -1失败
**************************************************************************************************/
static int set_hardware(xxx_priv *priv, struct device *dev, struct input_dev *input)
{
	int ret;

	/* 申请GPIO */
	ret = devm_gpio_request(dev, priv->pin, "xxx-pin");
	DS_RET(ret, devm_gpio_request);

	/* 设置输出 */
	ret = gpio_direction_input(priv->pin);
	DS_RET(ret, gpio_direction_input);

	/* 设置中断 */
	ret = devm_request_irq(dev, priv->irq, xxx_irq_handler, IRQ_TRIGGER_BOTH, "xxx-irq", input);
	DS_RET(ret, devm_request_irq);

	return 0;
}

/**************************************************************************************************
 * @brief  : 驱动匹配
 * @param  : 平台设备
 * @return : 0成功, -1失败
**************************************************************************************************/
static int xxx_probe(struct platform_device *pdev)
{
	int ret;
	xxx_priv *priv;
	struct input_dev *input;
	struct device *dev = &pdev->dev;

	/* 设置ckerr */
	set_ckerr_dev(dev);

	/* 申请input设备 */
	input = devm_input_allocate_device(dev);
	DP_RET(input, devm_input_allocate_device);

	/* 申请内存空间 */
	priv = devm_kzalloc(dev, sizeof(xxx_priv), GFP_KERNEL);
	DP_RET(priv, devm_kzalloc);

	/* 解析设备树 */
	ret = parse_dtb(priv, dev, dev->of_node);
	CK_RET(ret < 0, ret);

	/* 设置硬件 */
	ret = set_hardware(priv, dev, input);
	CK_RET(ret < 0, ret);

	/* 设置定时器 */
	init_timer(&priv->timer);
	priv->timer.function = timer_isr;
	priv->timer.data = (unsigned long)input;
	
	/* 注册input */
	input->name = "xxx";
	input->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);
	input_set_capability(input, EV_KEY, KEY_0);

	ret = input_register_device(input);
	DS_RET(ret, input_register_device);

	/* 保存私有数据 */
	input_set_drvdata(input, priv);
	LOG_I("xxx init");

	return 0;
}

/**************************************************************************************************
 * @brief  : 驱动删除
 * @param  : 设备
 * @return : 0成功, -1失败
**************************************************************************************************/
static int xxx_remove(struct platform_device *pdev)
{
	LOG_I("xxx remove");
	return 0;
}

static const struct of_device_id xxx_match[] = {
	{ .compatible = "zw-key" },
	{},
};
MODULE_DEVICE_TABLE(of, xxx_match);

static struct platform_driver xxx_driver = {
    .probe  = xxx_probe,
    .remove = xxx_remove,
	.driver	= {
		.name = "xxx",
		.of_match_table = of_match_ptr(xxx_match),
	},
};

module_platform_driver(xxx_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZW");
MODULE_DESCRIPTION("dirver - xxx");
