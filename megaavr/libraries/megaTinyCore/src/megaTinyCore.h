#include <Arduino.h>
#ifndef __MEGATINYCORE_H
#define __MEGATINYCORE_H
// in ADCErrors.cpp
// ADC error interpretation helper functions
#ifdef __cplusplus
  int8_t analogCheckError(int16_t val);
  int8_t analogCheckError(int32_t val);
  bool printADCRuntimeError(int32_t error, HardwareSerial &__dbgser = Serial);
  bool printADCRuntimeError(int16_t error, HardwareSerial &__dbgser = Serial);
#endif
// Reset immdiately using software reset. The bootloader, if present will run.

extern const uint8_t bit_mask_to_position[];
extern const uint8_t bit_mask_to_position_n[];

inline uint8_t bitMaskToPosition_by_Math(uint8_t bitmask) {
  if (__builtin_constant_p(bitmask)) {
    if (bitmask > 128) {
      badArg("bitMaskToPosition is being called a constant which is not a bitmask");
      return 255;
    }
  }
  __asm__ __volatile__ (
    "eor r0, r0"      "\n\t"
    "dec r0"          "\n\t" // set temp register to 255
    "lsr %0"          "\n\t" // Loop comes back to here where we shift bitmask right
    "inc r0"          "\n\t" // increment the counter so if 1 is first shift we get 0
    "brcc .-6"        "\n\t" // if it's a 0 rightshift repeat
    "cpi %0, 0"       "\n\t" // Now we've gotten a 1, is what's left 0?
    "breq .+4"        "\n\t" // if yes skip next 2 isn
    "ldi %0, 255"     "\n\t" // if no load 255 into %0
    "rjmp .+2"        "\n\t" // and skip the last isn.
    "mov %0, r0"      "\n\t" // otherwise copy r0 into %0
    : "+d" ((uint8_t) bitmask));
  return bitmask;
}
inline uint8_t bitMaskToPosition_by_LUT(uint8_t bitmask) {
  if (__builtin_constant_p(bitmask)) {
    if (bitmask > 128) {
      badArg("bitMaskToPosition is being called a constant which is not a bitmask");
      return 255;
    }
  } else { //here we need to do a bounds check
    if (bitmask > 128) {
      return 255; // Error - called with something not a bit mask!
    }
  }
  return bit_mask_to_position[bitmask];
}
inline uint8_t bitMaskToHalfPosition(uint8_t bitmask, bool highnybble) {
  if (highnybble) {
    _SWAP(bitmask);
  }
  bitmask &= 0x0F;
  // No further checks table has 15 entries.
  return bit_mask_to_position_n[bitmask];
}


inline void ResetViaWDT() {
  _PROTECTED_WRITE(WDT.CTRLA, WDT_PERIOD_8CLK_gc); //enable the WDT, minimum timeout
  while (1); // spin until reset
}

inline void ResetViaSoftware() {
  _PROTECTED_WRITE(RSTCTRL.SWRR, 1);
}

uint8_t getTunedOSCCAL(uint8_t osc, uint8_t target);

void printTuningStatus();

#endif
