#!/bin/sh
# the WATCOM environment variable needs to be defined with the Open Watcom install directory.
export PATH=$WATCOM/binl:$PATH
export EDPATH=$WATCOM/eddat
export WIPFC=$WATCOM/wipfc
export INCLUDE=$WATCOM/h
OPTIONS="-bt=dos -zc -fpc -3r -oneilr -mf -zq -d0"
wpp386 $OPTIONS vgatest.cpp
wpp386 $OPTIONS gs.cpp
wpp386 $OPTIONS ts.cpp
wpp386 $OPTIONS utils.cpp
wpp386 $OPTIONS ztimer.cpp
wcl386 -bcl=dos -bt=dos -k128k -l=dos4g -mf -d0 -fe=vgatest.exe utils.o vgatest.o gs.o ts.o ztimer.o
