#!/bin/sh

set -eux

LINUX_SRC="$1"

do_sed() {
    sed -e 's|cros\([_-]\)|fwk\1|g' \
        -e 's|CROS_|FWK_|g' \
        -e 's|"cros\([_-]\)\([^\.]*\)\.h"|"fwk\1\2\.h"|' \
        "$@"
}

MYDIR=$(dirname $(readlink -f "$0"))
OUTDIR=$MYDIR/src

mkdir -p $OUTDIR
rm -f $OUTDIR/*.? $OUTDIR/modules.order $OUTDIR/Module.symvers \
   $OUTDIR/*.ko $OUTDIR/.??* $OUTDIR/*.mod

TARGET=$(uname -r)

cd $LINUX_SRC

git rev-parse --short HEAD >$OUTDIR/REVISION

cd $LINUX_SRC/include/linux/platform_data

for FNAME in cros_ec*.h cros_usbpd*.h
do
    OUTFNAME=$(echo $FNAME | do_sed)
    cp -av $FNAME $OUTDIR/$OUTFNAME
done

cp -av $LINUX_SRC/drivers/mfd/cros_ec_dev.c $OUTDIR/fwk_ec_dev.c

cd $LINUX_SRC/drivers/platform/chrome

for FNAME in cros_ec.* cros_ec_trace.* cros_ec_proto.* \
                       cros_ec_chardev.* cros_ec_lpc* \
                       cros_ec_debugfs.*
do
    OUTFNAME=$(echo $FNAME | do_sed)
    cp -av $FNAME $OUTDIR/$OUTFNAME
done

# fun and games with proto name
mv $OUTDIR/fwk_ec_proto.c $OUTDIR/fwk_ec_proto_src.c
echo 'MODULE_LICENSE("GPL");' >>$OUTDIR/fwk_ec_proto_src.c

echo 'fwk_ec_proto-objs := fwk_ec_proto_src.o fwk_ec_trace.o' >$OUTDIR/Kbuild
echo 'obj-m += fwk_ec_proto.o' >>$OUTDIR/Kbuild
echo 'obj-m += fwk_ec_dev.o' >>$OUTDIR/Kbuild
do_sed -e 's|\$(CONFIG\([^)]*\))|m|' Makefile \
    | grep -E 'fwk_ec\.|fwk_ec_chardev\.|fwk_ec_lpc|fwk_ec_debugfs\.' \
           >>$OUTDIR/Kbuild
echo 'ccflags-y=-I$(src)' >>$OUTDIR/Kbuild

do_sed -e 's|linux/platform_data/fwk_|fwk_|' \
       -i $OUTDIR/*.h $OUTDIR/*.c

sed -e 's|"fwk_ec"|"cros_ec"|g' -i $OUTDIR/fwk_ec_proto.h
