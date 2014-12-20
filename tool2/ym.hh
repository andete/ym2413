#ifndef YM_HH
#define YM_HH

#include <cstdint>

namespace ym {

void setup();

void reset();
void writeReg(uint8_t reg, uint8_t value);

// Setup custom instrument with sine wave,
// play this instrument on channel-1
// @param volume: 0..15 (0 = loudest)
// @param freq: 0..4095
void playSine(int volume, int freq);

void syncTimer1();

} // namespace ym

#endif
