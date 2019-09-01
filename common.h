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

#ifndef _COMMON_H_
#define _COMMON_H_

#include <dos.h>
#include <stdint.h>

typedef enum {
    k_ESC         = 27,
    k_SPACE       = 32,
    k_HOME        = 71,
    k_END         = 79,
    k_PAGE_UP     = 73,
    k_PAGE_DOWN   = 81,
    k_UP_ARROW    = 72,
    k_LEFT_ARROW  = 75,
    k_DOWN_ARROW  = 80,
    k_RIGHT_ARROW = 77,
    k_F1          = 59,
    k_F2          = 60,
    k_F3          = 61,
    k_F4          = 62,
    k_F5          = 63,
    k_F6          = 64,
    k_F7          = 65,
    k_F8          = 66,
    k_F9          = 67,
    k_F10         = 68
} Keys;

typedef enum {
    c_black   = 0x0,
    c_blue    = 0x1,
    c_green   = 0x2,
    c_cyan    = 0x3,
    c_red     = 0x4,
    c_magenta = 0x5,
    c_brown   = 0x6,
    c_lgray   = 0x7,
    c_dgray   = 0x8,
    c_lblue   = 0x9,
    c_lgreen  = 0xa,
    c_lcyan   = 0xb,
    c_lred    = 0xc,
    c_pink    = 0xd,
    c_yellow  = 0xe,
    c_white   = 0xf
} VGAColors;

typedef enum {
    e_none,                     // no errors
    e_notVgaDisplay,            // it isn't a vga compatible display
    e_modeNotSupported,         // requested mode is not supported
    e_badPage,                  // invalid page requested
} Errors;


/*
Modes 0, 2, and 4 are identical to modes 1, 3, and 5, respectively. On the CGA
there is a difference in these modes. In modes 0, 2, and 4, the color burst was
turned off (giving only shades of gray) but color burst is not provided by the
VGA.
Mode hex 3+ is the default mode with an analog color display attached to the
system. Mode hex 7+ is the default mode with an analog monochrome display.
Modes 0*, 1*, 2*, and 3* emulate the support provided by the EGA.
*/
enum textMode {
    // BIOS
    t_b40x25_8x8_00h  = 0x00, //0  320x200  16  B800, same as mode 1
    t_b40x25_8x14_00h = 0xa0, //0* 320x350  16  B800, same as mode 1*
    t_b40x25_8x16_00h = 0xb0, //   320x400  16  B800
    t_b40x25_9x16_00h = 0xc0, //0+ 360x400  16  B800, same as mode 1+
    t_b40x25_8x8_01h  = 0x01, //1  320x200  16  B800
    t_b40x25_8x14_01h = 0xa1, //1* 320x350  16  B800
    t_b40x25_8x16_01h = 0xb1, //   320x400  16  B800
    t_b40x25_9x16_01h = 0xc1, //1+ 360x400  16  B800
    t_b80x25_8x8_02h  = 0x02, //2  640x200  16  B800, same as mode 3
    t_b80x25_8x14_02h = 0xa2, //2* 640x350  16  B800, same as mode 3*
    t_b80x25_8x16_02h = 0xb2, //   640x400  16  B800
    t_b80x25_9x16_02h = 0xc2, //2+ 720x400  16  B800, same as mode 3+
    t_b80x25_8x8_03h  = 0x03, //3  640x200  16  B800
    t_b80x25_8x14_03h = 0xa3, //3* 640x350  16  B800
    t_b80x25_8x16_03h = 0xb3, //   640x400  16  B800
    t_b80x25_9x16_03h = 0xc3, //3+ 720x400  16  B800
    t_b80x25_9x14_07h = 0x07, //7  720x350 mono B000
    t_b80x25_9x16_07h = 0xa7, //7+ 720x400 mono B000
    // Tweaked
    t_t80x43_8x8  = 0x1a, // 640x350
    t_t80x50_9x8  = 0x1b, // 720x400
    t_t80x28_9x14 = 0x1c, // 720x400
    t_t80x30_8x16 = 0x1d, // 640x480
    t_t80x34_8x14 = 0x1e, // 640x480
    t_t80x60_8x8  = 0x1f, // 640x480
};
enum videoModes {
    // BIOS modes
    v_b320x200_04h = 0x04,
    v_b640x200_06h = 0x06,
    v_b320x200_0Dh = 0x0D,
    v_b640x200_0Eh = 0x0E,
    v_b640x350_0Fh = 0x0F,
    v_b640x350_10h = 0x10,
    v_b640x350_11h = 0x11,
    v_b640x480_12h = 0x12,
    v_b320x200_13h = 0x13,
    // Tweaked modes
    v_t160x120     = 0x14,
    v_t256x256_Q   = 0x15,
    v_t296x220     = 0x16,
    v_t320x200_Y   = 0x17,
    v_t320x240_X   = 0x18,
    v_t320x400     = 0x19,
    v_t360x270     = 0x1a,
    v_t360x360     = 0x1b,
    v_t360x480     = 0x1c,
    v_t400x300     = 0x1d,
};

#define ISR1_ADDR_MONO 0x3ba
#define ISR1_ADDR_COL  0x3da

#define MOR_ADDR  (0x3c2)           // misc. output register
#define MOR_READ  (0x3cc)           // misc. output register

#define SEQ_ADDR  (0x3c4)           // base port of the Sequencer
#define SEQ_DATA  (0x3c5)           // data port of the Sequencer
#define SEQ_RESET    0x00
#define SEQ_CLOCKING 0x01
#define SEQ_MAPMASK  0x02
#define SEQ_CHARMAP  0x03
#define SEQ_MEMMODE  0x04
#define SEQ_OUT(_REG_,_VAL_) { outpw(SEQ_ADDR, (_VAL_<<8)|_REG_); }
#define SEQ_IN(_REG_,_VAR_) { outp(SEQ_ADDR, _REG_); _VAR_ = inp(SEQ_DATA); }

#define CRTC_ADDR_MONO 0x3b4
#define CRTC_ADDR_COL  0x3d4
#define CRTC_DATA_MONO 0x3b5
#define CRTC_DATA_COL  0x3d5
#define CRTC_HTOTAL          0x00
#define CRTC_HDISPLAY_END    0x01
#define CRTC_START_HBLANK    0x02
#define CRTC_END_HBLANK      0x03
#define CRTC_START_HRETRACE  0x04
#define CRTC_END_HRETRACE    0x05
#define CRTC_VTOTAL          0x06
#define CRTC_OVERFLOW        0x07
#define CRTC_PRESET_ROW_SCAN 0x08
#define CRTC_MAX_SCANLINE    0x09
#define CRTC_CURSOR_START    0x0A
#define CRTC_CURSOR_END      0x0B
#define CRTC_STARTADDR_HI    0x0C
#define CRTC_STARTADDR_LO    0x0D
#define CRTC_CURSOR_HI       0x0E
#define CRTC_CURSOR_LO       0x0F
#define CRTC_VRETRACE_START  0x10
#define CRTC_VRETRACE_END    0x11
#define CRTC_VDISPLAY_END    0x12
#define CRTC_OFFSET          0x13
#define CRTC_UNDERLINE       0x14
#define CRTC_START_VBLANK    0x15
#define CRTC_END_VBLANK      0x16
#define CRTC_MODE_CONTROL    0x17
#define CRTC_LINE_COMPARE    0x18
#define CRTC_OUT(_APORT_,_REG_,_VAL_) { outpw(_APORT_, ((_VAL_)<<8)|_REG_); }
#define CRTC_OUT_COL(_REG_,_VAL_) { outpw(CRTC_ADDR_COL, ((_VAL_)<<8)|_REG_); }
#define CRTC_IN(_APORT_,_REG_,_VAR_) { outp(_APORT_, _REG_); _VAR_ = inp(_APORT_+1); }

#define GCR_ADDR  (0x3ce)           // graphics controller address register
#define GCR_DATA  (0x3cf)           // graphics controller data register
#define GCR_SETRESET    0x00
#define GCR_EN_SETRESET 0x01
#define GCR_COL_COMPARE 0x02
#define GCR_ROTATE      0x03
#define GCR_READMAP_SEL 0x04
#define GCR_GFX_MODE    0x05
#define GCR_MISC        0x06
#define GCR_COL_DONTC   0x07
#define GCR_BITMASK     0x08
#define GCR_OUT(_REG_,_VAL_) { outpw(GCR_ADDR, (_VAL_<<8)|_REG_); }
#define GCR_IN(_REG_,_VAR_) { outp(GCR_ADDR, _REG_); _VAR_ = inp(GCR_DATA); }

#define ACR_ADDR  (0x3c0)           // attribute registers
#define ACR_DATA  (0x3c1)           // attribute registers
#define ACR_ATTMODE  0x10           // Attribute Mode Control Register
#define ACR_OVERSCAN 0x11           // Overscan Color Register
#define ACR_COLPLANE 0x12           // Color Plane Enable Register
#define ACR_HPELPAN  0x13           // Horizontal PEL Panning Register
#define ACR_COLSEL   0x14           // Color Select Register
#define ACR_OUT_COL(_REG_,_VAL_) { \
    inp(0x3da); \
    outp(0x3c0, _REG_ | 0x20); \
    outp(0x3c0, _VAL_); \
}

#define ACR_OUT(_ISRA_,_REG_,_VAL_) { \
    inp(_ISRA_); \
    outp(ACR_ADDR, _REG_ | 0x20); \
    outp(ACR_ADDR, _VAL_); \
}
#define ACR_IN(_ISRA_,_REG_,_VAR_) { \
    inp(_ISRA_); \
    outp(ACR_ADDR, _REG_ | 0x20); \
    _VAR_ = inp(ACR_DATA); \
}

#define PAL_WRITE_ADDR (0x3c8)      // palette write address
#define PAL_READ_ADDR  (0x3c7)      // palette write address
#define PAL_DATA       (0x3c9)      // palette data register

#endif
