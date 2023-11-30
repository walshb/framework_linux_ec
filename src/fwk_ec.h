/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * ChromeOS Embedded Controller core interface.
 *
 * Copyright (C) 2020 Google LLC
 */

#ifndef __FWK_EC_H
#define __FWK_EC_H

#include <linux/interrupt.h>

struct fwk_ec_device;

int fwk_ec_register(struct fwk_ec_device *ec_dev);
void fwk_ec_unregister(struct fwk_ec_device *ec_dev);

int fwk_ec_suspend(struct fwk_ec_device *ec_dev);
int fwk_ec_suspend_late(struct fwk_ec_device *ec_dev);
int fwk_ec_suspend_prepare(struct fwk_ec_device *ec_dev);
int fwk_ec_resume(struct fwk_ec_device *ec_dev);
int fwk_ec_resume_early(struct fwk_ec_device *ec_dev);
void fwk_ec_resume_complete(struct fwk_ec_device *ec_dev);

irqreturn_t fwk_ec_irq_thread(int irq, void *data);

#endif /* __FWK_EC_H */
