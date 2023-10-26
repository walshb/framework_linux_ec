/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * ChromeOS Embedded Controller core interface.
 *
 * Copyright (C) 2020 Google LLC
 */

#ifndef __FWK_EC_H
#define __FWK_EC_H

#include <linux/interrupt.h>

int fwk_ec_register(struct fwk_ec_device *ec_dev);
void fwk_ec_unregister(struct fwk_ec_device *ec_dev);

int fwk_ec_suspend(struct fwk_ec_device *ec_dev);
int fwk_ec_resume(struct fwk_ec_device *ec_dev);

irqreturn_t fwk_ec_irq_thread(int irq, void *data);

#endif /* __FWK_EC_H */
