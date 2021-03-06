/*
 * max97236.c  --  Audio Amplifier with Jack Detection for max97236
 *
 * Copyright (C) 2013 MM Solutions AD
 *
 * Author: Plamen Valev <pvalev@mm-sol.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _MAX97236_H__
#define _MAX97236_H__

/*
 * Driver revision
 */
#define MAX97236_REVISION			"0.01.00"

/*
 * MAX97236 Register Definitions
 */

#define MAX97236_STATUS_1			0x00
#define MAX97236_STATUS_2			0x01
#define MAX97236_STATUS_3			0x02
#define MAX97236_EXPANSION_1			0x03
#define MAX97236_IRQ_MASK_1			0x04
#define MAX97236_IRQ_MASK_2			0x05
#define MAX97236_EXPANSION_2			0x06
#define MAX97236_LEFT_VOLUME			0x07
#define MAX97236_RIGHT_VOLUME			0x08
#define MAX97236_MICROPHONE			0x09
#define MAX97236_EXPANSION_3			0x0A
#define MAX97236_VENDOR_ID			0x0B
#define MAX97236_EXPANSION_4			0x0D
#define MAX97236_EXPANSION_5			0x0E
#define MAX97236_EXPANSION_6			0x0F
#define MAX97236_EXPANSION_7			0x10
#define MAX97236_EXPANSION_8			0x11
#define MAX97236_KEYSCAN_CLK_DIV_1		0x12
#define MAX97236_KEYSCAN_CLK_DIV_2		0x13
#define MAX97236_KEYSCAN_CLK_DIV_ADC		0x14
#define MAX97236_KEYSCAN_DEBOUNCE		0x15
#define MAX97236_KEYSCAN_DELAY			0x16
#define MAX97236_PASSIVE_MBH_KEYSCAN_DATA	0x17
#define MAX97236_DC_TEST_SLEW_CONTROL		0x18
#define MAX97236_STATE_FORCING			0x19
#define MAX97236_AC_TEST_CONTROL		0x1A
#define MAX97236_EXPANSION_9			0x1B
#define MAX97236_EXPANSION_10			0x1C
#define MAX97236_ENABLE_1			0x1D
#define MAX97236_ENABLE_2			0x1E

#define MAX97236_PROGRAM_DETECTION		0x1F
#define MAX97236_CUSTOM_DETECT_ENABLE		0x20
#define MAX97236_DETECTION_RESULTS_1		0x21
#define MAX97236_DETECTION_RESULTS_2		0x22
#define MAX97236_RUN_CUSTOM_DETECT		0x23

#define MAX97236_MAX_REG			0x23

/* MAX97236 Register Bit Fields start */
/*
 * MAX97236_STATUS_1
 */
#define MAX97236_STATUS_1_JKIN_MASK		(1 << 7)
#define MAX97236_STATUS_1_JKIN_BIT		(1 << 7)
#define MAX97236_STATUS_1_JKIN_SHIFT		7
#define MAX97236_STATUS_1_JKIN_WIDTH		1
#define MAX97236_STATUS_1_DDONE_MASK		(1 << 6)
#define MAX97236_STATUS_1_DDONE_BIT		(1 << 6)
#define MAX97236_STATUS_1_DDONE_SHIFT		6
#define MAX97236_STATUS_1_DDONE_WIDTH		1
#define MAX97236_STATUS_1_VOL_MASK		(1 << 5)
#define MAX97236_STATUS_1_VOL_BIT		(1 << 5)
#define MAX97236_STATUS_1_VOL_SHIFT		5
#define MAX97236_STATUS_1_VOL_WIDTH		1
#define MAX97236_STATUS_1_MICIN_MASK		(1 << 3)
#define MAX97236_STATUS_1_MICIN_BIT		(1 << 3)
#define MAX97236_STATUS_1_MICIN_SHIFT		3
#define MAX97236_STATUS_1_MICIN_WIDTH		1
#define MAX97236_STATUS_1_JACKSW_MASK		(1 << 2)
#define MAX97236_STATUS_1_JACKSW_BIT		(1 << 2)
#define MAX97236_STATUS_1_JACKSW_SHIFT		2
#define MAX97236_STATUS_1_JACKSW_WIDTH		1
#define MAX97236_STATUS_1_MCSW_MASK		(1 << 1)
#define MAX97236_STATUS_1_MCSW_BIT		(1 << 1)
#define MAX97236_STATUS_1_MCSW_SHIFT		1
#define MAX97236_STATUS_1_MCSW_WIDTH		1
#define MAX97236_STATUS_1_MBH_MASK		(1 << 0)
#define MAX97236_STATUS_1_MBH_BIT		(1 << 0)
#define MAX97236_STATUS_1_MBH_SHIFT		0
#define MAX97236_STATUS_1_MBH_WIDTH		1

/*
 * MAX97236_STATUS_2
 */
#define MAX97236_STATUS_2_LINEL_MASK		(1 << 7)
#define MAX97236_STATUS_2_LINEL_BIT		(1 << 7)
#define MAX97236_STATUS_2_LINEL_SHIFT		7
#define MAX97236_STATUS_2_LINEL_WIDTH		1
#define MAX97236_STATUS_2_LINER_MASK		(1 << 6)
#define MAX97236_STATUS_2_LINER_BIT		(1 << 6)
#define MAX97236_STATUS_2_LINER_SHIFT		6
#define MAX97236_STATUS_2_LINER_WIDTH		1
#define MAX97236_STATUS_2_HPL_MASK		(1 << 5)
#define MAX97236_STATUS_2_HPL_BIT		(1 << 5)
#define MAX97236_STATUS_2_HPL_SHIFT		5
#define MAX97236_STATUS_2_HPL_WIDTH		1
#define MAX97236_STATUS_2_HPR_MASK		(1 << 4)
#define MAX97236_STATUS_2_HPR_BIT		(1 << 4)
#define MAX97236_STATUS_2_HPR_SHIFT		4
#define MAX97236_STATUS_2_HPR_WIDTH		1
#define MAX97236_STATUS_2_JACKSWINC_MASK	(1 << 3)
#define MAX97236_STATUS_2_JACKSWINC_BIT		(1 << 3)
#define MAX97236_STATUS_2_JACKSWINC_SHIFT	3
#define MAX97236_STATUS_2_JACKSWINC_WIDTH	1
#define MAX97236_STATUS_2_KEY_MASK		(1 << 2)
#define MAX97236_STATUS_2_KEY_BIT		(1 << 2)
#define MAX97236_STATUS_2_KEY_SHIFT		2
#define MAX97236_STATUS_2_KEY_WIDTH		1

/*
 * MAX97236_STATUS_3
 */
#define MAX97236_STATUS_3_GND_MASK		(0x03 << 0)
#define MAX97236_STATUS_3_GND_SHIFT		0
#define MAX97236_STATUS_3_GND_WIDTH		2

/*
 * MAX97236_IRQ_MASK_1
 */
#define MAX97236_IRQ_MASK_1_IJKIN_MASK		(1 << 7)
#define MAX97236_IRQ_MASK_1_IJKIN_BIT		(1 << 7)
#define MAX97236_IRQ_MASK_1_IJKIN_SHIFT		7
#define MAX97236_IRQ_MASK_1_IJKIN_WIDTH		1
#define MAX97236_IRQ_MASK_1_IDDONE_MASK		(1 << 6)
#define MAX97236_IRQ_MASK_1_IDDONE_BIT		(1 << 6)
#define MAX97236_IRQ_MASK_1_IDDONE_SHIFT	6
#define MAX97236_IRQ_MASK_1_IDDONE_WIDTH	1
#define MAX97236_IRQ_MASK_1_IVOL_MASK		(1 << 5)
#define MAX97236_IRQ_MASK_1_IVOL_BIT		(1 << 5)
#define MAX97236_IRQ_MASK_1_IVOL_SHIFT		5
#define MAX97236_IRQ_MASK_1_IVOL_WIDTH		1
#define MAX97236_IRQ_MASK_1_IMIC_MASK		(1 << 3)
#define MAX97236_IRQ_MASK_1_IMIC_BIT		(1 << 3)
#define MAX97236_IRQ_MASK_1_IMIC_SHIFT		3
#define MAX97236_IRQ_MASK_1_IMIC_WIDTH		1
#define MAX97236_IRQ_MASK_1_JACKSW_MASK		(1 << 2)
#define MAX97236_IRQ_MASK_1_JACKSW_BIT		(1 << 2)
#define MAX97236_IRQ_MASK_1_JACKSW_SHIFT	2
#define MAX97236_IRQ_MASK_1_JACKSW_WIDTH	1
#define MAX97236_IRQ_MASK_1_IMCSW_MASK		(1 << 1)
#define MAX97236_IRQ_MASK_1_IMCSW_BIT		(1 << 1)
#define MAX97236_IRQ_MASK_1_IMCSW_SHIFT		1
#define MAX97236_IRQ_MASK_1_IMCSW_WIDTH		1
#define MAX97236_IRQ_MASK_1_IMBH_MASK		(1 << 0)
#define MAX97236_IRQ_MASK_1_IMBH_BIT		(1 << 0)
#define MAX97236_IRQ_MASK_1_IMBH_SHIFT		0
#define MAX97236_IRQ_MASK_1_IMBH_WIDTH		1

/*
 * MAX97236_IRQ_MASK_2
 */
#define MAX97236_IRQ_MASK_2_ILINEL_MASK		(1 << 7)
#define MAX97236_IRQ_MASK_2_ILINEL_BIT		(1 << 7)
#define MAX97236_IRQ_MASK_2_ILINEL_SHIFT	7
#define MAX97236_IRQ_MASK_2_ILINEL_WIDTH	1
#define MAX97236_IRQ_MASK_2_ILINER_MASK		(1 << 6)
#define MAX97236_IRQ_MASK_2_ILINER_BIT		(1 << 6)
#define MAX97236_IRQ_MASK_2_ILINER_SHIFT	6
#define MAX97236_IRQ_MASK_2_ILINER_WIDTH	1
#define MAX97236_IRQ_MASK_2_IHPL_MASK		(1 << 5)
#define MAX97236_IRQ_MASK_2_IHPL_BIT		(1 << 5)
#define MAX97236_IRQ_MASK_2_IHPL_SHIFT		5
#define MAX97236_IRQ_MASK_2_IHPL_WIDTH		1
#define MAX97236_IRQ_MASK_2_IHPR_MASK		(1 << 4)
#define MAX97236_IRQ_MASK_2_IHPR_BIT		(1 << 4)
#define MAX97236_IRQ_MASK_2_IHPR_SHIFT		4
#define MAX97236_IRQ_MASK_2_IHPR_WIDTH		1
#define MAX97236_IRQ_MASK_2_IJACKSW_MASK	(1 << 3)
#define MAX97236_IRQ_MASK_2_IJACKSW_BIT		(1 << 3)
#define MAX97236_IRQ_MASK_2_IJACKSW_SHIFT	3
#define MAX97236_IRQ_MASK_2_IJACKSW_WIDTH	1
#define MAX97236_IRQ_MASK_2_IKEY_MASK		(1 << 2)
#define MAX97236_IRQ_MASK_2_IKEY_BIT		(1 << 2)
#define MAX97236_IRQ_MASK_2_IKEY_SHIFT		2
#define MAX97236_IRQ_MASK_2_IKEY_WIDTH		1

/*
 * MAX97236_LEFT_VOLUME
 */
#define MAX97236_LEFT_VOLUME_LR_MASK		(1 << 7)
#define MAX97236_LEFT_VOLUME_LR_SHIFT		7
#define MAX97236_LEFT_VOLUME_LR_WIDTH		1
#define MAX97236_LEFT_VOLUME_LR_BIT		(1 << 7)
#define MAX97236_LEFT_VOLUME_MUTEL_MASK		(1 << 6)
#define MAX97236_LEFT_VOLUME_MUTEL_BIT		(1 << 6)
#define MAX97236_LEFT_VOLUME_MUTEL_SHIFT	6
#define MAX97236_LEFT_VOLUME_MUTEL_WIDTH	1
#define MAX97236_LEFT_VOLUME_LVOL_MASK		0x3F
#define MAX97236_LEFT_VOLUME_LVOL_SHIFT		0
#define MAX97236_LEFT_VOLUME_LVOL_WIDTH		6

/*
 * MAX97236_RIGHT_VOLUME
 */
#define MAX97236_RIGHT_VOLUME_MUTER_MASK	(1 << 6)
#define MAX97236_RIGHT_VOLUME_MUTER_BIT		(1 << 6)
#define MAX97236_RIGHT_VOLUME_MUTER_SHIFT	6
#define MAX97236_RIGHT_VOLUME_MUTER_WIDTH	1
#define MAX97236_RIGHT_VOLUME_RVOL_MASK		0x3F
#define MAX97236_RIGHT_VOLUME_RVOL_SHIFT	0
#define MAX97236_RIGHT_VOLUME_RVOL_WIDTH	6

/*
 * MAX97236_MICROPHONE
 */
#define MAX97236_MICROPHONE_GAIN_MASK		(1 << 6)
#define MAX97236_MICROPHONE_GAIN_SHIFT		6
#define MAX97236_MICROPHONE_GAIN_WIDTH		1
#define MAX97236_MICROPHONE_MICR_MASK		(7 << 3)
#define MAX97236_MICROPHONE_MICR_SHIFT		3
#define MAX97236_MICROPHONE_MICR_WIDTH		3
#define MAX97236_MICROPHONE_BIAS_MASK		(1 << 2)
#define MAX97236_MICROPHONE_BIAS_BIT		(1 << 2)
#define MAX97236_MICROPHONE_BIAS_SHIFT		2
#define MAX97236_MICROPHONE_BIAS_WIDTH		1

/*
 * MAX97236_VENDOR_ID
 */
#define MAX97236_VENDOR_ID_MASK			(0x0F << 4)
#define MAX97236_VENDOR_ID_SHIFT		4
#define MAX97236_VENDOR_ID_WIDTH		4

/*
 * MAX97236_KEYSCAN_CLK_DIV_1
 */
#define MAX97236_KEYSCAN_KEY_DIV_HIGH_MASK	0xff
#define MAX97236_KEYSCAN_KEY_DIV_HIGH_SHIFT	0
#define MAX97236_KEYSCAN_KEY_DIV_HIGH_WIDTH	8

/*
 * MAX97236_KEYSCAN_CLK_DIV_2
 */
#define MAX97236_KEYSCAN_KEY_DIV_LOW_MASK	0xff
#define MAX97236_KEYSCAN_KEY_DIV_LOW_SHIFT	0
#define MAX97236_KEYSCAN_KEY_DIV_LOW_WIDTH	8

/*
 * MAX97236_KEYSCAN_CLK_DIV_ADC
 */
#define MAX97236_KEYSCAN_KEY_DIV_ADC_MASK	0xff
#define MAX97236_KEYSCAN_KEY_DIV_ADC_SHIFT	0
#define MAX97236_KEYSCAN_KEY_DIV_ADC_WIDTH	8

/*
 * MAX97236_KEYSCAN_DEBOUNCE
 */
#define MAX97236_KEYSCAN_KEY_DEB_MASK		0xff
#define MAX97236_KEYSCAN_KEY_DEB_SHIFT		0
#define MAX97236_KEYSCAN_KEY_DEB_WIDTH		8

/*
 * MAX97236_KEYSCAN_DELAY
 */
#define MAX97236_KEYSCAN_KEY_DEL_MASK		0xff
#define MAX97236_KEYSCAN_KEY_DEL_SHIFT		0
#define MAX97236_KEYSCAN_KEY_DEL_WIDTH		8

/*
 * MAX97236_PASSIVE_MBH_KEYSCAN_DATA
 */
#define MAX97236_PASSIVE_MBH_PRESS_MASK		(1 << 7)
#define MAX97236_PASSIVE_MBH_PRESS_BIT		(1 << 7)
#define MAX97236_PASSIVE_MBH_PRESS_SHIFT	7
#define MAX97236_PASSIVE_MBH_PRESS_WIDTH	1
#define MAX97236_PASSIVE_MBH_RANGE_MASK		(1 << 6)
#define MAX97236_PASSIVE_MBH_RANGE_BIT		(1 << 6)
#define MAX97236_PASSIVE_MBH_RANGE_SHIFT	6
#define MAX97236_PASSIVE_MBH_RANGE_WIDTH	1
#define MAX97236_PASSIVE_MBH_KEYDATA_MASK	(0x3F << 0)
#define MAX97236_PASSIVE_MBH_KEYDATA_SHIFT	0
#define MAX97236_PASSIVE_MBH_KEYDATA_WIDTH	6

#define	MAX97236_KEYDATA_THRESH_0		8
#define	MAX97236_KEYDATA_THRESH_1		16
#define	MAX97236_KEYDATA_THRESH_2		24
#define	MAX97236_KEYDATA_THRESH_3		31

/*
 * MAX97236_DC_TEST_SLEW_CONTROL
 */
#define MAX97236_DC_SLEW_MASK			0xff
#define MAX97236_DC_SLEW_SHIFT			0
#define MAX97236_DC_SLEW_WIDTH			8

/*
 * MAX97236_STATE_FORCING
 */
#define MAX97236_STATE_FORCING_FORCE_MASK	(1 << 5)
#define MAX97236_STATE_FORCING_FORCE_BIT	(1 << 5)
#define MAX97236_STATE_FORCING_FORCE_SHIFT	5
#define MAX97236_STATE_FORCING_FORCE_WIDTH	1
#define MAX97236_STATE_FORCING_STATE_MASK	(0x1F << 0)
#define MAX97236_STATE_FORCING_STATE_SHIFT	0
#define MAX97236_STATE_FORCING_STATE_WIDTH	5

#define MAX97236_STATE_FLOAT			0x01
#define MAX97236_STATE_LRGG_DC			0x12
#define MAX97236_STATE_LRGM			0x02
#define MAX97236_STATE_LRMG			0x03

/*
 * MAX97236_AC_TEST_CONTROL
 */
#define MAX97236_AC_TEST_AC_REPEAT_MASK		(0x03 << 4)
#define MAX97236_AC_TEST_AC_REPEAT_SHIFT	4
#define MAX97236_AC_TEST_AC_REPEAT_WIDTH	2
#define MAX97236_AC_TEST_PULSE_WIDTH_MASK	(0x03 << 2)
#define MAX97236_AC_TEST_PULSE_WIDTH_SHIFT	2
#define MAX97236_AC_TEST_PULSE_WIDTH_WIDTH	2
#define MAX97236_AC_TEST_PULSE_AMP_MASK		(0x03 << 0)
#define MAX97236_AC_TEST_PULSE_AMP_SHIFT	0
#define MAX97236_AC_TEST_PULSE_AMP_WIDTH	2

/*
 * MAX97236_ENABLE_1
 */
#define MAX97236_ENABLE_1_SHDN_MASK		(1 << 7)
#define MAX97236_ENABLE_1_SHDN_BIT		(1 << 7)
#define MAX97236_ENABLE_1_SHDN_SHIFT		7
#define MAX97236_ENABLE_1_SHDN_WIDTH		1
#define MAX97236_ENABLE_1_RESET_MASK		(1 << 6)
#define MAX97236_ENABLE_1_RESET_BIT		(1 << 6)
#define MAX97236_ENABLE_1_RESET_SHIFT		6
#define MAX97236_ENABLE_1_RESET_WIDTH		1
#define MAX97236_ENABLE_1_MIC_BIAS_MASK		(1 << 4)
#define MAX97236_ENABLE_1_MIC_BIAS_BIT		(1 << 4)
#define MAX97236_ENABLE_1_MIC_BIAS_SHIFT	4
#define MAX97236_ENABLE_1_MIC_BIAS_WIDTH	1
#define MAX97236_ENABLE_1_MIC_AMP_MASK		(1 << 3)
#define MAX97236_ENABLE_1_MIC_AMP_BIT		(1 << 3)
#define MAX97236_ENABLE_1_MIC_AMP_SHIFT		3
#define MAX97236_ENABLE_1_MIC_AMP_WIDTH		1
#define MAX97236_ENABLE_1_KS_MASK		(1 << 2)
#define MAX97236_ENABLE_1_KS_BIT		(1 << 2)
#define MAX97236_ENABLE_1_KS_SHIFT		2
#define MAX97236_ENABLE_1_KS_WIDTH		1

/*
 * MAX97236_ENABLE_2
 */
#define MAX97236_ENABLE_2_LFTEN_MASK		(1 << 7)
#define MAX97236_ENABLE_2_LFTEN_BIT		(1 << 7)
#define MAX97236_ENABLE_2_LFTEN_SHIFT		7
#define MAX97236_ENABLE_2_LFTEN_WIDTH		1
#define MAX97236_ENABLE_2_RGHEN_MASK		(1 << 6)
#define MAX97236_ENABLE_2_RGHEN_BIT		(1 << 6)
#define MAX97236_ENABLE_2_RGHEN_SHIFT		6
#define MAX97236_ENABLE_2_RGHEN_WIDTH		1
#define MAX97236_ENABLE_2_SVEN_MASK		(1 << 5)
#define MAX97236_ENABLE_2_SVEN_BIT		(1 << 5)
#define MAX97236_ENABLE_2_SVEN_SHIFT		5
#define MAX97236_ENABLE_2_SVEN_WIDTH		1
#define MAX97236_ENABLE_2_ZDEN_MASK		(1 << 4)
#define MAX97236_ENABLE_2_ZDEN_BIT		(1 << 4)
#define MAX97236_ENABLE_2_ZDEN_SHIFT		4
#define MAX97236_ENABLE_2_ZDEN_WIDTH		1
#define MAX97236_ENABLE_2_FAST_MASK		(1 << 3)
#define MAX97236_ENABLE_2_FAST_BIT		(1 << 3)
#define MAX97236_ENABLE_2_FAST_SHIFT		3
#define MAX97236_ENABLE_2_FAST_WIDTH		1
#define MAX97236_ENABLE_2_THRH_MASK		(1 << 2)
#define MAX97236_ENABLE_2_THRH_BIT		(1 << 2)
#define MAX97236_ENABLE_2_THRH_SHIFT		2
#define MAX97236_ENABLE_2_THRH_WIDTH		1
#define MAX97236_ENABLE_2_AUTO_MASK		(0x03 << 0)
#define MAX97236_ENABLE_2_AUTO_SHIFT		0
#define MAX97236_ENABLE_2_AUTO_WIDTH		2
#define MAX97236_ENABLE_2_AUTO_MANUAL		0
#define MAX97236_ENABLE_2_AUTO_AUTO1		1
#define MAX97236_ENABLE_2_AUTO_AUTO2		2

/* MAX97236 Register Bit Fields End */

#define MAX97236_DDONE_TIMEOUT			20
#define MAX97236_DETECT_RETRIES			10
#define MAX97236_DETECT_STABLEMAX		0
#define MAX97236_DETECT_STABLEDELAY		5



#define MAX97236_DETECT_DEFAULT			0x000D
#define MAX97236_DETECT_LINEOUT			0x0002

#define MAX97236_FORCE_DETECTED_HP		0x000001
#define MAX97236_FORCE_DETECTED_HP_LINE		0x000000
#define MAX97236_FORCE_DETECTED_DIGITAL		0x000008
#define MAX97236_FORCE_DETECTED_HS_OMTP		0x000009
#define MAX97236_FORCE_DETECTED_HS_NON_OMTP	0x000019
#define MAX97236_FORCE_DETECTED_LINE		0x000006

#define CLASS_INVALID       -1	/* detection error */
#define CLASS_UNKNOWN        0	/* unknown type of load */
#define CLASS_HP_LINE        1	/* LR low/mid impedance load w/o MIC */
#define CLASS_LINE_OUT       2
#define CLASS_HEADPHONE      3
#define CLASS_HEADSET_TYPE_1 4	/* LR low/mid impedance + MIC */
#define CLASS_HEADSET_TYPE_2 5	/* LR low/mid impedance + MIC */
#define CLASS_DIGITAL        6	/* high impedance load attached */
#define CLASS_NONE           7	/* nothing is attached */

#endif
