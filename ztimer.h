/*
 * Porting of The precision Zen timer by Michael Abrash
 *
 * See Michael Abrash's Graphics Programming Black Book, Chapter 3
 */

extern uint16_t zTimerCount;
extern uint8_t zTimerOverflow; // if > 0 then overflow happened (time elapsed > 53ms)
extern uint16_t zTimerRef;

double ZTimeToUs();

#define DELAY "db 0xeb,0,0xeb,0,0xeb,0" // 3 x jmp $+2

extern void ZTimerOn();
#pragma aux ZTimerOn = \
"sti"           \
"mov  al, 0x34" \
"out  0x43, al" \
DELAY           \
"mov  al, 0"    \
"out  0x40, al" \
DELAY           \
"out  0x40, al" \
DELAY           \
DELAY           \
DELAY           \
"cli"           \
"mov  al, 0x34" \
"out  0x43, al" \
DELAY           \
"mov  al, 0"    \
"out  0x40, al" \
DELAY           \
"out  0x40, al" \
modify [ax]


extern void ZTimerOff();
#pragma aux ZTimerOff = \
"xor  ax, ax"   \
"out  0x43, al" \
"mov  al, 0xA " \
"out  0x20, al" \
DELAY           \
"in   al, 0x20" \
"and  al, 1"    \
"mov  zTimerOverflow, al" \
\
"sti"           \
"in   al, 0x40" \
DELAY           \
"mov  ah, al"   \
"in   al, 0x40" \
"xchg ah, al"   \
"neg  ax"       \
"mov  zTimerCount, ax" \
\
"mov   bx, 0"   \
"mov   cx, 16"  \
"cli"           \
"RefLoop:"      \
\
"mov  al, 0x34" \
"out  0x43, al" \
DELAY           \
"mov  al, 0"    \
"out  0x40, al" \
DELAY           \
"out  0x40, al" \
\
"mov  al, 0"    \
"out  0x43, al" \
DELAY           \
"in   al, 0x40" \
DELAY           \
"mov  ah, al"   \
"in   al, 0x40" \
"xchg ah, al"   \
"neg  ax"       \
\
"add  bx, ax"   \
"loop RefLoop"  \
\
"sti"           \
"add  bx, 8"    \
"mov  cl, 4"    \
"shr  bx, cl"   \
"mov  zTimerRef, bx" \
modify [ax bx cx]
