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

// Graphics Mode demos

#include <conio.h>
#include <stdio.h>
#include <float.h>

#include "vgatest.h"
#include "ztimer.h"
#include "fps.h"


//---------------------------------------------------
//
// Benchmark
//
typedef void (*pt2MemBenchFn)(uint32_t);

void video_write_32(uint32_t bytes)
{
    uint8_t *_offset = gfx.activeOffset();
    uint32_t _size = bytes / 4;
    ZTimerOn();
    _asm {
        mov edi, _offset
        mov ecx, _size
        cld
        rep stosd
    };
    ZTimerOff();
}

void video_write_16(uint32_t bytes)
{
    uint8_t *_offset = gfx.activeOffset();
    uint32_t _size = bytes / 2;
    ZTimerOn();
    _asm {
        mov edi, _offset
        mov ecx, _size
        cld
        rep stosw
    };
    ZTimerOff();
}

void video_write_8(uint32_t bytes)
{
    uint8_t *_offset = gfx.activeOffset();
    uint32_t _size = bytes;
    ZTimerOn();
    _asm {
        mov edi, _offset
        mov ecx, _size
        cld
        rep stosb
    };
    ZTimerOff();
}

void video_read_32(uint32_t bytes)
{
    uint8_t *_offset = gfx.activeOffset();
    uint32_t _size = bytes / 4;
    ZTimerOn();
    _asm {
        mov esi, _offset
        mov ecx, _size
        cld
        rep lodsd
    };
    ZTimerOff();
}

void video_read_16(uint32_t bytes)
{
    uint8_t *_offset = gfx.activeOffset();
    uint32_t _size = bytes / 2;
    ZTimerOn();
    _asm {
        mov esi, _offset
        mov ecx, _size
        cld
        rep lodsw
    };
    ZTimerOff();
}

void video_read_8(uint32_t bytes)
{
    uint8_t *_offset = gfx.activeOffset();
    uint32_t _size = bytes;
    ZTimerOn();
    _asm {
        mov esi, _offset
        mov ecx, _size
        cld
        rep lodsb
    };
    ZTimerOff();
}


void demoMemBenchWriteResults(double results[6])
{
    gfx.setActivePage(0);
    gfx.clear(gfx.color(c_black));
    gfx.drawText(8, 8, gfx.color(c_white), gfx.modeName());

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

    int vpos = 8+gfx.fontHeight();
    char buf[50] = "";
    for(int r=0; r<6; r++) {
        if(results[r] >= DBL_MAX) {
            snprintf(buf, 50, testStrings[r], "ovr");
        } else if(results[r]>.0){
            snprintf(buf, 50, resStrings[r], results[r]);
        } else {
            snprintf(buf, 50, testStrings[r], results[r]<.0?"":"wait...");
        }
        gfx.drawText(8, vpos, gfx.color(r<3?c_lred:c_lgreen), buf);
        vpos += gfx.fontHeight();
    }
}

void demoMemBench()
{
    gfx.setVisiblePage(0);
    gfx.setActivePage(0);
    gfx.clear(gfx.color(c_black));
    gfx.drawText(8, 8, gfx.color(c_white), gfx.modeName());

    double results[6] = {0.f,-1.f,-1.f,-1.f,-1.f,-1.f};
    demoMemBenchWriteResults(results);

    pt2MemBenchFn functions[6] = {
        &video_write_32,
        &video_write_16,
        &video_write_8,
        &video_read_32,
        &video_read_16,
        &video_read_8
    };

    uint32_t size = 32000;
    for(int f=0; f<6; f++) {
        gfx.setActivePage(1);
        ztimercount = 0;
        functions[f](size);
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
        demoMemBenchWriteResults(results);
    }
    gfx.drawText(8, gfx.maxy()-gfx.fontHeight()-8, gfx.color(c_green), "press ESC");
    getch();
}


//---------------------------------------------------
//
// Draw a bunch of worms that crawl around the screen.
// Code by Robert C. Pendleton

#define worms (100)
#define segments (50)

typedef struct
{
    int x, y;
} point;

typedef struct
{
    int head;
    int color;
    int heading;
    point body[segments];
} worm;

static point headings[] =
{
    { 0, 1},
    { 1, 1},
    { 1, 0},
    { 1,-1},
    { 0,-1},
    {-1,-1},
    {-1, 0},
    {-1, 1}
};

void demoWorm()
{
    worm can[worms];
    int head;
    int tail;
    int i, j, k;
    int x, y;
    int next;
    uint8_t bg = gfx.color(c_black);

    gfx.setVisiblePage(0);
    gfx.setActivePage(0);
    gfx.clear(bg);

    fsrand((unsigned) time(NULL));

    for (i = 0; i < worms; i++) {
        can[i].head = 0;
        can[i].heading = i & 0x7;
        can[i].color = gfx.color((i & 0x7f) + 32);
        for (j = 0; j < segments; j++) {
            can[i].body[j].x = gfx.maxx() / 2;
            can[i].body[j].y = gfx.maxy() / 2;
        }
    }

    FPS fps;

    while (!kbhit()) {
        for (i = 0; i < worms; i++) {
            tail = head = can[i].head;
            head++;
            if (head >= segments) {
                head = 0;
            }
            can[i].head = head;

            gfx.putPixel(can[i].body[head].x,
                         can[i].body[head].y,
                         bg);

            if (frand() < (FRAND_MAX / 10)) {
                if (frand() < (FRAND_MAX / 2)) {
                    can[i].heading = (can[i].heading + 1) & 0x7;
                } else {
                    can[i].heading = (can[i].heading - 1) & 0x7;
                }
            }

            x = can[i].body[tail].x;
            y = can[i].body[tail].y;

            x = x + headings[can[i].heading].x;
            y = y + headings[can[i].heading].y;

            next = gfx.getPixel(x, y);
            k = 0;
            while (next != -1 &&
                   next != bg &&
                   next != can[i].color &&
                   k < 2)
            {
                can[i].heading = frand() & 0x7;

                x = can[i].body[tail].x;
                y = can[i].body[tail].y;

                x = x + headings[can[i].heading].x;
                y = y + headings[can[i].heading].y;

                next = gfx.getPixel(x, y);
                k++;
            }

            if (x < 0) {
                x = gfx.maxx();
            }

            if (x > gfx.maxx()) {
                x = 0;
            }

            if (y < 0) {
                y = gfx.maxy();
            }

            if (y > gfx.maxy()) {
                y = 0;
            }

            can[i].body[head].x = x;
            can[i].body[head].y = y;

            gfx.putPixel(x, y, can[i].color);
        }

        gfx.drawText(8, 8, gfx.color(c_white), gfx.modeName());

        if(fps.tick()) {
            gfx.drawText(8, gfx.maxy() - gfx.fontHeight() - 8, gfx.color(c_blue), fps.bkbuf);
            gfx.drawText(8, gfx.maxy() - gfx.fontHeight() - 8, gfx.color(c_white), fps.buf);
        }
    }
    getch();
}


//---------------------------------------------------
//
// Demo of line drawing. Gives a feel for the effects
// of different pixel sizes.
// Code by Robert C. Pendleton

void demoLine()
{
    int activePage = 1;
    int x, y;
    int red = gfx.color(c_red);
    int green = gfx.color(c_green);

    gfx.setVisiblePage(0);

    FPS fps;

    while (!kbhit()) {
        x = 0;
        while (!kbhit() && x < gfx.maxx()) {
            gfx.setActivePage(activePage);
            gfx.clear(0);

            gfx.drawLine(x, 0, gfx.maxx() - x, gfx.maxy(), red);
            gfx.drawLine(gfx.maxx() - x, 0, x, gfx.maxy(), green);

            gfx.drawText(8, 8, gfx.color(c_white), gfx.modeName());
            gfx.drawText(8, gfx.maxy() - gfx.fontHeight() - 8, gfx.color(c_white), fps.buf);

            gfx.setVisiblePage(activePage);
            activePage++;

            x++;

            fps.tick();
        }

        y = 0;
        while (!kbhit() && y < gfx.maxy()) {
            gfx.setActivePage(activePage);
            gfx.clear(0);

            gfx.drawLine(gfx.maxx(), y, 0, gfx.maxy() - y, red);
            gfx.drawLine(gfx.maxx(), gfx.maxy() - y, 0, y, green);

            gfx.drawText(8, 8, gfx.color(c_white), gfx.modeName());
            gfx.drawText(8, gfx.maxy() - gfx.fontHeight() - 8, gfx.color(c_white), fps.buf);

            gfx.setVisiblePage(activePage);
            activePage++;

            y++;

            fps.tick();
        }
    }

    getch();
}

//---------------------------------------------------
//
// Demo of circles drawing.
// Useful to check aspect ratio.
//
void demoCircle()
{
    gfx.setActivePage(0);
    gfx.clear(0);
    gfx.setVisiblePage(0);

    int radius = 0;
    if(gfx.width() < gfx.height()) {
        radius = gfx.width()/2;
    } else {
        radius = gfx.height()/2;
    }
    int cx = gfx.width()/2 - 1;
    int cy = gfx.height()/2 - 1;

    uint8_t color = 0;
    FPS fps;
    while(!kbhit()) {
        for(int i=1; i<10; i++) {
            gfx.drawCircle(cx, cy, radius/i, color+i);
        }
        color++;
        gfx.drawText(8, 8, gfx.color(c_white), gfx.modeName());
        if(fps.tick()) {
            gfx.drawText(8, gfx.maxy() - gfx.fontHeight() - 8, gfx.color(c_black), fps.bkbuf);
            gfx.drawText(8, gfx.maxy() - gfx.fontHeight() - 8, gfx.color(c_white), fps.buf);
        }
    }
    getch();
}

//---------------------------------------------------
//
// Draw the color palette.
//
void demoPalette()
{
    gfx.setActivePage(0);
    gfx.clear(0);
    gfx.setVisiblePage(0);

    int16_t c = gfx.colors();
    //char buf[4];
    //memset(buf, 0, 4);
    if(c <= 16) {
        for(int i=0; i<c; i++) {
            gfx.clear(gfx.height()/c * i, gfx.height()/c, gfx.palidx(i));
        }
        /*
        for(int i=0; i<c; i++) {
            int x = 0;
            int y = screen.height()/c * i;
            int c = screen.getPixel(x, y);
            sprintf(buf, "%d", c);
            screen.drawText(x, y, c_white, buf);
        }
        */
    } else {
        // 256 colors
        int w = gfx.width()/16;
        int h = gfx.height()/16;
        for(int y=0; y<16; y++) {
            for(int x=0; x<16; x++) {
                if(gfx.chained()) {
                    gfx.fillRect8chained(x*w, y*h, w, h, y*16+x);
                } else {
                    gfx.fillRect8(x*w, y*h, w, h, y*16+x);
                }
            }
        }
    }

    gfx.drawRectangle(0, 0, gfx.width(), gfx.height(), gfx.color(c_white));
    gfx.drawText(8, 8, gfx.color(c_white), gfx.modeName());

    while(!kbhit()) {}
    getch();
}
