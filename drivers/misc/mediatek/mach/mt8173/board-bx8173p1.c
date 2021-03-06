/* system header files */
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>
#include <linux/syscalls.h>
#include <linux/mtd/nand.h>
#include <linux/types.h>

#include <asm/irq.h>
#include <asm/io.h>
#ifndef CONFIG_ARM64
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#endif
#include <asm/mach/irq.h>
#include <asm/mach/map.h>
#include <asm/mach/time.h>
#include <asm/setup.h>

#include <mach/system.h>
#include <mach/board.h>
#include <mach/hardware.h>
#include <mach/mt_gpio.h>
#include <mach/mt_bt.h>
#include <mach/eint.h>
#include <mach/mtk_rtc.h>
#include <mach/mt_typedefs.h>
/* Fix-me: marked for early porting */
#include <cust_vibrator.h>
#include <bx8173p1/cust_gpio_usage.h>
#include <bx8173p1/cust_eint.h>
#include "bx8173p1/board-custom.h"

#if defined(CONFIG_MTK_COMBO) || defined(CONFIG_MTK_COMBO_MODULE)
#include <mach/mtk_wcn_cmb_stub.h>
#endif
#include <cust_leds.h>
#include <cust_leds_def.h>
#include <mach/mt_pwm.h>
#include <linux/kernel.h>
#include <mach/upmu_hw.h>
#define BACKLIGHT_LEVEL_PWM_64_FIFO_MODE_SUPPORT 64
#define BACKLIGHT_LEVEL_PWM_256_SUPPORT 256
#define BACKLIGHT_LEVEL_PWM_MODE_CONFIG BACKLIGHT_LEVEL_PWM_256_SUPPORT

#if defined(CONFIG_MTK_WCN_CMB_SDIO_SLOT)
static sdio_irq_handler_t mtk_wcn_cmb_sdio_eirq_handler;
int mtk_wcn_sdio_irq_flag_set(int falg);
static atomic_t sdio_irq_enable_flag;
static pm_callback_t mtk_wcn_cmb_sdio_pm_cb;
static void *mtk_wcn_cmb_sdio_pm_data;
static void *mtk_wcn_cmb_sdio_eirq_data;

const static u32 mtk_wcn_cmb_sdio_eint_pin = GPIO_WIFI_EINT_PIN;
const static u32 mtk_wcn_cmb_sdio_eint_num = CUST_EINT_WIFI_NUM;
const static u32 mtk_wcn_cmb_sdio_eint_m_eint = GPIO_WIFI_EINT_PIN_M_EINT;
const static u32 mtk_wcn_cmb_sdio_eint_m_gpio = GPIO_WIFI_EINT_PIN_M_GPIO;
/*
index: port number of combo chip (1:SDIO1, 2:SDIO2, no SDIO0)
value: slot power status of  (0:off, 1:on, 0xFF:invalid)
*/
#if (CONFIG_MTK_WCN_CMB_SDIO_SLOT == 0)
static unsigned char combo_port_pwr_map[4] = { 0x0, 0xFF, 0xFF, 0xFF };
#elif (CONFIG_MTK_WCN_CMB_SDIO_SLOT == 1)
static unsigned char combo_port_pwr_map[4] = { 0xFF, 0x0, 0xFF, 0xFF };
#elif (CONFIG_MTK_WCN_CMB_SDIO_SLOT == 2)
static unsigned char combo_port_pwr_map[4] = { 0xFF, 0xFF, 0x0, 0xFF };
#elif (CONFIG_MTK_WCN_CMB_SDIO_SLOT == 3)
static unsigned char combo_port_pwr_map[4] = { 0xFF, 0xFF, 0xFF, 0x0 };
#else
#error "unsupported CONFIG_MTK_WCN_CMB_SDIO_SLOT" CONFIG_MTK_WCN_CMB_SDIO_SLOT
#endif

#else
	/*standalone chip's structure should be add here */
#endif
/*=======================================================================*/
/* Board Specific Devices Power Management                               */
/*=======================================================================*/
extern kal_bool pmic_chrdet_status(void);

void mt_power_off(void)
{
	int bbpu_down = 0;
	pr_notice("mt_power_off\n");

#ifdef CONFIG_MTK_AUTO_POWER_ON_WITH_CHARGER
	if (pmic_chrdet_status() == KAL_TRUE)
		rtc_mark_enter_kpoc();
#endif

	/* pull PWRBB low */
	if (pmic_chrdet_status() == KAL_FALSE) {
		bbpu_down = 1;
		rtc_bbpu_power_down();
	}

	while (1) {
#if defined(CONFIG_POWER_EXT)
		/* EVB */
		pr_notice("EVB without charger\n");
#else
		pr_info("check charger\n");
		if (pmic_chrdet_status() == KAL_TRUE) {
#ifdef CONFIG_MTK_AUTO_POWER_ON_WITH_CHARGER
			arch_reset(0, "enter_kpoc");
#else
			arch_reset(0, "charger");
#endif
		} else if (bbpu_down == 0) {
			bbpu_down = 1;
			rtc_bbpu_power_down();
		}
#endif
	}
}

/*=======================================================================*/
/* Board Specific Devices                                                */
/*=======================================================================*/
/*GPS driver*/
/*FIXME: remove mt3326 notation */
struct mt3326_gps_hardware mt3326_gps_hw = {
	.ext_power_on = NULL,
	.ext_power_off = NULL,
};


#if defined(CONFIG_MTK_WCN_CMB_SDIO_SLOT)
static void mtk_wcn_cmb_sdio_enable_eirq(void)
{
#ifdef CONFIG_EINT_DEVICE_TREE
	if (!atomic_read(&wifi_irq_flag)) {
		printk(KERN_DEBUG "wifi eint has been enabled\n");
	} else {
		enable_irq(wifi_irq);
		atomic_dec(&wifi_irq_flag);
		/* printk(KERN_DEBUG " enable WIFI EINT irq %d !!\n",wifi_irq); */
	}
#else
	/* CUST_EINT_WIFI_NUM */
	mt_eint_unmask(mtk_wcn_cmb_sdio_eint_num);
#endif
}


static void mtk_wcn_cmb_sdio_disable_eirq(void)
{
#ifdef CONFIG_EINT_DEVICE_TREE
	if (atomic_read(&wifi_irq_flag)) {
		printk(KERN_DEBUG "wifi eint has been disabled!\n");
	} else {
		disable_irq_nosync(wifi_irq);
		atomic_inc(&wifi_irq_flag);
		/* printk(KERN_DEBUG "disable WIFI EINT irq %d !!\n",wifi_irq); */
	}
#else
	/* CUST_EINT_WIFI_NUM */
	mt_eint_mask(mtk_wcn_cmb_sdio_eint_num);
#endif
}

#ifdef CONFIG_EINT_DEVICE_TREE
irqreturn_t mtk_wcn_cmb_sdio_eirq_handler_stub(int irq, void *data)
{
	if ((NULL != mtk_wcn_cmb_sdio_eirq_handler) &&
		(0 != atomic_read(&sdio_irq_enable_flag))) {
		mtk_wcn_cmb_sdio_eirq_handler(mtk_wcn_cmb_sdio_eirq_data);
	}
	return IRQ_HANDLED;
}
#else
static void mtk_wcn_cmb_sdio_eirq_handler_stub(void)
{
	if ((NULL != mtk_wcn_cmb_sdio_eirq_handler) &&
		(0 != atomic_read(&sdio_irq_enable_flag))) {
		mtk_wcn_cmb_sdio_eirq_handler(mtk_wcn_cmb_sdio_eirq_data);
	}
}
#endif

static void mtk_wcn_cmb_sdio_request_eirq(sdio_irq_handler_t irq_handler, void *data)
{
#ifdef	CONFIG_EINT_DEVICE_TREE
	struct device_node *node;
	u32 ints[2] = {0, 0};
	int ret = -EINVAL;
#endif

	printk(KERN_INFO "enter %s\n", __func__);
	mtk_wcn_sdio_irq_flag_set(0);
	mtk_wcn_cmb_sdio_eirq_data    = data;
	mtk_wcn_cmb_sdio_eirq_handler = irq_handler;
	#if 1
#ifdef	CONFIG_EINT_DEVICE_TREE
	node = of_find_compatible_node(NULL, NULL, "mediatek, WIFI-eint");
	if (node) {
		of_property_read_u32_array(node, "debounce", ints, ARRAY_SIZE(ints));
		mt_gpio_set_debounce(ints[0], ints[1]);
		wifi_irq = irq_of_parse_and_map(node, 0);
		ret =
		    request_irq(wifi_irq, mtk_wcn_cmb_sdio_eirq_handler_stub, IRQF_TRIGGER_NONE,
				"WIFI-eint", NULL);
		printk(KERN_DEBUG "WIFI EINT irq %d !!\n", wifi_irq);
		atomic_set(&wifi_irq_flag, 0);/*default: irq enable*/

		if (ret)
			printk(KERN_ERR "WIFI EINT IRQ LINE NOT AVAILABLE!!\n");
		else {
			mtk_wcn_cmb_sdio_disable_eirq();/*not ,chip state is power off*/
		}
	} else
		printk(KERN_ERR "[%s] can't find wifi eint compatible node\n", __func__);
#else
	mt_eint_registration(mtk_wcn_cmb_sdio_eint_num,
		CUST_EINT_WIFI_TYPE, mtk_wcn_cmb_sdio_eirq_handler_stub, 0);
#endif
    #else
	{
		int i_ret = 0;
		i_ret = request_irq(mtk_wcn_cmb_sdio_eint_num,
				(irq_handler_t)mtk_wcn_cmb_sdio_eirq_handler_stub,
				IRQF_TRIGGER_LOW, "SDIO_EXT_IRQ", NULL);
		if (i_ret)
			printk(KERN_ERR "request_irq for SDIO ext IRQ failed, i_ret(%d)\n", i_ret);
		else
			printk(KERN_ERR "request_irq for SDIO ext IRQ succeed, i_ret(%d)\n", i_ret);
	}
    #endif

#ifdef	CONFIG_EINT_DEVICE_TREE
#else
	/*CUST_EINT_WIFI_NUM */
	mt_eint_mask(mtk_wcn_cmb_sdio_eint_num);
#endif
	printk(KERN_INFO "exit %s\n", __func__);
}

static void mtk_wcn_cmb_sdio_register_pm(pm_callback_t pm_cb, void *data)
{
	printk(KERN_INFO "mtk_wcn_cmb_sdio_register_pm (0x%p, 0x%p)\n", pm_cb, data);
	/* register pm change callback */
	mtk_wcn_cmb_sdio_pm_cb = pm_cb;
	mtk_wcn_cmb_sdio_pm_data = data;
}

static void mtk_wcn_cmb_sdio_on(int sdio_port_num)
{
	pm_message_t state = {.event = PM_EVENT_USER_RESUME };

	printk(KERN_INFO "mtk_wcn_cmb_sdio_on (%d)\n", sdio_port_num);

	/* 1. disable sdio eirq */
	mtk_wcn_cmb_sdio_disable_eirq();
	mt_set_gpio_pull_enable(mtk_wcn_cmb_sdio_eint_pin, GPIO_PULL_DISABLE);	/* GPIO_WIFI_EINT_PIN */
	mt_set_gpio_mode(mtk_wcn_cmb_sdio_eint_pin, mtk_wcn_cmb_sdio_eint_m_eint);	/* EINT mode */

	/* 2. call sd callback */
	if (mtk_wcn_cmb_sdio_pm_cb) {
		/* printk(KERN_INFO "mtk_wcn_cmb_sdio_pm_cb(PM_EVENT_USER_RESUME, 0x%p, 0x%p)\n", mtk_wcn_cmb_sdio_pm_cb, mtk_wcn_cmb_sdio_pm_data); */
		mtk_wcn_cmb_sdio_pm_cb(state, mtk_wcn_cmb_sdio_pm_data);
	} else {
		printk(KERN_WARNING "mtk_wcn_cmb_sdio_on no sd callback!!\n");
	}
}

static void mtk_wcn_cmb_sdio_off(int sdio_port_num)
{
	pm_message_t state = {.event = PM_EVENT_USER_SUSPEND };

	printk(KERN_INFO "mtk_wcn_cmb_sdio_off (%d)\n", sdio_port_num);

	/* 1. call sd callback */
	if (mtk_wcn_cmb_sdio_pm_cb) {
		/* printk(KERN_INFO "mtk_wcn_cmb_sdio_off(PM_EVENT_USER_SUSPEND, 0x%p, 0x%p)\n", mtk_wcn_cmb_sdio_pm_cb, mtk_wcn_cmb_sdio_pm_data); */
		mtk_wcn_cmb_sdio_pm_cb(state, mtk_wcn_cmb_sdio_pm_data);
	} else {
		printk(KERN_WARNING "mtk_wcn_cmb_sdio_off no sd callback!!\n");
	}

	/* 2. disable sdio eirq */
	mtk_wcn_cmb_sdio_disable_eirq();
	/*printk(KERN_INFO "[mt6620] set WIFI_EINT input pull down\n"); */
	mt_set_gpio_mode(mtk_wcn_cmb_sdio_eint_pin, mtk_wcn_cmb_sdio_eint_m_gpio);	/* GPIO mode */
	mt_set_gpio_dir(mtk_wcn_cmb_sdio_eint_pin, GPIO_DIR_IN);
	mt_set_gpio_pull_select(mtk_wcn_cmb_sdio_eint_pin, GPIO_PULL_UP);
	mt_set_gpio_pull_enable(mtk_wcn_cmb_sdio_eint_pin, GPIO_PULL_ENABLE);
}

int board_sdio_ctrl(unsigned int sdio_port_num, unsigned int on)
{
#if defined(CONFIG_MTK_WCN_CMB_SDIO_SLOT)
	sdio_port_num = CONFIG_MTK_WCN_CMB_SDIO_SLOT;
	printk(KERN_WARNING "mt_combo_sdio_ctrl: force set sdio port to (%d)\n", sdio_port_num);
#endif
	if ((sdio_port_num >= 4) || (combo_port_pwr_map[sdio_port_num] == 0xFF)) {
		/* invalid sdio port number or slot mapping */
		printk(KERN_WARNING "mt_mtk_wcn_cmb_sdio_ctrl invalid port(%d, %d)\n",
		       sdio_port_num, combo_port_pwr_map[sdio_port_num]);
		return -1;
	}
	/*printk(KERN_INFO "mt_mtk_wcn_cmb_sdio_ctrl (%d, %d)\n", sdio_port_num, on); */

	if (!combo_port_pwr_map[sdio_port_num] && on) {
#if 1
		printk(KERN_WARNING "board_sdio_ctrl force off before on\n");
		mtk_wcn_cmb_sdio_off(sdio_port_num);
#else
		printk(KERN_WARNING "skip sdio off before on\n");
#endif
		combo_port_pwr_map[sdio_port_num] = 0;
		/* off -> on */
		mtk_wcn_cmb_sdio_on(sdio_port_num);
		combo_port_pwr_map[sdio_port_num] = 1;
	} else if (combo_port_pwr_map[sdio_port_num] && !on) {
		/* on -> off */
		mtk_wcn_cmb_sdio_off(sdio_port_num);
		combo_port_pwr_map[sdio_port_num] = 0;
	} else {
		return -2;
	}
	return 0;
}
EXPORT_SYMBOL(board_sdio_ctrl);

int mtk_wcn_sdio_irq_flag_set(int flag)
{

	if (0 != flag) {
		atomic_set(&sdio_irq_enable_flag, 1);
	} else {
		atomic_set(&sdio_irq_enable_flag, 0);
	}
	printk(KERN_INFO "sdio_irq_enable_flag:%d\n", atomic_read(&sdio_irq_enable_flag));

	return atomic_read(&sdio_irq_enable_flag);
}
EXPORT_SYMBOL(mtk_wcn_sdio_irq_flag_set);

#endif				/* end of defined(CONFIG_MTK_WCN_CMB_SDIO_SLOT) */
/*=======================================================================*/
/* Board Specific Devices Init                                           */
/*=======================================================================*/

#if defined(CONFIG_WLAN)

int mt_wifi_resume(pm_message_t state)
{
	int evt = state.event;

	if (evt != PM_EVENT_USER_RESUME && evt != PM_EVENT_RESUME) {
		return -1;
	}

	/*printk(KERN_INFO "[WIFI] %s Resume\n", evt == PM_EVENT_RESUME ? "PM":"USR"); */
	return 0;
}

int mt_wifi_suspend(pm_message_t state)
{
	int evt = state.event;


	if (evt != PM_EVENT_USER_SUSPEND && evt != PM_EVENT_SUSPEND) {
		return -1;
	}

	return 0;
}

void mt_wifi_power_on(void)
{
	pm_message_t state = {.event = PM_EVENT_USER_RESUME };

	(void)mt_wifi_resume(state);
}
EXPORT_SYMBOL(mt_wifi_power_on);

void mt_wifi_power_off(void)
{
	pm_message_t state = {.event = PM_EVENT_USER_SUSPEND };

	(void)mt_wifi_suspend(state);
}
EXPORT_SYMBOL(mt_wifi_power_off);

#endif				/* end of defined(CONFIG_WLAN) */

/* Board Specific Devices                                                */
/*=======================================================================*/

/*=======================================================================*/
/* Board Specific Devices Init                                           */
/*=======================================================================*/

/*=======================================================================*/
/* Board Devices Capability                                              */
/*=======================================================================*/
#define MSDC_SDIO_FLAG    (MSDC_EXT_SDIO_IRQ | MSDC_HIGHSPEED | MSDC_UHS1)

struct msdc_hw msdc0_hw = {
	.clk_src = MSDC_CLKSRC_400MHZ,
	.cmd_edge = MSDC_SMPL_FALLING,
	.rdata_edge = MSDC_SMPL_FALLING,
	.wdata_edge = MSDC_SMPL_FALLING,
	.clk_drv = 2,
	.cmd_drv = 2,
	.dat_drv = 2,
	.rst_drv = 2,
	.ds_drv = 2,
	.data_pins = 8,
	.data_offset = 0,
#ifndef CONFIG_MTK_EMMC_CACHE
	.flags = MSDC_SYS_SUSPEND | MSDC_HIGHSPEED | MSDC_UHS1 | MSDC_DDR | MSDC_HS400,
#else
	.flags = MSDC_SYS_SUSPEND | MSDC_HIGHSPEED | MSDC_CACHE | MSDC_UHS1 | MSDC_DDR | MSDC_HS400,
#endif
	.dat0rddly = 0,
	.dat1rddly = 0,
	.dat2rddly = 0,
	.dat3rddly = 0,
	.dat4rddly = 0,
	.dat5rddly = 0,
	.dat6rddly = 0,
	.dat7rddly = 0,
	.datwrddly = 0,
	.cmdrrddly = 0,
	.cmdrddly = 0,
	.host_function = MSDC_EMMC,
	.boot = MSDC_BOOT_EN,
};

struct msdc_hw msdc1_hw = {
	.clk_src = MSDC_CLKSRC_200MHZ,
	.cmd_edge = MSDC_SMPL_FALLING,
	.rdata_edge = MSDC_SMPL_FALLING,
	.wdata_edge = MSDC_SMPL_FALLING,
	.clk_drv = 2,
	.cmd_drv = 2,
	.dat_drv = 2,
	.clk_drv_sd_18 = 3,	/* sdr104 mode */
	.cmd_drv_sd_18 = 2,
	.dat_drv_sd_18 = 2,
	.clk_drv_sd_18_sdr50 = 2,	/* sdr50 mode */
	.cmd_drv_sd_18_sdr50 = 2,
	.dat_drv_sd_18_sdr50 = 2,
	.clk_drv_sd_18_ddr50 = 2,	/* ddr50 mode */
	.cmd_drv_sd_18_ddr50 = 2,
	.dat_drv_sd_18_ddr50 = 2,
	.data_pins = 4,
	.data_offset = 0,
	.flags =
	    MSDC_SYS_SUSPEND | MSDC_CD_PIN_EN | MSDC_REMOVABLE | MSDC_HIGHSPEED |
	    MSDC_UHS1 | MSDC_DDR,
	.dat0rddly = 0,
	.dat1rddly = 0,
	.dat2rddly = 0,
	.dat3rddly = 0,
	.dat4rddly = 0,
	.dat5rddly = 0,
	.dat6rddly = 0,
	.dat7rddly = 0,
	.datwrddly = 0,
	.cmdrrddly = 0,
	.cmdrddly = 0,
	.host_function = MSDC_SD,
	.boot = 0,
	.cd_level = MSDC_CD_LOW,
};

struct msdc_hw msdc2_hw = {
	.clk_src = MSDC_CLKSRC_200MHZ,
	.cmd_edge = MSDC_SMPL_FALLING,
	.rdata_edge = MSDC_SMPL_FALLING,
	.wdata_edge = MSDC_SMPL_FALLING,
	.clk_drv = 1,
	.cmd_drv = 1,
	.dat_drv = 1,
	.clk_drv_sd_18 = 2,	/* sdr104 mode */
	.cmd_drv_sd_18 = 2,
	.dat_drv_sd_18 = 2,
	.clk_drv_sd_18_sdr50 = 2,	/* sdr50 mode */
	.cmd_drv_sd_18_sdr50 = 2,
	.dat_drv_sd_18_sdr50 = 2,
	.clk_drv_sd_18_ddr50 = 2,	/* ddr50 mode */
	.cmd_drv_sd_18_ddr50 = 2,
	.dat_drv_sd_18_ddr50 = 2,
	.data_pins = 4,
	.data_offset = 0,
	.flags =
	    MSDC_SYS_SUSPEND | MSDC_CD_PIN_EN | MSDC_REMOVABLE | MSDC_HIGHSPEED |
	    MSDC_UHS1 | MSDC_DDR,
	.dat0rddly = 0,
	.dat1rddly = 0,
	.dat2rddly = 0,
	.dat3rddly = 0,
	.dat4rddly = 0,
	.dat5rddly = 0,
	.dat6rddly = 0,
	.dat7rddly = 0,
	.datwrddly = 0,
	.cmdrrddly = 0,
	.cmdrddly = 0,
	.host_function = MSDC_SD,
	.boot = 0,
	.cd_level = MSDC_CD_LOW,
};

/* MSDC3 settings for MT66xx combo connectivity chip */
struct msdc_hw msdc3_hw = {
	.clk_src = MSDC_CLKSRC_200MHZ,
	.cmd_edge = MSDC_SMPL_FALLING,
	.rdata_edge = MSDC_SMPL_FALLING,
	.wdata_edge = MSDC_SMPL_FALLING,
	.clk_drv = 3,
	.cmd_drv = 3,
	.dat_drv = 3,
	.data_pins = 4,
	.data_offset = 0,
	/* MT6620 use External IRQ, wifi uses high speed. here wifi manage his own suspend and resume, does not support hot plug */
	.flags = MSDC_SDIO_FLAG,	/* MSDC_SYS_SUSPEND | MSDC_WP_PIN_EN | MSDC_CD_PIN_EN | MSDC_REMOVABLE, */
	.dat0rddly = 0,
	.dat1rddly = 0,
	.dat2rddly = 0,
	.dat3rddly = 0,
	.dat4rddly = 0,
	.dat5rddly = 0,
	.dat6rddly = 0,
	.dat7rddly = 0,
	.datwrddly = 0,
	.cmdrrddly = 0,
	.cmdrddly = 0,
	.cmdrtactr_sdr50 = 0x1,
	.wdatcrctactr_sdr50 = 0x1,
	.intdatlatcksel_sdr50 = 0x0,
	.cmdrtactr_sdr200 = 0x3,
	.wdatcrctactr_sdr200 = 0x3,
	.intdatlatcksel_sdr200 = 0x0,
	.host_function = MSDC_SDIO,
	.boot = 0,
	.request_sdio_eirq = NULL,
	.enable_sdio_eirq = NULL,
	.disable_sdio_eirq = NULL,
	.register_pm = NULL,
};

/* MT6575 NAND Driver */
#if defined(CONFIG_MTK_MTD_NAND)
struct mt6575_nand_host_hw mt6575_nand_hw = {
	.nfi_bus_width = 8,
	.nfi_access_timing = NFI_DEFAULT_ACCESS_TIMING,
	.nfi_cs_num = NFI_CS_NUM,
	.nand_sec_size = 512,
	.nand_sec_shift = 9,
	.nand_ecc_size = 2048,
	.nand_ecc_bytes = 32,
	.nand_ecc_mode = NAND_ECC_HW,
};
#endif
unsigned int Cust_GetBacklightLevelSupport_byPWM(void)
{
	return BACKLIGHT_LEVEL_PWM_MODE_CONFIG;
}

unsigned int brightness_mapping(unsigned int level)
{
	unsigned int mapped_level;

	mapped_level = level;

	return mapped_level;
}

/*

 * To explain How to set these para for cust_led_list[] of led/backlight
 * "name" para: led or backlight
 * "mode" para:which mode for led/backlight
 *	such as:
 *			MT65XX_LED_MODE_NONE,
 *			MT65XX_LED_MODE_PWM,
 *			MT65XX_LED_MODE_GPIO,
 *			MT65XX_LED_MODE_PMIC,
 *			MT65XX_LED_MODE_CUST_LCM,
 *			MT65XX_LED_MODE_CUST_BLS_PWM
 *
 *"data" para: control methord for led/backlight
 *   such as:
 *			MT65XX_LED_PMIC_LCD_ISINK=0,
 *			MT65XX_LED_PMIC_NLED_ISINK0,
 *			MT65XX_LED_PMIC_NLED_ISINK1,
 *			MT65XX_LED_PMIC_NLED_ISINK2,
 *			MT65XX_LED_PMIC_NLED_ISINK3
 *
 *"PWM_config" para:PWM(AP side Or BLS module), by default setting{0,0,0,0,0} Or {0}
 *struct PWM_config {
 *  int clock_source;
 *  int div;
 *  int low_duration;
 *  int High_duration;
 *  BOOL pmic_pad;//AP side PWM pin in PMIC chip (only 89 needs confirm); 1:yes 0:no(default)
 *};
 *-------------------------------------------------------------------------------------------
 *   for AP PWM setting as follow:
 *1.	 PWM config data
 *  clock_source: clock source frequency, can be 0/1
 *  div: clock division, can be any value within 0~7 (i.e. 1/2^(div) = /1, /2, /4, /8, /16, /32, /64, /128)
 *  low_duration: only for BACKLIGHT_LEVEL_PWM_64_FIFO_MODE_SUPPORT
 *  High_duration: only for BACKLIGHT_LEVEL_PWM_64_FIFO_MODE_SUPPORT
 *
 *2.	 PWM freq.
 * If BACKLIGHT_LEVEL_PWM_MODE_CONFIG = BACKLIGHT_LEVEL_PWM_256_SUPPORT,
 *	 PWM freq. = clock source / 2^(div) / 256
 *
 * If BACKLIGHT_LEVEL_PWM_MODE_CONFIG = BACKLIGHT_LEVEL_PWM_64_FIFO_MODE_SUPPORT,
 *	 PWM freq. = clock source / 2^(div) / [(High_duration+1)(Level')+(low_duration+1)(64 - Level')]
 *	           = clock source / 2^(div) / [(High_duration+1)*64]     (when low_duration = High_duration)
 *Clock source:
 *	 0: block clock/1625 = 26M/1625 = 16K (MT6571)
 *	 1: block clock = 26M (MT6571)
 *Div: 0~7
 *
 *For example, in MT6571, PWM_config = {1,1,0,0,0}
 *	 ==> PWM freq. = 26M/2^1/256	 =	50.78 KHz ( when BACKLIGHT_LEVEL_PWM_256_SUPPORT )
 *	 ==> PWM freq. = 26M/2^1/(0+1)*64 = 203.13 KHz ( when BACKLIGHT_LEVEL_PWM_64_FIFO_MODE_SUPPORT )
 *-------------------------------------------------------------------------------------------
 *   for BLS PWM setting as follow:
 *1.	 PWM config data
 *	 clock_source: clock source frequency, can be 0/1/2/3
 *	 div: clock division, can be any value within 0~1023
 *	 low_duration: non-use
 *	 High_duration: non-use
 *	 pmic_pad: non-use
 *
 *2.	 PWM freq.= clock source / (div + 1) /1024
 *Clock source:
 *	 0: 26 MHz
 *	 1: 104 MHz
 *	 2: 124.8 MHz
 *	 3: 156 MHz
 *Div: 0~1023
 *
 *By default, clock_source = 0 and div = 0 => PWM freq. = 26 KHz
 *-------------------------------------------------------------------------------------------
 */
static struct cust_mt65xx_led cust_led_list[MT65XX_LED_TYPE_TOTAL] = {
	/* have no hw, add as blue for android app need */
	{"red", MT65XX_LED_MODE_NONE, -1, {0} },
	{"green", MT65XX_LED_MODE_NONE, -1, {0} },
	{"blue", MT65XX_LED_MODE_NONE, -1, {0} },
	{"jogball-backlight", MT65XX_LED_MODE_NONE, -1, {0} },
	{"keyboard-backlight", MT65XX_LED_MODE_NONE, -1, {0} },
	{"button-backlight", MT65XX_LED_MODE_NONE, -1, {0} },
	{"lcd-backlight", MT65XX_LED_MODE_NONE, -1, {0} },
};

struct cust_mt65xx_led *get_cust_led_list(void)
{
	return cust_led_list;
}

/* ----------------Vibrator Custom Configs----------------------*/
static struct vibrator_hw cust_vibrator_hw = {
	.vib_timer = 25,
#ifdef CUST_VIBR_LIMIT
	.vib_limit = 9,
#endif
#ifdef CUST_VIBR_VOL
	.vib_vol = 0x5,		/* 2.8V for vibr */
#endif
};

struct vibrator_hw *get_cust_vibrator_hw(void)
{
	return &cust_vibrator_hw;
}
/* ----------------Vibrator Custom Configs----------------------*/



#ifdef CONFIG_MTK_KEYPAD
#include <mach/hal_pub_kpd.h>
#include <uapi/linux/input.h>
#include <arch/arm64/boot/dts/mt8173-pinfunc.h>


#define KPD_KEY_DEBOUNCE  1024      /* (val / 32) ms */
#define KPD_PWRKEY_MAP    KEY_POWER
#define MTK_PMIC_PWR_KEY    8

/* HW keycode [0 ~ 71] -> Linux keycode */
#define KPD_INIT_KEYMAP()	\
{	\
	[0] = KEY_VOLUMEUP,		\
	[1] = KEY_VOLUMEDOWN,		\
}


static u16 kpd_keymap[KPD_NUM_KEYS] = KPD_INIT_KEYMAP();

struct mtk_kpd_hardware mtk_kpd_hw = {
	.kpd_init_keymap = kpd_keymap,
	.kpd_pwrkey_map = KEY_POWER,
	.kpd_key_debounce = KPD_KEY_DEBOUNCE,
};

#endif
