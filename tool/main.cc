/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil -*- */

// efm32 specific headers
#include "em_device.h"
#include "em_chip.h"
#include "em_adc.h"
#include "em_cmu.h"
#include "em_usart.h"

// own code
#include "tick.h"
#include "clock.h"
#include "adc0.h"
#include "leuart0.h"
#include "timer0.h"
#include "swd.h"

static void setup() {
  // Chip errata
  CHIP_Init();

  tick::setup();
  clock::setup();
  adc0::setup();
  leuart0::setup();
  timer0::setup();
  swd::setup();
}

int main(int argc, char ** argv) {
  (void)argc;
  (void)argv;

  setup();

  while (true) {
  }

}
