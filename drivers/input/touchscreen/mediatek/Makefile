include $(srctree)/drivers/misc/mediatek/Makefile.custom
ccflags-y += -I$(srctree)/drivers/misc/mediatek/video \
             -I$(srctree)/drivers/misc/mediatek/video/$(CONFIG_MTK_PLATFORM) \
             -I$(srctree)/drivers/misc/mediatek/videox/$(CONFIG_MTK_PLATFORM) \
             -I$(srctree)/drivers/misc/mediatek/dispsys/$(CONFIG_MTK_PLATFORM)

# In case the platform does NOT support this type of sensors
obj-y   +=  mtk_tpd.o
obj-y   +=  tpd_button.o
obj-y   +=  tpd_calibrate.o
obj-y   +=  tpd_debug.o
obj-y   +=  tpd_default.o
obj-y   +=  tpd_init.o
obj-y   +=  tpd_misc.o
obj-y   +=  tpd_setting.o

ifeq ($(CONFIG_TOUCHSCREEN_MTK_GT9XX),y)
obj-y	+=  GT9XX/
endif

ifeq ($(CONFIG_TOUCHSCREEN_MTK_GT911),y)
obj-y	+=  GT911/
endif

ifeq ($(CONFIG_TOUCHSCREEN_MTK_SYNAPTICS_I2C_RMI4),y)
obj-y	+=  synaptics_i2c_rmi4/
endif
