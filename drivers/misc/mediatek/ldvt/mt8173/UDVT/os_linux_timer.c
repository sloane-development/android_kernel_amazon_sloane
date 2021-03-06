/********************************************************************************************
 *     LEGAL DISCLAIMER
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED,
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION,
 *     TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/


#include <linux/jiffies.h>
#include <linux/module.h>
#include <linux/param.h>
#include <linux/slab.h>
#include <linux/timer.h>
#include "x_assert.h"
#include "x_os.h"


typedef struct os_timer_light
{
    struct timer_list timer;
    x_os_timer_cb_fct pf_callback;
    VOID *pv_tag;
    BOOL fg_active;
    unsigned long interval;
} OS_TIMER_LIGHT_T;


static const UINT32 s_timer_res = 1000 / HZ;


static void TimerProc(unsigned long id)
{
    OS_TIMER_LIGHT_T *pt_timer;

    pt_timer = (OS_TIMER_LIGHT_T *)(id);
    if(pt_timer->interval == 0)
    	pt_timer->fg_active = FALSE; //move deactive to this line, in case of some one call del_timer_sync in 'pt_timer->pf_callback'
    pt_timer->pf_callback((HANDLE_T)(pt_timer), pt_timer->pv_tag);

    if (pt_timer->interval != 0)
    {
        pt_timer->timer.expires += pt_timer->interval;
        add_timer(&pt_timer->timer);
    }
}


INT32 x_timer_create (HANDLE_T *ph_timer)
{
    OS_TIMER_LIGHT_T *pt_timer;

    if (ph_timer == NULL)
    {
        return OSR_INV_ARG;
    }

    pt_timer = kcalloc(1, sizeof(OS_TIMER_LIGHT_T), GFP_KERNEL);
    if (pt_timer == NULL)
    {
        return OSR_NO_RESOURCE;
    }
    //FILL_CALLER(pt_timer);

    init_timer(&pt_timer->timer);

    *ph_timer = (HANDLE_T)(pt_timer);
    return OSR_OK;
}

EXPORT_SYMBOL(x_timer_create);


INT32 x_timer_start (HANDLE_T           h_timer,
                     UINT32             ui4_delay,
                     TIMER_FLAG_T       e_flags,
                     x_os_timer_cb_fct  pf_callback,
                     VOID*              pv_tag)
{
    OS_TIMER_LIGHT_T *pt_timer;
    unsigned long j;

    if ((pf_callback == NULL) || (ui4_delay == 0) ||
        ((e_flags != X_TIMER_FLAG_ONCE) && (e_flags != X_TIMER_FLAG_REPEAT)))
    {
        return OSR_INV_ARG;
    }

    j = jiffies;
    pt_timer = (OS_TIMER_LIGHT_T *)(h_timer);
    pt_timer->timer.expires = j + ui4_delay / s_timer_res;
    pt_timer->timer.function = &TimerProc;
    pt_timer->timer.data = (unsigned long)pt_timer;
    if (e_flags == X_TIMER_FLAG_ONCE)
    {
        pt_timer->interval = 0;
    }
    else
    {
        pt_timer->interval = ui4_delay / s_timer_res;
    }

    pt_timer->pf_callback = pf_callback;
    pt_timer->pv_tag = pv_tag;
    pt_timer->fg_active = TRUE;
    add_timer(&pt_timer->timer);

    return OSR_OK;
}

EXPORT_SYMBOL(x_timer_start);


INT32 x_timer_stop (HANDLE_T  h_timer)
{
    OS_TIMER_LIGHT_T *pt_timer;

    pt_timer = (OS_TIMER_LIGHT_T *)(h_timer);
    if (pt_timer->fg_active)
    {
        del_timer_sync(&pt_timer->timer);
        pt_timer->fg_active = FALSE;
        pt_timer->timer.expires -= jiffies;
    }
    return OSR_OK;
}

EXPORT_SYMBOL(x_timer_stop);


INT32 x_timer_delete (HANDLE_T  h_timer)
{
    OS_TIMER_LIGHT_T *pt_timer;

    pt_timer = (OS_TIMER_LIGHT_T *)(h_timer);
    if (pt_timer->fg_active)
    {
        del_timer_sync(&pt_timer->timer);
    }
    kfree(pt_timer);
    return OSR_OK;
}

EXPORT_SYMBOL(x_timer_delete);


INT32 x_timer_resume (HANDLE_T  h_timer)
{
    OS_TIMER_LIGHT_T *pt_timer;

    pt_timer = (OS_TIMER_LIGHT_T *)(h_timer);
    if (!pt_timer->fg_active)
    {
        pt_timer->timer.expires += jiffies;
        pt_timer->fg_active = TRUE;
        add_timer(&pt_timer->timer);
    }

    return OSR_OK;
}

EXPORT_SYMBOL(x_timer_resume);


UINT32 x_os_get_sys_tick (VOID)
{
    return jiffies;
}

EXPORT_SYMBOL(x_os_get_sys_tick);


UINT32 x_os_get_sys_tick_period (VOID)
{
    return s_timer_res;
}

EXPORT_SYMBOL(x_os_get_sys_tick_period);


INT32 os_timer_init (VOID)
{
    return OSR_OK;
}


