## What is this?

This is the source code for replacemant "cros\_ec" modules for Linux.

The original "cros\_ec" modules give errors in the system log:

```
cros_ec_lpcs cros_ec_lpcs.0: bad packet checksum
```

These are caused by ACPI AML and the "cros\_ec" modules both trying to
access the EC at the same time.

The new code makes the "cros\_ec" modules use the AML mutex.

## How do I build and install?

```
cd src
make
sudo make insmod
```

This just performs `insmod`, and does not install in /lib/modules, so
the changes won't persist over reboot.

If you have Secure Boot enabled, then the insmod will fail because the
modules aren't signed. You can manually sign them with your MOK, but
probably easier is to install via DKMS (see below), which will
automatically take care of the signing process.

## Why are the modules called "fwk\_ec\_\*", not "cros\_ec\_\*"?

We have to replace the "cros\_ec\_proto" module which is built in to
the kernel, so we create a loadable module with a different name. Or,
we could compile a new kernel but that's a bit more work.

Most of the code is exactly the same as in the upstream Linux
chrome-platform
[for-next](https://git.kernel.org/pub/scm/linux/kernel/git/chrome-platform/linux.git/log/?h=for-next)
branch. All I have done is rename the modules and add the AML mutex
locking.

## Can I compile a new kernel instead?

Yes. Please see [here](https://github.com/walshb/linux), branch
"bw-cros-ec-acpi-mutex-dh-v3".

## Why does `make insmod` dump a stack trace in the log?

If the log mentions "cros\_ec\_lpc\_exit" then it's a known bug in
"cros\_ec\_lpc". This bug is fixed in "fwk\_ec\_lpc".

## How can I check the modules are working?

Check that the "cros_ec" device exists:

```
ls -l /dev/cros_ec
```

Check for these messages in the log:

```
fwk_ec_lpcs PNP0C09:00: Got AML mutex 'ECMT'
fwk_ec_lpcs PNP0C09:00: fwk_ec_lpc_mutex_lock, result 1
fwk_ec_lpcs PNP0C09:00: fwk_ec_lpc_mutex_unlock, result 1
fwk_ec_lpcs PNP0C09:00: Chrome EC device registered
```
After this, the first 100 `fwk_ec_lpc_mutex_{un,}lock` calls will be
logged, and should correlate with `ectool` usage, giving further
confidence that the modules are working correctly.

## How do I make changes persist over reboot?

You can use `sudo make install`, or even better, use DKMS to recompile
the code when the kernel is updated:

```
sudo apt install dkms  # or whatever your distro requires
cd src
make
sudo make dkms-install
sudo dkms add -m fwk_ec -v 3.0.0
sudo dkms autoinstall
sudo install -m 644 ../extras/cros_ec_blacklist.conf /etc/modprobe.d/
sudo update-initramfs -u
```
