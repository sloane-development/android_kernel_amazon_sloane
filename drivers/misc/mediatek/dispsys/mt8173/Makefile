# drivers/dispsys
include $(srctree)/drivers/misc/mediatek/Makefile.custom

ccflags-y += -I$(srctree)/drivers/misc/mediatek/video/ \
             -I$(srctree)/drivers/misc/mediatek/videox/$(MTK_PLATFORM) \
             -I$(srctree)/drivers/misc/mediatek/ext_disp/$(MTK_PLATFORM) \
             -I$(srctree)/drivers/misc/mediatek/cmdq \
             -I$(srctree)/drivers/misc/mediatek/lcm

#driver
obj-y += ddp_ovl.o
obj-y += ddp_color.o
obj-y += ddp_rdma.o
obj-y += ddp_wdma.o
obj-y += ddp_aal.o
obj-y += ddp_pwm.o
obj-y += ddp_gamma.o
obj-y += ddp_dither.o
obj-y += ddp_od.o
obj-y += ddp_dsi.o
obj-y += ddp_dpi.o
#obj-y += ddp_dpi1.o
obj-y += ddp_split.o

#common
#obj-y += ddp_drv.o
obj-y += ddp_path.o
obj-y += ddp_debug.o
obj-y += ddp_dump.o

obj-y += ddp_manager.o
obj-y += ddp_irq.o
obj-y += ddp_info.o
obj-y += ddp_color_format.o


obj-y += data_rgb888_64x64.o
obj-y += data_rgb888_64x64_golden.o

obj-y += ddp_mmp.o

#should remove
#obj-y += ddp_misc.o

#new add
obj-y += ddp_ufoe.o ddp_met.o

obj-y += display_recorder.o

ifeq ($(CONFIG_MMPROFILE),y)
ccflags-y += -DDEFAULT_MMP_ENABLE
endif

ccflags-y += -DMET_USER_EVENT_SUPPORT


ifeq ($(CONFIG_MTK_ALPS_BOX_SUPPORT), y)
ccflags-y += -DMTK_ALPS_BOX_SUPPORT
endif

# EOF
