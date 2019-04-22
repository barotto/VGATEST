/*
 * Porting of The precision Zen timer by Michael Abrash
 */

#include "stdint.h"
#include "ztimer.h"

uint32_t ztimercount; // if > 2^16-1 then overflow
uint16_t ztimerref;

