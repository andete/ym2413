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
    { gpioPortE,  8 }, // LED 5
    { gpioPortE,  9 }, // LED 6
    { gpioPortE, 10 }, // LED 7
    { gpioPortE, 12 }, // LED 8
    { gpioPortE, 13 }, // LED 9
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

  static inline void toggle(const uint8_t led) {
    GPIO_PinOutToggle(pins[led].port, pins[led].pin);
  }

  static void demo() {
    for (int j = 1; j < 100; ++j) {
      tick::delay(50+j/2);
      led::off((j-1) % 10);
      led::on(j % 10);
    }
    for (int j = 100; j > 0; --j) {
      tick::delay(50+j/2);
      led::off((j+1) % 10);
      led::on(j % 10);
    }
  }

}
