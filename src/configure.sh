#!/bin/sh
cat inc.mk.tmpl | grep -v "PROJECT_HOME=" > inc.mk.temp
echo "PROJECT_HOME=`pwd`/../" > inc.mk
cat inc.mk.temp >> inc.mk
rm -f inc.mk.temp
