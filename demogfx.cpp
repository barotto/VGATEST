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
#include <stdlib.h>

#include "vgatest.h"
#include "fps.h"

void demoGfxDecodeSpecialKey(int key)
{
    gfx.drawLine(0, 0, 20, 0, c_white);
    
    int16_t addr = 0;
    uint8_t reg = 0;
    switch(key) {
        case k_F1:
            addr = gfx.CRTCAddress();
            reg = CRTC_MODE_CONTROL;
            break;
        case k_F2:
            addr = gfx.CRTCAddress();
            reg = CRTC_UNDERLINE;
            break;
        default:
            break;
    }
    if(addr && reg) {
        
        gfx.drawLine(0, 0, 1, 0, c_red);
        
        uint8_t regval = readRegister(addr, reg);
        for(int i=0; i<8; i++) {
            if(regval & 1) {
                gfx.putPixel(3+i*2, 0, c_green);
            } else {
                gfx.putPixel(3+i*2, 0, c_lblue);
            }
            regval >>= 1;
        }
        key = getch();
        switch(key) {
            case '0': case '1': case '2': case '3':
            case '4': case '5': case '6': case '7': {
                int bit = key - '0';
                bool newval = toggleRegisterBit(addr, reg, bit);
                gfx.putPixel(3+bit*2, 0, (newval)?c_green:c_lblue);
                break;
            }
            default:
                break;
        }
    }
    
    gfx.drawLine(0, 0, 1, 0, c_white);
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
            gfx.drawText(8, gfx.maxy() - gfx.fontHeight() - 8, gfx.color(c_blue), fps.backStr());
            gfx.drawText(8, gfx.maxy() - gfx.fontHeight() - 8, gfx.color(c_white), fps.resultStr());
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
            gfx.drawText(8, gfx.maxy() - gfx.fontHeight() - 8, gfx.color(c_white), fps.resultStr());

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
            gfx.drawText(8, gfx.maxy() - gfx.fontHeight() - 8, gfx.color(c_white), fps.resultStr());

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
            gfx.drawText(8, gfx.maxy() - gfx.fontHeight() - 8, gfx.color(c_black), fps.backStr());
            gfx.drawText(8, gfx.maxy() - gfx.fontHeight() - 8, gfx.color(c_white), fps.resultStr());
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


//---------------------------------------------------
//
// Splits the screen in half
//

void demoGfxSpitscreen_setScanline(int &scanline, int txtcol, int txtline)
{
    char buf[4];
    char bkg[4] = { 219,219,219,0 };
    
    if(scanline < 0) {
        scanline = 0;
    }
    if(scanline > gfx.scanlines()-1) {
        scanline = gfx.scanlines()-1;
    }
    
    gfx.drawText(txtcol, txtline, gfx.color(c_blue), bkg);
    snprintf(buf, 4, "%03d", scanline);
    gfx.drawText(txtcol, txtline, gfx.color(c_white), buf);
    
    gfx.setSplitScreen(scanline);
    
}

void demoGfxSpitscreen_setHPan(int &hpan, int txtcol, int txtline)
{
    char buf[3];
    char bkg[3] = { 219,219,0 };

    if(hpan < 0) {
        hpan = 0;
    }
    if(hpan > gfx.width()) {
        hpan = gfx.width();
    }
    
    gfx.drawText(txtcol, txtline, gfx.color(c_blue), bkg);
    snprintf(buf, 3, "%02d", hpan);
    gfx.drawText(txtcol, txtline, gfx.color(c_white), buf);
    
    gfx.setHPanning(hpan);
}

void demoGfxSpitscreen_setVPan(int &vpan, int txtcol, int txtline)
{
    char buf[3];
    char bkg[3] = { 219,219,0 };

    if(vpan < 0) {
        vpan = 0;
    }
    if(vpan > gfx.height()) {
        vpan = gfx.height();
    }
    
    gfx.drawText(txtcol, txtline, gfx.color(c_blue), bkg);
    snprintf(buf, 3, "%02d", vpan);
    gfx.drawText(txtcol, txtline, gfx.color(c_white), buf);
    
    gfx.setVPanning(vpan);
}

void demoGfxSpitscreen_setStartAddress(int &addr, int txtcol, int txtline)
{
    char buf[5];
    char bkg[5] = { 219,219,219,219,0 };
    
    if(addr < 0) {
        addr = 0;
    }
    gfx.drawText(txtcol, txtline, gfx.color(c_blue), bkg);
    snprintf(buf, 5, "%04x", addr);
    gfx.drawText(txtcol, txtline, gfx.color(c_white), buf);
    
    gfx.setStartAddress(addr);
}

void demoGfxSpitscreen_setPanMode(bool &mode, int txtcol, int txtline)
{
    char buf[2];
    char bkg[2] = { 219,0 };
    
    gfx.drawText(txtcol, txtline, gfx.color(c_blue), bkg);
    snprintf(buf, 2, "%d", mode);
    gfx.drawText(txtcol, txtline, gfx.color(c_white), buf);
    
    gfx.setPanningMode(mode);
}

void demoGfxSpitscreen()
{
    gfx.setActivePage(0);
    gfx.clear(0);
    gfx.setVisiblePage(0);
    
    int16_t c = gfx.colors();
    if(c <= 16) {
        for(int i=0; i<c; i++) {
            gfx.clear(gfx.height()/c * i, gfx.height()/c, gfx.palidx(i));
        }
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
    
    int line[5];
    for(int i=0; i<5; i++) {
        line[i] = 22 + i*12;
    }
    const int col = 88;
    
    gfx.drawText(8, line[0], gfx.color(c_white), " scanline=");
    gfx.drawText(8, line[1], gfx.color(c_white), "horiz.pan=");
    gfx.drawText(8, line[2], gfx.color(c_white), " vert.pan=");
    gfx.drawText(8, line[3], gfx.color(c_white), "startaddr=");
    gfx.drawText(8, line[4], gfx.color(c_white), " pan mode=");
    
    int scanline = (gfx.scanlines() / 2) - 1;
    demoGfxSpitscreen_setScanline(scanline, col, line[0]);
    
    int vpan = 0, hpan = 0;
    demoGfxSpitscreen_setHPan(hpan, col, line[1]);
    demoGfxSpitscreen_setVPan(vpan, col, line[2]);
    
    int addr = 0;
    demoGfxSpitscreen_setStartAddress(addr, col, line[3]);
    
    bool panmode = 0;
    demoGfxSpitscreen_setPanMode(panmode, col, line[4]);
    
    int k = getch();
    while(k != k_ESC) {
        if(k == 0 || k == 224) {
            k = getch();
            switch(k) { // the real value
                case k_PAGE_UP: 
                    scanline--;
                    demoGfxSpitscreen_setScanline(scanline, col, line[0]);
                    break;
                case k_PAGE_DOWN: 
                    scanline++;
                    demoGfxSpitscreen_setScanline(scanline, col, line[0]);
                    break;
                case k_UP_ARROW: 
                    vpan++;
                    demoGfxSpitscreen_setVPan(vpan, col, line[2]);
                    break;
                case k_DOWN_ARROW: 
                    vpan--;
                    demoGfxSpitscreen_setVPan(vpan, col, line[2]);
                    break;
                case k_LEFT_ARROW:
                    hpan--;
                    demoGfxSpitscreen_setHPan(hpan, col, line[1]);
                    break;
                case k_RIGHT_ARROW:
                    hpan++;
                    demoGfxSpitscreen_setHPan(hpan, col, line[1]);
                    break;
                default:
                    demoGfxDecodeSpecialKey(k);
                    break;
            }
        } else {
            switch(k) {
                case '+': 
                    addr++;
                    demoGfxSpitscreen_setStartAddress(addr, col, line[3]);
                    break;
                case '-': 
                    addr--;
                    demoGfxSpitscreen_setStartAddress(addr, col, line[3]);
                    break;
                case 'm':
                case 'M':
                    panmode = !panmode;
                    demoGfxSpitscreen_setPanMode(panmode, col, line[4]);
                    break;
                default:
                    break;
            }
        }
        k = getch();
    }
}


//---------------------------------------------------
//
// Scrolls the screen horizontally.
//
void demoGfxHScrolling()
{
    gfx.setActivePage(0);
    gfx.clear(0);
    gfx.setVisiblePage(0);
    
    int x0 = (gfx.width() / 4) - 10;
    int x1 = gfx.width() - (gfx.width() / 4) - 10;
    for(int i=0; i<20; i+=2) {
        gfx.drawLine(x0+i, 0, x0+i, gfx.maxy(), gfx.color(c_yellow));
        gfx.drawLine(x1+i, 0, x1+i, gfx.maxy(), gfx.color(c_yellow));
    }
    
    gfx.drawText(8, 8, gfx.color(c_white), gfx.modeName());
    gfx.vsync();
    
    int k = 0;
    int hpan = 0;
    int startaddr = 0;
    int amount = 1;
    while(k != k_ESC) {
        hpan += amount;
        if(hpan > 7) {
            hpan = 0;
            startaddr += amount;
        }
        if(startaddr > gfx.lineOffset()) {
            startaddr = amount;
        }
        
        gfx.wait_disp_enable();
        gfx.setStartAddress(startaddr);
        gfx.vsync();
        gfx.setHPanning(hpan);
        
        if(kbhit()) {
            k = getch();
            if(k == 0 || k == 224) {
                k = getch();
            } else {
                switch(k) {
                    case '+':
                        amount++;
                        if(amount > 10) {
                            amount = 10;
                        }
                        break;
                    case '-': 
                        amount--;
                        if(amount < 0) {
                            amount = 0;
                        }
                        break;
                    default:
                        break;
                }
            }
        }
    }
}