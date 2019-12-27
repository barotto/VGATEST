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

rm *.obj
rm *.o

WPP="wpp386 -bt=dos -zc -fpc -3r -oneilr -mf -zq -d0"
WCL="wcl386 -bcl=dos -bt=dos -k128k -l=dos4g -mf -d0"

$WPP vgatest.cpp
$WPP gs.cpp
$WPP ts.cpp
$WPP utils.cpp
$WPP ztimer.cpp
$WPP demotext.cpp
$WPP demogfx.cpp
$WPP bench.cpp

rm vgatest.exe

$WCL -fe=vgatest.exe \
  utils.o \
  vgatest.o \
  demotext.o \
  demogfx.o \
  gs.o \
  ts.o \
  ztimer.o \
  bench.o
