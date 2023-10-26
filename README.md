## What is this?

This is the source code for replacemant "cros\_ec" modules for Linux.

The original "cros\_ec" modules give errors in the system log:

```
cros_ec_lpcs cros_ec_lpcs.0: bad packet checksum
```

These are caused by both ACPI AML and the "cros\_ec" modules both
trying to access the EC at the same time.

The new code makes the "cros\_ec" modules use the AML mutex.

## How do I build and install?

```
cd src
make
sudo make insmod
```

## Why are the modules called "fwk\_ec\_\*", not "cros\_ec\_\*"?

We have to replace the "cros\_ec\_proto" module which is built in to
the kernel, so we create a loadable module with a different name. Or,
we could compile a new kernel but that's a bit more work.

## Can I compile a new kernel instead?

Yes. Please see [here](https://github.com/walshb/linux), branch
"bw-cros-ec-acpi-mutex".

## Why does `make insmod` dump a stack trace in the log?

If the log mentions "cros\_ec\_lpc\_exit" then it's a known bug in
"cros\_ec\_lpc". This bug is fixed in "fwk\_ec\_lpc".
