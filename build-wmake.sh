#!/bin/sh
# the WATCOM environment variable needs to be defined with the Open Watcom install directory.
if [ -z "$WATCOM" ]; then
	echo "please define the WATCOM environment variable"
	exit 1
fi
export PATH=$WATCOM/binl:$PATH
export EDPATH=$WATCOM/eddat
export WIPFC=$WATCOM/wipfc
export INCLUDE=$WATCOM/h

wmake clean
wmake all
