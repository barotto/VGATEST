DEBUG   = -d0
MACHINE = -3r
OPT     = -oneilr

.cpp.obj:
	wpp386 -bt=dos -mf -zc -fpc $(MACHINE) $(OPT) $(DEBUG) -zq -fo=$[&.obj $*.cpp

OBJS = vgatest.obj gs.obj ts.obj utils.obj ztimer.obj demotext.obj demogfx.obj

all: vgatest.exe

vgatest.exe : $(OBJS)
	wcl386 -k128k -l=dos4g -mf $(DEBUG) $(OBJS)

clean:
	-rm *.bak
	-rm *.obj
	-rm *.exe
	-rm err
	-rm j
