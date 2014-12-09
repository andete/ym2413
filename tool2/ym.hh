#ifndef YM_HH
#define YM_HH

#include <cstdint>

namespace ym {

void setup();

void reset();
void writeReg(uint8_t reg, uint8_t value);

} // namespace ym

#endif
