include $(srctree)/drivers/misc/mediatek/Makefile.custom

obj-  := dummy.o

ifeq ($(CONFIG_THERMAL), y)
else
    obj-y += mtk_thermal_dummy.o
endif

obj-$(CONFIG_THERMAL) += mtk_thermal_platform.o
obj-$(CONFIG_THERMAL) += mtk_ts_pa_thput.o
obj-$(CONFIG_THERMAL) += mtk_cooler_bcct.o

ifneq ($(CONFIG_MTK_COMBO_WIFI),)
    obj-$(CONFIG_THERMAL) += mtk_ts_wmt.o
endif

# Only included in non-EVB projects
ifeq ($(CONFIG_POWER_EXT), y)
else
    ifeq ($(CONFIG_MTK_SMART_BATTERY), y)
        # Always align the Makefile in mediatek/platform/mt6595/kernel/drivers/power/
        ifeq ($(CONFIG_MTK_NCP1851_SUPPORT),y)
            obj-$(CONFIG_THERMAL) += mtk_cooler_bcct.o
        else
            ifeq ($(CONFIG_MTK_BQ24196_SUPPORT),y)
                obj-$(CONFIG_THERMAL) += mtk_cooler_bcct.o
            else
                # FAN5405 or PMIC6320 pulse charging or new charger IC
                obj-$(CONFIG_THERMAL) += mtk_cooler_bcct.o
            endif
        endif
    endif
endif

ifeq ($(CONFIG_MTK_THERMAL_PA_VIA_ATCMD),yes)
    #obj-$(CONFIG_THERMAL) += mtk_cooler_mdulthro.o
    #obj-$(CONFIG_THERMAL) += mtk_cooler_mdtxpwr.o
    #obj-$(CONFIG_THERMAL) += mtk_cooler_mddtxpwr.o
    #obj-$(CONFIG_THERMAL) += mtk_cooler_amdtxctrl.o
    #obj-$(CONFIG_THERMAL) += mtk_cooler_amddulthro.o
    #obj-$(CONFIG_THERMAL) += mtk_cooler_mddulthro.o
endif

obj-$(CONFIG_THERMAL) += mtk_ts_cpu.o
obj-$(CONFIG_THERMAL) += mtk_ts_abb.o
obj-$(CONFIG_THERMAL) += mtk_ts_all_ts.o
obj-$(CONFIG_THERMAL) += mtk_ts_pa.o
obj-$(CONFIG_THERMAL) += mtk_ts_battery.o
obj-$(CONFIG_THERMAL) += mtk_ts_pmic.o
obj-$(CONFIG_THERMAL) += mtk_ts_buck.o
obj-$(CONFIG_THERMAL) += mtk_ts_bts.o
#obj-$(CONFIG_THERMAL) += mtk_ts_popram.o


obj-$(CONFIG_THERMAL) += tm_nth.o
