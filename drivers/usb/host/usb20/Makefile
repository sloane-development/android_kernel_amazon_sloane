#
# for USB OTG silicon based on Mentor Graphics INVENTRA designs
#
obj-$(CONFIG_USB_MTK_HDRC) += musb_hdrc.o
ifneq ($(CONFIG_USB_MTK_HDRC),)
obj-$(CONFIG_ARCH_MT8135) += mt8135/
endif

musb_hdrc-y := musb_core.o

musb_hdrc-y					+= musb_gadget_ep0.o musb_gadget.o
musb_hdrc-y					+= musb_virthub.o musb_host.o
musb_hdrc-y					+= musbhsdma.o
musb_hdrc-$(CONFIG_USB_MTK_DEBUG_FS)		+= musb_debugfs.o
