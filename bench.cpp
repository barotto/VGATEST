/*
  VGATEST
  Use at your own risk.

  Copyright (C) 2019  Marco Bortolin

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Benchmarks used in both gfx and text modes

#include <conio.h>
#include <stdio.h>
#include <float.h>
#include <math.h>

#include "vgatest.h"
#include "ztimer.h"
#include "fps.h"

class BenchRes
{
public:
    bool running;
    double bandwidth;
    double fps;

    BenchRes() :
        running(true), bandwidth(.0), fps(.0) {}
    BenchRes(double _bw, double _fps) :
        running(false), bandwidth(_bw), fps(_fps) {}
};

typedef BenchRes(*pt2MemBenchFn)(uint32_t, uint32_t, uint8_t*);

double toMBps(uint32_t bytes, double us)
{
    if(isnan(us) || us == 0.0) {
        return NAN;
    } else {
        return (double(bytes) / us) * 0.953674316;
    }
}

BenchRes mem_write_32(uint32_t bwbytes, uint32_t pagebytes, uint8_t *memoff)
{
    uint32_t rept = bwbytes / 4;
    ZTimerOn();
    _asm {
        mov edi, memoff
        mov ecx, rept
        cld
        rep stosd
    };
    ZTimerOff();

    rept = pagebytes / 4;
    FPS fps;
    while(!fps.tick()) {
        _asm {
            mov edi, memoff
            mov ecx, rept
            cld
            rep stosd
        };
    }

    return BenchRes(toMBps(bwbytes, zTimeToUs()), fps.fps);
}

BenchRes mem_write_16(uint32_t bwbytes, uint32_t pagebytes, uint8_t *memoff)
{
    uint32_t rept = bwbytes / 2;
    ZTimerOn();
    _asm {
        mov edi, memoff
        mov ecx, rept
        cld
        rep stosw
    };
    ZTimerOff();

    rept = pagebytes / 2;
    FPS fps;
    while(!fps.tick()) {
        _asm {
            mov edi, memoff
            mov ecx, rept
            cld
            rep stosw
        };
    }

    return BenchRes(toMBps(bwbytes, zTimeToUs()), fps.fps);
}

BenchRes mem_write_8(uint32_t bwbytes, uint32_t pagebytes, uint8_t *memoff)
{
    ZTimerOn();
    _asm {
        mov edi, memoff
        mov ecx, bwbytes
        cld
        rep stosb
    };
    ZTimerOff();

    FPS fps;
    while(!fps.tick()) {
        _asm {
            mov edi, memoff
            mov ecx, pagebytes
            cld
            rep stosb
        };
    }

    return BenchRes(toMBps(bwbytes, zTimeToUs()), fps.fps);
}

BenchRes mem_read_32(uint32_t bwbytes, uint32_t pagebytes, uint8_t *memoff)
{
    uint32_t rept = bwbytes / 4;
    ZTimerOn();
    _asm {
        mov esi, memoff
        mov ecx, rept
        cld
        rep lodsd
    };
    ZTimerOff();

    rept = pagebytes / 4;
    FPS fps;
    while(!fps.tick()) {
        _asm {
            mov esi, memoff
            mov ecx, rept
            cld
            rep lodsd
        };
    }

    return BenchRes(toMBps(bwbytes, zTimeToUs()), fps.fps);
}

BenchRes mem_read_16(uint32_t bwbytes, uint32_t pagebytes, uint8_t *memoff)
{
    uint32_t rept = bwbytes / 2;
    ZTimerOn();
    _asm {
        mov esi, memoff
        mov ecx, rept
        cld
        rep lodsw
    };
    ZTimerOff();

    rept = pagebytes / 2;
    FPS fps;
    while(!fps.tick()) {
        _asm {
            mov esi, memoff
            mov ecx, rept
            cld
            rep lodsw
        };
    }

    return BenchRes(toMBps(bwbytes, zTimeToUs()), fps.fps);
}

BenchRes mem_read_8(uint32_t bwbytes, uint32_t pagebytes, uint8_t *memoff)
{
    ZTimerOn();
    _asm {
        mov esi, memoff
        mov ecx, bwbytes
        cld
        rep lodsb
    };
    ZTimerOff();

    FPS fps;
    while(!fps.tick()) {
        _asm {
            mov esi, memoff
            mov ecx, pagebytes
            cld
            rep lodsb
        };
    }

    return BenchRes(toMBps(bwbytes, zTimeToUs()), fps.fps);
}


void demoMemBenchWriteResults(BenchRes results[6], bool gfxmode)
{
    if(gfxmode) {
        gfx.setActivePage(0);
        gfx.clear(gfx.color(c_black));
        gfx.drawText(8, 8, gfx.color(c_white), gfx.modeName());
    } else {
        text.setActivePage(0);
        text.erasePage(c_black, c_black);
        text(1,1)(text.modeName(), c_lgray);
    }

    const char *testStrings[6] = {
        "32bit w: %s",
        "16bit w: %s",
        " 8bit w: %s",
        "32bit r: %s",
        "16bit r: %s",
        " 8bit r: %s"
    };

    const char *resStrings[6] = {
        "32bit w: %.2f MB/s, %.2f fps",
        "16bit w: %.2f MB/s, %.2f fps",
        " 8bit w: %.2f MB/s, %.2f fps",
        "32bit r: %.2f MB/s, %.2f fps",
        "16bit r: %.2f MB/s, %.2f fps",
        " 8bit r: %.2f MB/s, %.2f fps"
    };

    int vpos;

    if(gfxmode) {
        vpos = 8 + gfx.fontHeight();
    } else {
        vpos = 3;
    }

    char buf[50] = "";
    for(int r=0; r<6; r++) {
        if(results[r].running) {
            snprintf(buf, 50, testStrings[r], "wait...");
        } else if(isnan(results[r].bandwidth)) {
            snprintf(buf, 50, testStrings[r], "NaN");
        } else {
            snprintf(buf, 50, resStrings[r], results[r].bandwidth, results[r].fps);
        }

        if(gfxmode) {
            gfx.drawText(8, vpos, gfx.color(r<3?c_lred:c_lgreen), buf);
            vpos += gfx.fontHeight();
        } else {
            text(vpos++,1)(buf, (r<3?c_lred:c_lgreen));
        }
    }
}

void demoMemBench(bool gfxmode)
{
    uint32_t pagebytes, bwbytes = 32000;
    uint8_t *offset;

repeat:
    if(gfxmode) {
        gfx.setVisiblePage(0);
        gfx.setActivePage(1);
        offset = gfx.activeOffset();
        pagebytes = gfx.pageSize();
    } else {
        text.setVisiblePage(0);
        text.setActivePage(1);
        offset = (uint8_t*)text.activeOffset();
        pagebytes = text.pageSize();
    }

    BenchRes results[6];
    demoMemBenchWriteResults(results, gfxmode);

    pt2MemBenchFn benchFunctions[6] = {
        &mem_write_32,
        &mem_write_16,
        &mem_write_8,
        &mem_read_32,
        &mem_read_16,
        &mem_read_8
    };

    for(int f=0; f<6; f++) {
        if(gfxmode) {
            gfx.setActivePage(1);
        } else {
            text.setActivePage(1);
        }
        ztimercount = 0;
        results[f] = benchFunctions[f](bwbytes, pagebytes, offset);
        demoMemBenchWriteResults(results, gfxmode);
    }
    if(gfxmode) {
        gfx.drawText(8, gfx.maxy()-gfx.fontHeight()-8, gfx.color(c_green), "ESC to exit, R to repeat");
    } else {
        text(text.rows()-2,1)("ESC to exit, R to repeat", c_green);
    }
    char ch = getch();
    if(ch == 114) {
        // a goto! the orror!
        goto repeat;
    }
}
