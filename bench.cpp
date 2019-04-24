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

#include "vgatest.h"
#include "ztimer.h"
#include "fps.h"

typedef void (*pt2MemBenchFn)(uint32_t, uint8_t*);

void mem_write_32(uint32_t _bytes, uint8_t *_offset)
{
    _bytes = _bytes / 4;
    ZTimerOn();
    _asm {
        mov edi, _offset
        mov ecx, _bytes
        cld
        rep stosd
    };
    ZTimerOff();
}

void mem_write_16(uint32_t _bytes, uint8_t *_offset)
{
    _bytes = _bytes / 2;
    ZTimerOn();
    _asm {
        mov edi, _offset
        mov ecx, _bytes
        cld
        rep stosw
    };
    ZTimerOff();
}

void mem_write_8(uint32_t _bytes, uint8_t *_offset)
{
    ZTimerOn();
    _asm {
        mov edi, _offset
        mov ecx, _bytes
        cld
        rep stosb
    };
    ZTimerOff();
}

void mem_read_32(uint32_t _bytes, uint8_t *_offset)
{
    _bytes = _bytes / 4;
    ZTimerOn();
    _asm {
        mov esi, _offset
        mov ecx, _bytes
        cld
        rep lodsd
    };
    ZTimerOff();
}

void mem_read_16(uint32_t _bytes, uint8_t *_offset)
{
    _bytes = _bytes / 2;
    ZTimerOn();
    _asm {
        mov esi, _offset
        mov ecx, _bytes
        cld
        rep lodsw
    };
    ZTimerOff();
}

void mem_read_8(uint32_t _bytes, uint8_t *_offset)
{
    ZTimerOn();
    _asm {
        mov esi, _offset
        mov ecx, _bytes
        cld
        rep lodsb
    };
    ZTimerOff();
}


void demoMemBenchWriteResults(double results[6], bool gfxmode)
{
    if(gfxmode) {
        gfx.setActivePage(0);
        gfx.clear(gfx.color(c_black));
        gfx.drawText(8, 8, gfx.color(c_white), gfx.modeName());
    } else {
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
        "32bit w: %.2f MB/s",
        "16bit w: %.2f MB/s",
        " 8bit w: %.2f MB/s",
        "32bit r: %.2f MB/s",
        "16bit r: %.2f MB/s",
        " 8bit r: %.2f MB/s"
    };

    int vpos;

    if(gfxmode) {
        vpos = 8 + gfx.fontHeight();
    } else {
        vpos = 3;
    }

    char buf[50] = "";
    for(int r=0; r<6; r++) {
        if(results[r] >= DBL_MAX) {
            snprintf(buf, 50, testStrings[r], "ovr");
        } else if(results[r]>.0){
            snprintf(buf, 50, resStrings[r], results[r]);
        } else {
            snprintf(buf, 50, testStrings[r], results[r]<.0?"":"wait...");
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
    uint32_t size = 32000;
    uint8_t *offset;

repeat:
    if(gfxmode) {
        gfx.setVisiblePage(0);
        gfx.setActivePage(1);
        offset = gfx.activeOffset();
    } else {
        offset = (uint8_t*)text.activeOffset();
    }

    double results[6] = {0.f,-1.f,-1.f,-1.f,-1.f,-1.f};
    demoMemBenchWriteResults(results, gfxmode);

    pt2MemBenchFn functions[6] = {
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
        }
        ztimercount = 0;
        functions[f](size, offset);
        if(ztimercount > 65535) {
            // overflow
            results[f] = DBL_MAX;
        } else {
            ztimercount -= ztimerref;
            double us = double(ztimercount) / 0.8381;
            results[f] = (double(size)/us) * 0.953674316;
        }
        if(f<5) {
            results[f+1] = 0.f;
        }
        demoMemBenchWriteResults(results, gfxmode);
    }
    if(gfxmode) {
        gfx.drawText(8, gfx.maxy()-gfx.fontHeight()-8, gfx.color(c_green), "press ESC or R to repeat");
    } else {
        text(text.rows()-2,1)("press ESC or R to repeat", c_green);
    }
    char ch = getch();
    if(ch == 114) {
        // a goto! the orror!
        goto repeat;
    }
}
