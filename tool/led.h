/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil -*- */

namespace led {

  static const struct {
    GPIO_Port_TypeDef port;
    unsigned int pin;
  } pins[10] = {
    { gpioPortA,  2 }, // LED 0
    { gpioPortA,  3 }, // LED 1
    { gpioPortA,  4 }, // LED 2
    { gpioPortA,  5 }, // LED 3
    { gpioPortA,  8 }, // LED 4
    { gpioPortA,  9 }, // LED 5
    { gpioPortA, 10 }, // LED 6
    { gpioPortB,  7 }, // LED 7
    { gpioPortB,  8 }, // LED 8
    { gpioPortB, 11 }, // LED 9
  };

  static void setup() {
    for (int i = 0; i < 10; ++i) {
      GPIO_PinModeSet(pins[i].port, pins[i].pin, gpioModePushPull, 0);
    }
    // maybe do a funky blink pattern on startup
  }

  static inline void on(const uint8_t led) {
    GPIO_PinOutSet(pins[led].port, pins[led].pin);
  }

  static inline void off(const uint8_t led) {
    GPIO_PinOutClear(pins[led].port, pins[led].pin);
  }

}
