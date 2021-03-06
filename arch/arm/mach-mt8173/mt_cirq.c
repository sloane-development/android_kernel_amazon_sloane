#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/cpu.h>
#include <linux/smp.h>
#include <linux/types.h>
#include <linux/irqchip/arm-gic.h>
#include <asm/system.h>
#include <asm/mach/irq.h>
#include "mach/mt_cirq.h"
#include "mach/mt_reg_base.h"
#include "mach/irqs.h"
#include "mach/sync_write.h"
#include "mach/mt_sleep.h"


/*
 * Definition
 */
#define LDVT
#define CIRQ_LOG_LEVEL KERN_INFO

#define CIRQ_DEBUG   0
#if (CIRQ_DEBUG == 1)
#ifdef CTP
#define dbgmsg dbg_print
#define print_func() do { \
    dbgmsg("in %s\n", __func__); \
} while (0)
#else
#define dbgmsg printk
#define print_func() do { \
    dbgmsg(CIRQ_LOG_LEVEL "in %s\n", __func__); \
} while (0)
#endif
#else
#define dbgmsg(...)
#define print_func() do { } while (0)
#endif

#define cirq_num_validate(x) \
do { \
    if (x < 0 || x >= CIRQ_IRQ_NUM) { \
	printk(KERN_ERR "[CIRQ] %s: wrong cirq num %d which is less than 0 or larger than %d\n", __func__, x, CIRQ_IRQ_NUM); \
	return -1; \
    } \
} while (0)


/*
 * Define Data Structure
 */
struct mt_cirq_driver {
	struct device_driver driver;
	const struct platform_device_id *id_table;
};


/*
 * Define Global Variable
 */
static struct mt_cirq_driver mt_cirq_drv = {
	.driver = {
		   .name = "cirq",
		   .bus = &platform_bus_type,
		   .owner = THIS_MODULE,
		   },
	.id_table = NULL,
};


/*
 * Declare Funtion
 */
int mt_cirq_test(void);
void mt_cirq_dump_reg(void);


/*
 * Define Function
 */

/*
 * mt_cirq_ack_all: Ack all the interrupt on SYS_CIRQ
 */
void mt_cirq_ack_all(void)
{
	unsigned int i;

	for (i = 0; i < CIRQ_CTRL_REG_NUM; i++) {
		writel_relaxed(0xFFFFFFFF, CIRQ_ACK_BASE + (i * 4));
	}
	dsb();

	return;
}

/*
 * mt_cirq_get_mask: Get the specified SYS_CIRQ mask
 * @cirq_num: the SYS_CIRQ number to get
 * @return:
 *    1: this cirq is masked
 *    0: this cirq is umasked
 *   -1: cirq num is out of range
 */
static unsigned int mt_cirq_get_mask(unsigned int cirq_num)
{
	unsigned int st;
	unsigned int bit = 1 << (cirq_num % 32);

	print_func();
	cirq_num_validate(cirq_num);

	st = readl(IOMEM((cirq_num / 32) * 4 + CIRQ_MASK_BASE));
	return (st & bit);
}

/*
 * mt_cirq_mask_all: Mask all interrupts on SYS_CIRQ.
 */
void mt_cirq_mask_all(void)
{
	unsigned int i;

	for (i = 0; i < CIRQ_CTRL_REG_NUM; i++) {
		writel_relaxed(0xFFFFFFFF, CIRQ_MASK_SET_BASE + (i * 4));
	}
	dsb();

	return;
}

/*
 * mt_cirq_unmask_all: Unmask all interrupts on SYS_CIRQ.
 */
void mt_cirq_unmask_all(void)
{
	unsigned int i;

	for (i = 0; i < CIRQ_CTRL_REG_NUM; i++) {
		writel_relaxed(0xFFFFFFFF, CIRQ_MASK_CLR_BASE + (i * 4));
	}
	dsb();

	return;
}

/*
 * mt_cirq_mask: Mask the specified SYS_CIRQ.
 * @cirq_num: the SYS_CIRQ number to mask
 * @return:
 *    0: mask success
 *   -1: cirq num is out of range
 */
static int mt_cirq_mask(unsigned int cirq_num)
{
	unsigned int bit = 1 << (cirq_num % 32);

	print_func();
	cirq_num_validate(cirq_num);

	mt65xx_reg_sync_writel(bit, (cirq_num / 32) * 4 + CIRQ_MASK_SET_BASE);
	/* mt65xx_reg_sync_writel(bit, (cirq_num / 32) * 4 + CIRQ_ACK_BASE); */

	dbgmsg(CIRQ_LOG_LEVEL "[CIRQ] mask cirq %d; addr:0x%x = 0x%x, after set:0x%x\n",
	       cirq_num, (cirq_num / 32) * 4 + CIRQ_MASK_SET_BASE, bit,
	       readl(IOMEM((cirq_num / 32) * 4 + CIRQ_MASK_BASE)));
	return 0;
}

/*
 * mt_cirq_unmask: Unmask the specified SYS_CIRQ.
 * @cirq_num: the SYS_CIRQ number to unmask
 * @return:
 *    0: umask success
 *   -1: cirq num is out of range
 */
static int mt_cirq_unmask(unsigned int cirq_num)
{
	unsigned int bit = 1 << (cirq_num % 32);

	print_func();
	cirq_num_validate(cirq_num);

	/* mt65xx_reg_sync_writel(bit, (cirq_num / 32) * 4 + CIRQ_ACK_BASE); */
	mt65xx_reg_sync_writel(bit, (cirq_num / 32) * 4 + CIRQ_MASK_CLR_BASE);

	dbgmsg(CIRQ_LOG_LEVEL "[CIRQ] unmask cirq %d; addr:0x%x = 0x%x, after clear:0x%x\n",
	       cirq_num, (cirq_num / 32) * 4 + CIRQ_MASK_CLR_BASE, bit,
	       readl(IOMEM((cirq_num / 32) * 4 + CIRQ_MASK_BASE)));
	return 0;
}

/*
 * mt_cirq_get_sens: Get the specified SYS_CIRQ sensitivity
 * @cirq_num: the SYS_CIRQ number to get
 * @return:
 *    1: this cirq is MT_LEVEL_SENSITIVE
 *    0: this cirq is MT_EDGE_SENSITIVE
 *   -1: cirq num is out of range
 */
static unsigned int mt_cirq_get_sens(unsigned int cirq_num)
{
	unsigned int st;
	unsigned int bit = 1 << (cirq_num % 32);

	print_func();
	cirq_num_validate(cirq_num);

	st = readl(IOMEM((cirq_num / 32) * 4 + CIRQ_SENS_BASE));
	return (st & bit);
}

/*
 * mt_cirq_set_sens: Set the sensitivity for the specified SYS_CIRQ number.
 * @cirq_num: the SYS_CIRQ number to set
 * @sens: sensitivity to set
 * @return:
 *    0: set sens success
 *   -1: cirq num is out of range
 */
static int mt_cirq_set_sens(unsigned int cirq_num, unsigned int sens)
{
	unsigned int base;
	unsigned int bit = 1 << (cirq_num % 32);

	print_func();
	cirq_num_validate(cirq_num);

	if (sens == MT_EDGE_SENSITIVE) {
		base = (cirq_num / 32) * 4 + CIRQ_SENS_CLR_BASE;
	} else if (sens == MT_LEVEL_SENSITIVE) {
		base = (cirq_num / 32) * 4 + CIRQ_SENS_SET_BASE;
	} else {
		printk(KERN_ERR "[CIRQ] set_sens invalid sensitivity value %d\n", sens);
		return -1;
	}

	mt65xx_reg_sync_writel(bit, base);
	dbgmsg(CIRQ_LOG_LEVEL
	       "[CIRQ] set_sens cirq %d with sens %d; addr:0x%x = 0x%x, after set:0x%x\n", cirq_num,
	       sens, base, bit, readl(IOMEM((cirq_num / 32) * 4 + CIRQ_SENS_BASE)));
	return 0;
}

/*
 * mt_cirq_get_pol: Get the specified SYS_CIRQ polarity
 * @cirq_num: the SYS_CIRQ number to get
 * @return:
 *    1: this cirq is MT_CIRQ_POL_POS
 *    0: this cirq is MT_CIRQ_POL_NEG
 *   -1: cirq num is out of range
 */
static unsigned int mt_cirq_get_pol(unsigned int cirq_num)
{
	unsigned int st;
	unsigned int bit = 1 << (cirq_num % 32);

	print_func();
	cirq_num_validate(cirq_num);

	st = readl(IOMEM((cirq_num / 32) * 4 + CIRQ_POL_BASE));
	return (st & bit);
}

/*
 * mt_cirq_set_pol: Set the polarity for the specified SYS_CIRQ number.
 * @cirq_num: the SYS_CIRQ number to set
 * @pol: polarity to set
 * @return:
 *    0: set pol success
 *   -1: cirq num is out of range
 */
static int mt_cirq_set_pol(unsigned int cirq_num, unsigned int pol)
{
	unsigned int base;
	unsigned int bit = 1 << (cirq_num % 32);

	print_func();
	cirq_num_validate(cirq_num);

	if (pol == MT_CIRQ_POL_NEG) {
		base = (cirq_num / 32) * 4 + CIRQ_POL_CLR_BASE;
	} else if (pol == MT_CIRQ_POL_POS) {
		base = (cirq_num / 32) * 4 + CIRQ_POL_SET_BASE;
	} else {
		printk(KERN_ERR "[CIRQ] set_pol invalid polarity value %d\n", pol);
		return -1;
	}

	mt65xx_reg_sync_writel(bit, base);
	dbgmsg(CIRQ_LOG_LEVEL
	       "[CIRQ] set_pol cirq %d with pol %d; addr:0x%x = %0x%x, after set:0x%x\n", cirq_num,
	       pol, base, bit, readl(IOMEM((cirq_num / 32) * 4 + CIRQ_POL_BASE)));
	return 0;
}

/*
 * mt_cirq_enable: Enable SYS_CIRQ
 */
void mt_cirq_enable(void)
{
	unsigned int st;

	print_func();

	mt_cirq_ack_all();

	st = readl(IOMEM(CIRQ_CON));
	st |= (CIRQ_CON_EN << CIRQ_CON_EN_BITS) | (CIRQ_CON_EDGE_ONLY << CIRQ_CON_EDGE_ONLY_BITS);
	mt65xx_reg_sync_writel((st & CIRQ_CON_BITS_MASK), CIRQ_CON);

	return;
}

/*
 * mt_cirq_disable: Disable SYS_CIRQ
 */
void mt_cirq_disable(void)
{
	unsigned int st;

	print_func();

	st = readl(IOMEM(CIRQ_CON));
	st &= ~(CIRQ_CON_EN << CIRQ_CON_EN_BITS);
	mt65xx_reg_sync_writel((st & CIRQ_CON_BITS_MASK), CIRQ_CON);

	return;
}

/*
 * mt_cirq_disable: Flush interrupt from SYS_CIRQ to GIC
 */
void mt_cirq_flush(void)
{
	unsigned int i;
	unsigned int st;

	print_func();

	mt_cirq_unmask_all();	/* Whether it is masked on GIC or not */
	for (i = 0; i < CIRQ_CTRL_REG_NUM; i++) {
		st = readl(IOMEM(CIRQ_STA_BASE + (i * 4)));

#if 1				/* if(CIRQ_TO_IRQ_NUM(0) % 32 == 0) */
		mt65xx_reg_sync_writel(st,
				       GIC_DIST_BASE + GIC_DIST_PENDING_SET +
				       (CIRQ_TO_IRQ_NUM(i * 32) / 32 * 4));
#else				/* if(CIRQ_TO_IRQ_NUM(0) % 32 != 0) */
		mt65xx_reg_sync_writel(st << (CIRQ_TO_IRQ_NUM(0) % 32),
				       GIC_DIST_BASE + GIC_DIST_PENDING_SET +
				       (CIRQ_TO_IRQ_NUM(i * 32) / 32 * 4));
		if (CIRQ_TO_IRQ_NUM(0) % 32 != 0) {
			if (i != CIRQ_CTRL_REG_NUM - 1
			    || (CIRQ_TO_IRQ_NUM(CIRQ_IRQ_NUM - 1) % 32) < (CIRQ_TO_IRQ_NUM(0) % 32))
				mt65xx_reg_sync_writel(st >> (32 - (CIRQ_TO_IRQ_NUM(0) % 32)),
						       GIC_DIST_BASE + GIC_DIST_PENDING_SET +
						       (CIRQ_TO_IRQ_NUM((i + 1) * 32) / 32 * 4));
		}
#endif
	}
	mt_cirq_mask_all();	/* Can be bypassed */
	mt_cirq_ack_all();

	return;
}

/*
 * mt_cirq_clone_pol: Copy the polarity setting from GIC to SYS_CIRQ
 */
static void mt_cirq_clone_pol(void)
{
	unsigned int cirq_num, irq_num;
	unsigned int st;
	unsigned int bit;

	print_func();

	for (cirq_num = 0; cirq_num < CIRQ_IRQ_NUM; cirq_num++) {
		irq_num = CIRQ_TO_IRQ_NUM(cirq_num);

		if (cirq_num == 0 || irq_num % 32 == 0) {
			st = readl(IOMEM
				   (INT_POL_CTL0 + ((irq_num - GIC_PRIVATE_SIGNALS) / 32 * 4)));
			dbgmsg(CIRQ_LOG_LEVEL
			       "[CIRQ] clone_pol read pol 0x%08x at cirq %d (irq %d)\n", st,
			       cirq_num, irq_num);
		}

		bit = 0x1 << ((irq_num - GIC_PRIVATE_SIGNALS) % 32);

		if (st & bit) {
			mt_cirq_set_pol(cirq_num, MT_CIRQ_POL_NEG);
			dbgmsg(CIRQ_LOG_LEVEL "[CIRQ] clone_pol set cirq %d (irq %d) as negative\n",
			       cirq_num, irq_num);
		} else {
			mt_cirq_set_pol(cirq_num, MT_CIRQ_POL_POS);
			dbgmsg(CIRQ_LOG_LEVEL "[CIRQ] clone_pol set cirq %d (irq %d) as postive\n",
			       cirq_num, irq_num);
		}
	}

	return;
}

/*
 * mt_cirq_clone_sens: Copy the sensitivity setting from GIC to SYS_CIRQ
 */
static void mt_cirq_clone_sens(void)
{
	unsigned int cirq_num, irq_num;
	unsigned int st;
	unsigned int bit;

	print_func();

	for (cirq_num = 0; cirq_num < CIRQ_IRQ_NUM; cirq_num++) {
		irq_num = CIRQ_TO_IRQ_NUM(cirq_num);

		if (cirq_num == 0 || irq_num % 16 == 0) {
			st = readl(IOMEM(GIC_DIST_BASE + GIC_DIST_CONFIG + (irq_num / 16 * 4)));
			dbgmsg(CIRQ_LOG_LEVEL
			       "[CIRQ] clone_sens read sens 0x%08x at cirq %d (irq %d)\n", st,
			       cirq_num, irq_num);
		}

		bit = 0x2 << ((irq_num % 16) * 2);

		if (st & bit) {
			mt_cirq_set_sens(cirq_num, MT_EDGE_SENSITIVE);
			dbgmsg(CIRQ_LOG_LEVEL "[CIRQ] clone_sens set cirq %d (irq %d) as edge\n",
			       cirq_num, irq_num);
		} else {
			mt_cirq_set_sens(cirq_num, MT_LEVEL_SENSITIVE);
			dbgmsg(CIRQ_LOG_LEVEL "[CIRQ] clone_sens set cirq %d (irq %d) as level\n",
			       cirq_num, irq_num);
		}
	}

	return;
}

/*
 * mt_cirq_clone_mask: Copy the mask setting from GIC to SYS_CIRQ
 */
static void mt_cirq_clone_mask(void)
{
	unsigned int cirq_num, irq_num;
	unsigned int st;
	unsigned int bit;

	print_func();

	for (cirq_num = 0; cirq_num < CIRQ_IRQ_NUM; cirq_num++) {
		irq_num = CIRQ_TO_IRQ_NUM(cirq_num);

		if (cirq_num == 0 || irq_num % 32 == 0) {
			st = readl(IOMEM(GIC_DIST_BASE + GIC_DIST_ENABLE_SET + (irq_num / 32 * 4)));
			dbgmsg(CIRQ_LOG_LEVEL
			       "[CIRQ] clone_mask read enable 0x%08x at cirq %d (irq %d)\n", st,
			       cirq_num, irq_num);
		}

		bit = 0x1 << (irq_num % 32);

		if (st & bit) {
			mt_cirq_unmask(cirq_num);
			dbgmsg(CIRQ_LOG_LEVEL "[CIRQ] clone_mask unmask cirq %d (irq %d)\n",
			       cirq_num, irq_num);
		} else {
			mt_cirq_mask(cirq_num);
			dbgmsg(CIRQ_LOG_LEVEL "[CIRQ] clone_mask mask cirq %d (irq %d)\n", cirq_num,
			       irq_num);
		}
	}

	return;
}

/*
 * mt_cirq_clone_gic: Copy the setting from GIC to SYS_CIRQ
 */
void mt_cirq_clone_gic(void)
{
	mt_cirq_clone_pol();
	mt_cirq_clone_sens();
	mt_cirq_clone_mask();

	return;
}


#if defined(LDVT)
/*
 * cirq_dvt_show: To show usage.
 */
static ssize_t cirq_dvt_show(struct device_driver *driver, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "CIRQ dvt test\n");
}

/*
 * mci_dvt_store: To select mci test case.
 */
static ssize_t cirq_dvt_store(struct device_driver *driver, const char *buf, size_t count)
{
	char *p = (char *)buf;
	unsigned int num;

	num = simple_strtoul(p, &p, 10);
	switch (num) {
	case 1:
		mt_cirq_clone_gic();
		mt_cirq_dump_reg();
		break;
	case 2:
		mt_cirq_test();
		break;
	case 3:
		mt_cirq_disable();
		break;
	default:
		break;
	}

	return count;
}

DRIVER_ATTR(cirq_dvt, 0664, cirq_dvt_show, cirq_dvt_store);

#define __CHECK_IRQ_TYPE
#if defined(__CHECK_IRQ_TYPE)
#define X_DEFINE_IRQ(__name, __num, __polarity, __sensitivity) \
	{ .num = __num, .polarity = __polarity, .sensitivity = __sensitivity, },
#define L 0
#define H 1
#define EDGE MT_EDGE_SENSITIVE
#define LEVEL MT_LEVEL_SENSITIVE
struct __check_irq_type {
	int num;
	int polarity;
	int sensitivity;
};
struct __check_irq_type __check_irq_type[] = {
#include <mach/x_define_irq.h>
	{.num = -1,},
};

#undef X_DEFINE_IRQ
#undef L
#undef H
#undef EDGE
#undef LEVEL
#endif

void mt_cirq_dump_reg(void)
{
	int cirq_num;
	int pol, sens, mask;
	int irq_iter;

	printk(KERN_NOTICE "IRQ:\tPOL\tSENS\tMASK\n");
	for (cirq_num = 0; cirq_num < CIRQ_IRQ_NUM; cirq_num++) {
		pol = mt_cirq_get_pol(cirq_num);
		sens = mt_cirq_get_sens(cirq_num);
		mask = mt_cirq_get_mask(cirq_num);

#if defined(__CHECK_IRQ_TYPE)
		/* only check unmask irq */
		if (mask == 0) {
			for (irq_iter = 0; __check_irq_type[irq_iter].num >= 0; irq_iter++) {
				if (__check_irq_type[irq_iter].num == CIRQ_TO_IRQ_NUM(cirq_num)) {
					if (__check_irq_type[irq_iter].sensitivity != sens) {
						printk(KERN_NOTICE "[CIRQ] Error sens in irq:%d\n",
						       __check_irq_type[irq_iter].num);
					}
					if (__check_irq_type[irq_iter].polarity != pol) {
						printk(KERN_NOTICE
						       "[CIRQ] Error polarity in irq:%d\n",
						       __check_irq_type[irq_iter].num);
					}
					break;
				}
			}
		}
#endif

		printk(KERN_NOTICE "IRQ:%d\t%d\t%d\t%d\n", CIRQ_TO_IRQ_NUM(cirq_num), pol, sens,
		       mask);

	}
}

int mt_cirq_test(void)
{
	int cirq_num = 100;

	/* mt_cirq_enable(); */

	/*test polarity */
	mt_cirq_set_pol(cirq_num, MT_CIRQ_POL_NEG);
	if (mt_cirq_get_pol(cirq_num) != MT_CIRQ_POL_NEG)
		printk(KERN_NOTICE "mt_cirq_set_pol test failed!!\n");
	mt_cirq_set_pol(cirq_num, MT_CIRQ_POL_POS);
	if (mt_cirq_get_pol(cirq_num) != MT_CIRQ_POL_POS)
		printk(KERN_NOTICE "mt_cirq_set_pol test failed!!\n");

	/*test sensitivity */
	mt_cirq_set_sens(cirq_num, MT_EDGE_SENSITIVE);
	if (mt_cirq_get_sens(cirq_num) != MT_EDGE_SENSITIVE)
		printk(KERN_NOTICE "mt_cirq_set_sens test failed!!\n");
	mt_cirq_set_sens(cirq_num, MT_LEVEL_SENSITIVE);
	if (mt_cirq_get_sens(cirq_num) != MT_LEVEL_SENSITIVE)
		printk(KERN_NOTICE "mt_cirq_set_sens test failed!!\n");

	/*test mask */
	mt_cirq_mask(cirq_num);
	if (mt_cirq_get_mask(cirq_num) != 1)
		printk(KERN_NOTICE "mt_cirq_mask test failed!!\n");
	mt_cirq_unmask(cirq_num);
	if (mt_cirq_get_mask(cirq_num) != 0)
		printk(KERN_NOTICE "mt_cirq_unmask test failed!!\n");

	mt_cirq_clone_gic();
	mt_cirq_dump_reg();

	return 0;
}
#endif				/* !LDVT */

/*
 * cirq_irq_handler: SYS_CIRQ interrupt service routine.
 */
static irqreturn_t cirq_irq_handler(int irq, void *dev_id)
{
	printk(CIRQ_LOG_LEVEL "CIRQ_Handler\n");

	mt_cirq_ack_all();

	return IRQ_HANDLED;
}

/*
 * mt_cirq_init: SYS_CIRQ init function
 * always return 0
 */
static int __init mt_cirq_init(void)
{
	int ret;

	printk(CIRQ_LOG_LEVEL "CIRQ init...\n");

	if (request_irq(SYS_CIRQ_IRQ_BIT_ID, cirq_irq_handler, IRQF_TRIGGER_LOW, "CIRQ", NULL)) {
		printk(KERN_ERR "CIRQ IRQ LINE NOT AVAILABLE!!\n");
	} else {
		printk(CIRQ_LOG_LEVEL "CIRQ handler init success.\n");
	}

	ret = driver_register(&mt_cirq_drv.driver);
	if (ret == 0)
		printk(CIRQ_LOG_LEVEL "CIRQ init done...\n");

#ifdef LDVT
	ret = driver_create_file(&mt_cirq_drv.driver, &driver_attr_cirq_dvt);
	if (ret == 0)
		printk(CIRQ_LOG_LEVEL "CIRQ create sysfs file done...\n");
#endif

	return 0;
}
arch_initcall(mt_cirq_init);
EXPORT_SYMBOL(mt_cirq_enable);
EXPORT_SYMBOL(mt_cirq_disable);
EXPORT_SYMBOL(mt_cirq_clone_gic);
EXPORT_SYMBOL(mt_cirq_flush);
