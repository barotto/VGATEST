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
#include <string.h>

#include "common.h"
#include "utils.h"

bool isVga()
{
    // if function 1axxH is supported then we have a vga
    union REGS rg;
    memset(&rg, 0, sizeof(rg));
    rg.w.ax = 0x1a00;
    int386(0x10, &rg, &rg);
    return (rg.h.al == 0x1a);
}

uint8_t far * getFont(int size)
{
    union REGPACK rg;
    memset(&rg, 0, sizeof(rg));
    rg.w.ax = 0x1130;
    switch(size) {
        default:
        case 8:
            rg.h.bh = 0x03;
            break;
        case 14:
            rg.h.bh = 0x02;
            break;
        case 16:
            rg.h.bh = 0x06;
            break;
    }
    intr(0x10, &rg);
    uint32_t seg = rg.w.es;
    uint32_t off = rg.w.bp;

    return (uint8_t far *)MK_FP(seg, off);
}

uint8_t getBIOSMode()
{
    union REGS rg;
    memset(&rg, 0, sizeof(rg));
    rg.h.ah = 0x0f;
    int386(0x10, &rg, &rg);

    return rg.h.al;
}

void setBIOSMode(uint8_t mode)
{
    union REGS rg;
    memset(&rg, 0, sizeof(rg));
    rg.h.ah = 0x00;
    rg.h.al = mode;
    int386(0x10, &rg, &rg);
}

void setVGARegisters(uint16_t baseAddr, const int16_t *regs)
{
    do {
        outpw(baseAddr, *regs++);
    } while(*regs != -1);
}

void unlockCRTC(int16_t baseAddr)
{
    outp(baseAddr, 0x11);
    int crtc11 = inp(baseAddr+1) & 0x7f; // Protect Registers 0-7 = 0
    outp(baseAddr+1, crtc11);
}

void setSplitScreen(uint16_t baseAddr, uint16_t scanline)
{
    CRTC_OUT(baseAddr, CRTC_LINE_COMPARE, (scanline&0xff));

    int overflow;
    CRTC_IN(baseAddr, CRTC_OVERFLOW, overflow);
    overflow &= ~0x10;
    overflow |= (scanline>>4) & 0x10;
    CRTC_OUT(baseAddr, CRTC_OVERFLOW, overflow);

    int msl;
    CRTC_IN(baseAddr, CRTC_MAX_SCANLINE, msl);
    msl &= ~0x40;
    msl |= (scanline>>3) & 0x40;
    CRTC_OUT(baseAddr, CRTC_MAX_SCANLINE, msl);
}

void setStartAddress(uint16_t baseAddr, uint16_t address)
{
    CRTC_OUT(baseAddr, CRTC_STARTADDR_HI, (address & 0xff00) >> 8);
    CRTC_OUT(baseAddr, CRTC_STARTADDR_LO, (address & 0x00ff));
}

uint8_t readRegister(uint16_t baseAddr, uint8_t reg)
{
    uint8_t regval;
    
    outp(baseAddr, reg);
    regval = inp(baseAddr + 1);
    
    return regval;
}

bool toggleRegisterBit(uint16_t baseAddr, uint8_t reg, uint8_t bit)
{
    if((baseAddr == CRTC_ADDR_MONO || baseAddr == CRTC_ADDR_COL) && reg <= CRTC_OVERFLOW) {
        unlockCRTC(baseAddr);
    }
    
    uint8_t regval = readRegister(baseAddr, reg);
    
    uint8_t mask = (1 << bit);
    bool bitval = !(regval & mask);
    regval &= ~mask;
    regval |= bitval << bit;
    
    outp(baseAddr, reg);
    outp(baseAddr+1, regval);
    
    return bitval;
}
