#ifndef _DDP_OVL_H_
#define _DDP_OVL_H_
#include "ddp_hal.h"
#include "DpDataType.h"


#define OVL_MAX_WIDTH  (4095)
#define OVL_MAX_HEIGHT (4095)
#define OVL_LAYER_NUM  (4)


/* start overlay module */
int ovl_start(DISP_MODULE_ENUM module, void *handle);

/* stop overlay module */
int ovl_stop(DISP_MODULE_ENUM module, void *handle);

/* reset overlay module */
int ovl_reset(DISP_MODULE_ENUM module, void *handle);

/* set region of interest */
int ovl_roi(DISP_MODULE_ENUM module, unsigned int bgW, unsigned int bgH,	/* region size */
	    unsigned int bgColor,	/* border color */
	    void *handle);

/* switch layer on/off */
int ovl_layer_switch(DISP_MODULE_ENUM module, unsigned layer, unsigned int en, void *handle);

/* configure layer property */
int ovl_layer_config(DISP_MODULE_ENUM module, unsigned layer, unsigned source, DpColorFormat format, unsigned int addr, unsigned int src_x,	/* ROI x offset */
		     unsigned int src_y,	/* ROI y offset */
		     unsigned int src_pitch, unsigned int dst_x,	/* ROI x offset */
		     unsigned int dst_y,	/* ROI y offset */
		     unsigned int dst_w,	/* ROT width */
		     unsigned int dst_h,	/* ROI height */
		     unsigned int keyEn, unsigned int key, unsigned int aen, unsigned char alpha,	/* trancparency */
		     void *handle);

int ovl_3d_config(DISP_MODULE_ENUM module,
		  unsigned int layer_id,
		  unsigned int en_3d, unsigned int landscape, unsigned int r_first, void *handle);

int ovl_dump(DISP_MODULE_ENUM module);

#endif
