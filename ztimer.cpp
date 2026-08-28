/*
 * Porting of The precision Zen timer by Michael Abrash
 *
 * See Michael Abrash's Graphics Programming Black Book, Chapter 3
 */

#include <stdint.h>
#include <math.h>
#include "ztimer.h"

// the last result of ZTimerOff()
uint16_t zTimerCount;
uint8_t zTimerOverflow; // if > 0 then overflow happened (time elapsed > 53ms)
uint16_t zTimerRef;

double ZTimeToUs()
{
    double us;
    if(zTimerOverflow) {
        us = NAN;
    } else {
        us = double(zTimerCount - zTimerRef) / 0.8381;
    }
    return us;
}
