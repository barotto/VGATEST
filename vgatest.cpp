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

#include <conio.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "vgatest.h"
#include "demotext.h"
#include "demogfx.h"


GfxScreen gfx;
TextScreen text;


int getCharFromKeyb(const char* allowed, int promptrow, int promptcol)
{
    int c = ' ';
    size_t len = strlen(allowed);
    while(NULL == memchr(allowed, c, len)) {
        text.moveCursor(promptrow, promptcol);
        c = getche();
        if(c == k_ESC) {
            c = 'q';
            break;
        }
        c = tolower(c);
    }
    return c;
}

int getHexFromKeyb(int row, int col)
{
    int hex1 = -1;
    int hex2 = -1;
    int hex = -1;
    const char *hexch = "0123456789abcdef";

    // first digit
    hex1 = getCharFromKeyb(hexch, row, col);
    if(hex1 == 'q') {
        return 0xFFFF;
    }
    if(hex1 >= 'a') {
        hex1 = (hex1-'a')+10;
    } else {
        hex1 -= '0';
    }

    // second digit
    hex2 = getCharFromKeyb(hexch, row, col+1);
    if(hex2 == 'q') {
        return 0xFFFF;
    }
    if(hex2 >= 'a') {
        hex2 = (hex2-'a')+10;
    } else {
        hex2 -= '0';
    }

    hex = hex1*16 + hex2;

    return hex;
}

void putTitle()
{
    text(2,28)("-", c_dgray)("-", c_lgray)("-", c_white)
              (" VGA modes test ", c_white)
              ("-", c_white)("-", c_lgray)("-", c_dgray);
}

int main(int argc, char *argv[])
{
    if (gfx.error() != e_none) {
        if(gfx.error() == e_notVgaDisplay) {
            printf("This is not a VGA compatible display.\n");
        } else {
            printf("An error occurred.\n");
        }
        return 1;
    }

    char demo = 0;
    uint8_t a;
    int promptrow, promptcol;

    while(true) {
        text.erasePage(DEFAULT_FG_COL, DEFAULT_BG_COL);
        putTitle();

        text(4,33);
        text("Text     [t]\n", DEFAULT_FG_COL);
        text("Graphics [g]\n");
        text("Options  [o]\n");
        text(text.getRow()+1, 33);
        text("Which Mode?");
        text.getPos(promptrow, promptcol);

        int mode = getCharFromKeyb("tTgGoO", promptrow, promptcol);
        if(mode == 'q') {
            break;
        }

        if(mode == 'o') {
            text(promptrow+3,33)("Overscan color [o]\n");
            text(text.getRow()+1, 33);
            text("Which option?");
            text.getPos(promptrow, promptcol);
            int opt = getCharFromKeyb("oO", promptrow, promptcol);
            if(opt == 'q') {
                continue;
            }
            switch(opt) {
                case 'o': {
                    text(text.getRow()+1, 33);
                    text("Palette index (hex)?");
                    text.getPos(promptrow, promptcol);
                    int pal = getHexFromKeyb(promptrow, promptcol);
                    if(pal > 0xff) {
                        continue;
                    }
                    text.setOverscanColor(pal);
                    gfx.setOverscanColor(pal);
                    break;
                }
                default:
                    continue;
            }
        }

        if(mode == 't') {

            text.erasePage(DEFAULT_FG_COL, DEFAULT_BG_COL);
            putTitle();
            text(4,33);
            text("Font Maps   [f]\n", DEFAULT_FG_COL);
            text("Split & Pan [s]\n");
            text(text.getRow()+1, 33);
            text("Which Test?");
            text.getPos(promptrow, promptcol);

            int demo = getCharFromKeyb("fFsS", promptrow, promptcol);
            if(demo == 'q') {
                continue;
            }

            int modesrow = text.setRow(text.getRow()+2);
            int askrow = modesrow;

            text(modesrow, 16)("BIOS modes\n");
            text("0,1   40x25 8x8  [01]\n");
            text("0*,1* 40x25 8x14 [a1]\n");
            text("0+,1+ 40x25 9x16 [c1]\n");
            text("2,3   80x25 8x8  [03]\n");
            text("2*,3* 80x25 8x14 [a3]\n");
            text("2+,3+ 80x25 9x16 [c3]\n");
            text("7     80x25 9x14 [07]\n");
            text("7+    80x25 9x16 [a7]\n");
            askrow = (text.getRow()>askrow)?text.getRow():askrow;

            text(modesrow, 42)("Tweaked modes\n");
            text("  80x43 8x8  [1a]\n");
            text("  80x50 9x8  [1b]\n");
            text("  80x28 9x14 [1c]\n");
            text("° 80x30 8x16 [1d]\n");
            text("° 80x34 8x14 [1e]\n");
            text("° 80x60 8x8  [1f]\n");
            askrow = (text.getRow()>askrow)?text.getRow():askrow;

            text(askrow+1, 33);
            text("Which Mode?");
            text.getPos(promptrow, promptcol);

            text(23,50)("° = multisync monitor req.");

            do {
                int mode = getHexFromKeyb(promptrow, promptcol);
                if(mode == 0xFFFF) {
                    demo = 'q';
                    break;
                }
                text.setMode(mode);
            } while(text.error());

            switch(demo) {
                case 'f':
                    demoTextFontMaps();
                    break;
                case 's':
                    demoTextSplitscreen();
                    break;
            }

            text.resetMode();
        }

        if(mode == 'g') {

            text.erasePage(DEFAULT_FG_COL, DEFAULT_BG_COL);
            putTitle();
            text(4,33);
            text("Circles [c]\n", DEFAULT_FG_COL);
            text("Lines   [l]\n");
            text("Palette [p]\n");
            text("Worms   [w]\n");
            text("Bench   [b]\n");
            text(text.getRow()+1, 33);
            text("Which Test?");
            text.getPos(promptrow, promptcol);

            int demo = getCharFromKeyb("cClLpPwWbB", promptrow, promptcol);
            if(demo == 'q') {
                continue;
            }

            int modesrow = text.setRow(text.getRow()+2);
            text(modesrow, 6)("BIOS modes\n");
            text(" Dh 320x200 [0d]\n");
            text(" Eh 640x200 [0e]\n");
            text(" Fh 640x350 [0f]\n");
            text("10h 640x350 [10]\n");
            text("12h 640x480 [12]\n");
            text("13h 320x200 [13]\n");

            text(modesrow, 28)("Tweaked 256-color modes\n");
            text("* 160x120 planar [14]\n");
            text("Q 256x256 chain4 [15]\n");
            text("  296x220 planar [16]\n");
            text("Y 320x200 planar [17]\n");
            text("X 320x240 planar [18]\n");
            text("  320x400 planar [19]\n");

            text(modesrow+1, 50);
            text("  360x270 planar [1a]\n");
            text("  360x360 planar [1b]\n");
            text("  360x480 planar [1c]\n");
            text("* 400x300 planar [1d]\n");

            text(text.getRow()+3, 33);
            text("Which Mode?");
            text.getPos(promptrow, promptcol);

            text(23,50)("* = multisync monitor req.");

            int mode1 = -1;
            int mode2 = -1;
            int mode = -1;
            do {
                int mode = getHexFromKeyb(promptrow, promptcol);
                if(mode == 0xFFFF) {
                    demo = 'q';
                    break;
                }
                gfx.setMode(mode);
            } while(gfx.error());

            switch(demo) {
                case 'w':
                    demoWorm();
                    break;
                case 'c':
                    demoCircle();
                    break;
                case 'l':
                    demoLine();
                    break;
                case 'p':
                    demoPalette();
                    break;
                case 'b':
                    demoMemBench();
                    break;
            }

            gfx.resetMode();
        }
    }

    text.erasePage(c_lgray, c_black);

    return 0;
}
