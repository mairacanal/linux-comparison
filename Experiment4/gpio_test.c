/*
 * @file gpio_test.c
 * @author Maíra Canal
 * @date 14 january 2023
 * @brief A kernel module for Xenomai that replies a GPIO signal
 */

#include <linux/module.h>

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>

#include <rtdm/rtdm_driver.h>

#define MODULE_NAME "experiment4"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Maíra Canal");
MODULE_DESCRIPTION("A kernel module for Xenomai that replies a GPIO signal");
MODULE_VERSION("0.0.1");

static struct gpio_desc *output, *input;
static rtdm_irq_t irq_rtdm;

static int gpio_irq_handler(rtdm_irq_t *irq)
{
	unsigned int value = 0;

	value = gpiod_get_value(input);
	gpiod_set_value(output, value);
	return RTDM_IRQ_HANDLED;
}

static int gpio_probe(struct platform_device *p_dev)
{
	int result = 0;
	unsigned int irqNum;

	printk(KERN_INFO "GPIO_TEST: initializing the LKM GPIO_TEST\n");

	output = gpiod_get(&(p_dev->dev), "output", GPIOD_OUT_LOW);

	if (output == NULL) {
		printk(KERN_INFO "GPIO_TEST: GPIO not configured (input)\n");
		return 0;
	}

	input = gpiod_get(&(p_dev->dev), "input", GPIOD_IN);

	if (input == NULL) {
		printk(KERN_INFO "GPIO_TEST: GPIO not configured (output)\n");
		return 0;
	}

	printk(KERN_INFO "GPIO_TEST: GPIO configured\n");

	irqNum = gpiod_to_irq(input);
	printk(KERN_INFO "GPIO_TEST: the button is mapped to IRQ #%d\n", irqNum);

	irq_set_irq_type(irqNum,  IRQF_TRIGGER_RISING);

	result = rtdm_irq_request(&irq_rtdm, irqNum, gpio_irq_handler, RTDM_IRQTYPE_EDGE,
			          MODULE_NAME, NULL);

	printk(KERN_INFO "GPIO_TEST: the interrupt request resulted %d", result);

	return result;
}

static int gpio_remove(struct platform_device *dev)
{
	gpiod_set_value(output, 0);
	rtdm_irq_free(&irq_rtdm);

	printk(KERN_INFO "GPIO_TEST: Exiting GPIO_TEST LKM\n");

	return 0;
}

static const struct of_device_id gpio_ids[] = {
    { .compatible = "gpio-keys" },
    { }
};

MODULE_DEVICE_TABLE(of, gpio_ids);

static struct platform_driver gpio_driver = {
	.driver = {
		.name = MODULE_NAME,
		.of_match_table = gpio_ids,
	},
	.probe = gpio_probe,
	.remove = gpio_remove
};

static int __init gpio_button_init(void)
{
	return platform_driver_register(&gpio_driver);
}

static void __exit gpio_button_exit(void)
{
	platform_driver_unregister(&gpio_driver);
}

module_init(gpio_button_init);
module_exit(gpio_button_exit);
