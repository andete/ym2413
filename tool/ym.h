/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil -*- */

namespace ym {

  static const struct {
    GPIO_Port_TypeDef port;
    unsigned int pin;
    bool negative_logic;
  } pins[12] = {
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
  };

  static void setup() {
    for (int i = 0; i < 12; ++i) {
      GPIO_PinModeSet(pins[i].port, pins[i].pin, gpioModePushPull, pins[i].negative_logic?1:0);
    }
  }

  // todo clever access methods

}
