/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil -*- */

namespace timer0 {

  static void setup() {

    // TODO
    // Pin PA0 is configured to Input enabled
  GPIO->P[0].MODEL = (GPIO->P[0].MODEL & ~_GPIO_P_MODEL_MODE0_MASK) | GPIO_P_MODEL_MODE0_INPUT;
    // Pin PA1 is configured to Input enabled
  GPIO->P[0].MODEL = (GPIO->P[0].MODEL & ~_GPIO_P_MODEL_MODE1_MASK) | GPIO_P_MODEL_MODE1_INPUT;

    // Enable signals CC0, CC1
    TIMER0->ROUTE |= TIMER_ROUTE_CC0PEN | TIMER_ROUTE_CC1PEN;

    // TODO

  }

}
