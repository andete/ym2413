/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil -*- */

#define YM_D0  0
#define YM_D1  1
#define YM_D2  2
#define YM_D3  3
#define YM_D4  4
#define YM_D5  5
#define YM_D6  6
#define YM_D7  7
#define YM_A0  8
#define YM_WE  9
#define YM_CS 10
#define YM_IC 11
#define YM_XIN 12
#define YM_XOUT 13
#define YM_COUNT 14

namespace ym {

  static const struct {
    GPIO_Port_TypeDef port;
    unsigned int pin;
    bool negative_logic;
  } pins[YM_COUNT] = {
    { gpioPortC,  0, false }, // D0
    { gpioPortC,  1, false }, // D1
    { gpioPortC,  2, false }, // D2
    { gpioPortC,  3, false }, // D3
    { gpioPortC,  4, false }, // D4
    { gpioPortC,  5, false }, // D5
    { gpioPortC,  6, false }, // D6
    { gpioPortC,  7, false }, // D7
    { gpioPortC,  8, false }, // A0
    { gpioPortC,  9, true  }, // ~WE
    { gpioPortC, 10, true  }, // ~CS
    { gpioPortC, 11, true  }, // ~IC
    { gpioPortA,  0, false }, // XIN
    { gpioPortA,  1, false }, // XOUT
  };

  static void setup() {
    for (int i = 0; i < YM_COUNT; ++i) {
      GPIO_PinModeSet(pins[i].port, pins[i].pin, gpioModePushPull, 
                      pins[i].negative_logic?1:0);
    }
  }

  // todo clever access methods

  static inline void high(const uint8_t n) {
    GPIO_PinOutSet(pins[n].port, pins[n].pin);
  }

  static inline void low(const uint8_t n) {
    GPIO_PinOutClear(pins[n].port, pins[n].pin);
  }

  static inline void start_clock(const uint32_t freq=3579545) {
    const uint32_t topValue = CMU_ClockFreqGet(cmuClock_HFPER)/freq;
    TIMER_TopSet(TIMER0, topValue);
    TIMER_CompareSet(TIMER0, 0, topValue/2);
    TIMER_Enable(TIMER0, true);
  }

  static inline void stop_clock() {
    TIMER_Enable(TIMER0, false);
  }

}
