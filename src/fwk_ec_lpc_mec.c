// SPDX-License-Identifier: GPL-2.0
// LPC variant I/O for Microchip EC
//
// Copyright (C) 2016 Google, Inc

#include <linux/delay.h>
#include <linux/io.h>
#include <linux/mutex.h>
#include <linux/types.h>

#include "fwk_ec_lpc_mec.h"

#define ACPI_LOCK_DELAY_MS 500

/*
 * This mutex must be held while accessing the EMI unit. We can't rely on the
 * EC mutex because memmap data may be accessed without it being held.
 */
static DEFINE_MUTEX(io_mutex);
static u16 mec_emi_base, mec_emi_end;
static acpi_handle aml_mutex;

static int n_debug;

static int fwk_ec_lpc_mec_lock(void)
{
	bool success;

	if (!aml_mutex) {
		mutex_lock(&io_mutex);
		return 0;
	}

	success = ACPI_SUCCESS(acpi_acquire_mutex(aml_mutex,
						  NULL, ACPI_LOCK_DELAY_MS));
	if (n_debug++ < 100)
		pr_info("%s, result %d", __func__, (int)success);

	if (!success) {
		pr_info("%s failed.", __func__);
		return -ENODEV;
	}

	return 0;
}

static int fwk_ec_lpc_mec_unlock(void)
{
	bool success;

	if (!aml_mutex) {
		mutex_unlock(&io_mutex);
		return 0;
	}

	success = ACPI_SUCCESS(acpi_release_mutex(aml_mutex, NULL));

	if (n_debug++ < 100)
		pr_info("%s, result %d", __func__, (int)success);

	if (!success) {
		pr_err("%s failed.", __func__);
		return -ENODEV;
	}

	return 0;
}

/**
 * fwk_ec_lpc_mec_emi_write_address() - Initialize EMI at a given address.
 *
 * @addr: Starting read / write address
 * @access_type: Type of access, typically 32-bit auto-increment
 */
static void fwk_ec_lpc_mec_emi_write_address(u16 addr,
			enum fwk_ec_lpc_mec_emi_access_mode access_type)
{
	outb((addr & 0xfc) | access_type, MEC_EMI_EC_ADDRESS_B0(mec_emi_base));
	outb((addr >> 8) & 0x7f, MEC_EMI_EC_ADDRESS_B1(mec_emi_base));
}

/**
 * fwk_ec_lpc_mec_in_range() - Determine if addresses are in MEC EMI range.
 *
 * @offset: Address offset
 * @length: Number of bytes to check
 *
 * Return: 1 if in range, 0 if not, and -EINVAL on failure
 *         such as the mec range not being initialized
 */
int fwk_ec_lpc_mec_in_range(unsigned int offset, unsigned int length)
{
	if (length == 0)
		return -EINVAL;

	if (WARN_ON(mec_emi_base == 0 || mec_emi_end == 0))
		return -EINVAL;

	if (offset >= mec_emi_base && offset < mec_emi_end) {
		if (WARN_ON(offset + length - 1 >= mec_emi_end))
			return -EINVAL;
		return 1;
	}

	if (WARN_ON(offset + length > mec_emi_base && offset < mec_emi_end))
		return -EINVAL;

	return 0;
}

/**
 * fwk_ec_lpc_io_bytes_mec() - Read / write bytes to MEC EMI port.
 *
 * @io_type: MEC_IO_READ or MEC_IO_WRITE, depending on request
 * @offset:  Base read / write address
 * @length:  Number of bytes to read / write
 * @buf:     Destination / source buffer
 *
 * Return: 8-bit checksum of all bytes read / written
 */
int fwk_ec_lpc_io_bytes_mec(enum fwk_ec_lpc_mec_io_type io_type,
			     unsigned int offset, unsigned int length,
			     u8 *buf)
{
	int i = 0;
	int io_addr;
	u8 sum = 0;
	enum fwk_ec_lpc_mec_emi_access_mode access, new_access;
	int ret;

	/* Return checksum of 0 if window is not initialized */
	WARN_ON(mec_emi_base == 0 || mec_emi_end == 0);
	if (mec_emi_base == 0 || mec_emi_end == 0)
		return 0;

	/*
	 * Long access cannot be used on misaligned data since reading B0 loads
	 * the data register and writing B3 flushes.
	 */
	if (offset & 0x3 || length < 4)
		access = ACCESS_TYPE_BYTE;
	else
		access = ACCESS_TYPE_LONG_AUTO_INCREMENT;

	ret = fwk_ec_lpc_mec_lock();
	if (ret)
		return ret;

	/* Initialize I/O at desired address */
	fwk_ec_lpc_mec_emi_write_address(offset, access);

	/* Skip bytes in case of misaligned offset */
	io_addr = MEC_EMI_EC_DATA_B0(mec_emi_base) + (offset & 0x3);
	while (i < length) {
		while (io_addr <= MEC_EMI_EC_DATA_B3(mec_emi_base)) {
			if (io_type == MEC_IO_READ)
				buf[i] = inb(io_addr++);
			else
				outb(buf[i], io_addr++);

			sum += buf[i++];
			offset++;

			/* Extra bounds check in case of misaligned length */
			if (i == length)
				goto done;
		}

		/*
		 * Use long auto-increment access except for misaligned write,
		 * since writing B3 triggers the flush.
		 */
		if (length - i < 4 && io_type == MEC_IO_WRITE)
			new_access = ACCESS_TYPE_BYTE;
		else
			new_access = ACCESS_TYPE_LONG_AUTO_INCREMENT;

		if (new_access != access ||
		    access != ACCESS_TYPE_LONG_AUTO_INCREMENT) {
			access = new_access;
			fwk_ec_lpc_mec_emi_write_address(offset, access);
		}

		/* Access [B0, B3] on each loop pass */
		io_addr = MEC_EMI_EC_DATA_B0(mec_emi_base);
	}

done:
	fwk_ec_lpc_mec_unlock();

	return sum;
}
EXPORT_SYMBOL(fwk_ec_lpc_io_bytes_mec);

void fwk_ec_lpc_mec_init(unsigned int base, unsigned int end)
{
	mec_emi_base = base;
	mec_emi_end = end;
}
EXPORT_SYMBOL(fwk_ec_lpc_mec_init);

int fwk_ec_lpc_mec_mutex(struct acpi_device *adev,
			  const char *aml_mutex_name)
{
	int status;

	if (!adev)
		return -ENOENT;

	status = acpi_get_handle(adev->handle,
				 (acpi_string)aml_mutex_name,
				 &aml_mutex);
	if (ACPI_FAILURE(status))
		return -ENOENT;

	return 0;
}
EXPORT_SYMBOL(fwk_ec_lpc_mec_mutex);
