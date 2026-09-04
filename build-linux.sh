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

# -bt build target is operating system <id>
# -zc place const data into the code segment
# -fpc calls to floating-point library
# -3r 386 register calling conventions
# -o optimization
#   e expand user functions inline
#   i expand intrinsic functions inline
#   l enable loop optimizations
#   r reorder instructions for best pipeline usage
# -mf flat memory model
# -zq operate quietly
# -d0 no debugging information
# -fm generate map file
# -k  set stack size
WPP32="wpp386 -bt=dos -zc -fpc -3r -oeilr -mf -zq -d0"
WCL32="wcl386 -bcl=dos -bt=dos -l=dos4g -k128k -mf -d0 -fm"

# -0 8086 instructions
# -mh huge memory model
WPP16="wpp -0 -bt=dos -zc -fpc -oeilr -mh -zq -d0"
WCL16="wcl -0 -bcl=dos -bt=dos -lr -k32k -mh -d0 -fm"

$WPP32 *.cpp data/fonts/*.cpp data/luts/*.cpp
$WCL32 -fe=VGATEST.EXE *.o
rm *.o

$WPP16 *.cpp data/fonts/*.cpp data/luts/*.cpp
$WCL16 -fe=VGATEST0.EXE *.o
rm *.o
