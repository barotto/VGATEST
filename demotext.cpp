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

// Text Mode demos

#include <conio.h>
#include <stdio.h>
#include "vgatest.h"


void drawTextAxis()
{
    char buf[2];
    // columns axis
    for(int i=0; i<text.cols(); i++) {
        snprintf(buf, 2, "%d", i%10);
        text(0,i)(buf);
    }
    // rows axis
    for(int i=0; i<text.rows(); i++) {
        snprintf(buf, 2, "%d", i%10);
        text(i,0)(buf);
    }
}

void demoTextSplitscreen_setStartAddress(int &addr, int row, int col)
{
    if(addr < 0) {
        addr = 0;
    }
    addr &= 0xffff;
    
    char buf[10];
    snprintf(buf, 10, "addr=%04d", addr);
    text(row,col)(buf,c_white,c_black);
    
    text.setStartAddress(addr);
}

void demoTextSplitscreen_setPanning(int &hpan, int &vpan, int row, int col)
{
    char buf[10];
    
    if(hpan < 0) {
        hpan = 0;
    }
    if(hpan > 15) {
        hpan = 15;
    }
    snprintf(buf, 10, "hpan=%02d", hpan);
    text(row,col)(buf,c_white,c_black);
    
    if(vpan < 0) {
        vpan = 0;
    }
    if(vpan > (text.boxh()-1)) {
        vpan = (text.boxh()-1);
    }
    snprintf(buf, 10, "vpan=%02d", vpan);
    text(row,col+8)(buf,c_white,c_black);

    text.setPanning(hpan, vpan);
}

void demoTextSplitscreen_setSplitscreen(int &scanline, int row, int col)
{
    if(scanline < 0) {
        scanline = 0;
    }
    if(scanline > text.scanlines()-1) {
        scanline = text.scanlines()-1;
    }
    char buf[13];
    snprintf(buf, 13, "scanline=%03d", scanline);
    text(row,col)(buf,c_white,c_black);
    
    text.setSplitScreen(scanline);
}

void demoTextSplitscreen()
{
    text.setColor(c_brown);
    drawTextAxis();

    text(1,1)(text.modeName(),c_lgray);

    int scanline = text.scanlines()/2;
    int hpan = text.getHPanning(), vpan = text.getVPanning();
    int addr = 0;
    
    demoTextSplitscreen_setSplitscreen(scanline, 2, 1);
    demoTextSplitscreen_setPanning(hpan, vpan, 2, 15);
    demoTextSplitscreen_setStartAddress(addr, 2, 32);

    text.setColor(c_white);
    char ch = 0;
    char color = 0;
    for(int y=3; y<text.rows(); y++) {
        for(int x=1; x<text.cols(); x++) {
            text.setColor((color++)%16, x%8);
            text(y,x)(ch++);
        }
    }

    int k = getch();
    while(k != k_ESC) {
        if(k == 0 || k == 224) {
            switch(getch()) { // the real value
                case k_PAGE_UP: 
                    scanline--;
                    demoTextSplitscreen_setSplitscreen(scanline, 2, 1);
                    break;
                case k_PAGE_DOWN: 
                    scanline++;
                    demoTextSplitscreen_setSplitscreen(scanline, 2, 1);
                    break;
                case k_UP_ARROW:
                    vpan++;
                    demoTextSplitscreen_setPanning(hpan, vpan, 2, 15);
                    break;
                case k_DOWN_ARROW:
                    vpan--;
                    demoTextSplitscreen_setPanning(hpan, vpan, 2, 15);
                    break;
                case k_LEFT_ARROW:
                    hpan--;
                    demoTextSplitscreen_setPanning(hpan, vpan, 2, 15);
                    break;
                case k_RIGHT_ARROW:
                    hpan++;
                    demoTextSplitscreen_setPanning(hpan, vpan, 2, 15);
                    break;
                default:
                    break;
            }
        } else {
            switch(k) {
                case '+': 
                    addr++;
                    demoTextSplitscreen_setStartAddress(addr, 2, 32);
                    break;
                case '-': 
                    addr--;
                    demoTextSplitscreen_setStartAddress(addr, 2, 32);
                    break;
                default:
                    break;
            }
        }
        k = getch();
    }
}

void drawCharMap(int r, int c, int color)
{
    int ch = 0;
    int w = 16;
    int h = 16;
    char buf[2];
    for(int i=0; i<16; i++) {
        snprintf(buf, 2, "%x", i);
        text(r,c+2+i)(buf);
        text(r+1,c+2+i)("-");
        text(r+2+i,c)(buf);
        text(r+2+i,c+1)(":");
    }

    text.setColor(color);
    for(int y=r; (y<r+h && ch<256); y++) {
        for(int x=c; (x<c+w && ch<256); x++,ch++) {
            //text.setColor(((y-r)%8)+8);
            text(y+2,x+2)(ch);
        }
    }
}

void demoTextFontMaps()
{
    char buf[4];

    text.setColor(c_brown);
    drawTextAxis();

    int ro = 1;
    int co = 1;

    // write the mode name
    text(ro++,co)(text.modeName(),c_lgray);

    int w = 16;
    int h = 16;

    // print Map A characters
    text.setColor(c_lgray);
    text.drawBox(ro, co, w+1+2, h+1+2)("Map A");
    int mapselR = text.getRow();
    int mapselC = text.getCol()+1;
    drawCharMap(ro+1, co+1, c_white);

    // print Map B characters
    text.setColor(c_lgray);
    text.drawBox(ro, co+3+(text.cols()==40?0:1)+w, w+1+2, h+1+2)("Map B");
    drawCharMap(ro+1, co+4+(text.cols()==40?0:1)+w, c_cyan);

    // print Background and Foreground combinations
    text.setColor(c_lgray);
    text.drawBox(ro+h+4, co, 17, 2)("Background");
    text.drawBox(ro+h+4, co+17+1, 17, 2)("Foreground");
    for(int i=0; i<16; i++) {
        text.setColor(c_white,i);
        text(ro+h+5,co+i+1)(7);//176);
        text.setColor(i,c_black);
        text(ro+h+5,co+i+3+16)(0xdb);//8);//178);
    }

    text.setColor(c_lgray,c_black);

    bool exit = false;
    int mapA = 0;
    int mapAsel = 0;
    int mapCount = 8;
    snprintf(buf, 2, "%d", mapA);
    text(mapselR,mapselC)(buf);

    while(!exit) {
        char ch = getch();
        switch(ch) {
            case k_ESC:
                exit = true;
                break;
            case 48: case 49: case 50: case 51: case 52: case 53: case 54: case 55:
            {
                // switch between maps
                mapA = ch-48;
                if(mapA > 3) {
                    mapAsel = ((mapA&3) << 2) | 0x20;
                } else {
                    mapAsel = (mapA << 2);
                }
                //see Int 10/AX=1103h
                //union REGS rg;
                //rg.h.ah = 0x11;
                //rg.h.al = 0x03;
                //rg.h.bl = mapAsel;
                //int386(0x10, &rg, &rg);
                SEQ_OUT(3, mapAsel);
                snprintf(buf, 2, "%d", mapA);
                text(mapselR,mapselC)(buf);
                break;
            }
            default:
                break;
        }
    }
}
