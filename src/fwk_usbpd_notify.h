// SPDX-License-Identifier: GPL-2.0-only
/*
 * ChromeOS EC Power Delivery Notifier Driver
 *
 * Copyright 2020 Google LLC
 */

#ifndef __LINUX_PLATFORM_DATA_FWK_USBPD_NOTIFY_H
#define __LINUX_PLATFORM_DATA_FWK_USBPD_NOTIFY_H

#include <linux/notifier.h>

int fwk_usbpd_register_notify(struct notifier_block *nb);

void fwk_usbpd_unregister_notify(struct notifier_block *nb);

#endif  /* __LINUX_PLATFORM_DATA_FWK_USBPD_NOTIFY_H */
