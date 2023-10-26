/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __FWK_TYPEC_VDM__
#define __FWK_TYPEC_VDM__

#include <linux/usb/typec_altmode.h>

extern struct typec_altmode_ops port_amode_ops;

void fwk_typec_handle_vdm_attention(struct fwk_typec_data *typec, int port_num);
void fwk_typec_handle_vdm_response(struct fwk_typec_data *typec, int port_num);

#endif /*  __FWK_TYPEC_VDM__ */
