/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil -*- */

namespace timer0 {

  static void setup() {

    // Enable signals CC0, CC1
    TIMER0->ROUTE |= TIMER_ROUTE_CC0PEN | TIMER_ROUTE_CC1PEN;

  }

}
