include $(srctree)/drivers/misc/mediatek/Makefile.custom

obj-$(CONFIG_MTK_SMART_BATTERY) += battery_meter.o battery_common.o

ifeq ($(CONFIG_MTK_FAN5405_SUPPORT),y)
    obj-$(CONFIG_MTK_SMART_BATTERY) += switch_charging.o
else
    ifeq ($(CONFIG_MTK_SWCHR_SUPPORT),y)
        obj-$(CONFIG_MTK_SMART_BATTERY) += switch_charging.o
    else
                obj-$(CONFIG_MTK_SMART_BATTERY) += linear_charging.o
            endif
        endif
