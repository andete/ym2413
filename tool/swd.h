/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil -*- */

namespace swd {

  static void setup() {

    // enable SWO tracing output
    GPIO->ROUTE |= GPIO_ROUTE_SWOPEN;

  }

}
