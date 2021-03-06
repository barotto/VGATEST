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

#ifndef _FPS_H_
#define _FPS_H_

#include <time.h>
#include <string.h>
#include <stdio.h>

class FPS
{
public:
    float fps;
private:
    char buf[8];
    char bkbuf[8];
    clock_t t0, t1;
    unsigned long frames;

public:
    FPS()
    {
        fps = .0f;
        buf[0] = 0;
        t0 = clock();
        t1 = t0;
        frames = 0;
        memset(bkbuf, 219, 7);
        bkbuf[7] = 0;
    }

    bool tick()
    {
        t1 = clock();
        frames++;
        clock_t elapsed = t1 - t0;
        if(elapsed >= CLOCKS_PER_SEC) {
            t0 = t1;
            fps = float(frames * CLOCKS_PER_SEC) / float(elapsed);
            frames = 0;
            return true;
        }
        return false;
    }

    inline const char *resultStr()
    {
        if(frames == 0) {
            snprintf(buf, 8, "%.2f", fps);
        }
        return buf;
    }

    inline const char *backStr() const
    {
        return bkbuf;
    }
};

#endif
