/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil -*- */

// efm32 specific headers
#include "em_device.h"
#include "em_chip.h"
#include "em_adc.h"
#include "em_cmu.h"
#include "em_usart.h"

// own code
#include "tick.h"

static void setup() {
  // Chip errata
  CHIP_Init();

  tick::setup();
}

int main(int argc, char ** argv) {
  (void)argc;
  (void)argv;

  setup();

  while (true) {
  }

}
