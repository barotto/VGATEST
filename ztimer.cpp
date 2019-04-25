/*
 * Porting of The precision Zen timer by Michael Abrash
 *
 * See Michael Abrash's Graphics Programming Black Book, Chapter 3
 */

#include "stdint.h"
#include "ztimer.h"
#include <math.h>

// the last result of ZTimerOff()
uint32_t ztimercount; // if > 2^16-1 then overflow happened (time elapsed > 53ms)
uint16_t ztimerref;

double zTimeToUs()
{
    double us;
    if(ztimercount > 65535) {
        us = NAN;
    } else {
        us = double(ztimercount - ztimerref) / 0.8381;
    }
    return us;
}
