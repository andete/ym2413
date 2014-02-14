/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil -*- */

// efm32 specific headers
#include "em_device.h"
#include "em_chip.h"
#include "em_adc.h"
#include "em_cmu.h"
#include "em_usart.h"
#include "em_leuart.h"
#include "em_dma.h"
#include "em_gpio.h"

// own code
#include "dma.h"
#include "tick.h"
#include "clock.h"
#include "adc0.h"
#include "leuart0.h"
#include "timer0.h"
#include "swd.h"
#include "usart0.h"
#include "led.h"
#include "ym.h"
#include "usb.h"

static void setup() {
  // Chip errata
  CHIP_Init();

  // setup all subsystems
  tick::setup();
  clock::setup();
  dma::setup();
  adc0::setup();
  leuart0::setup();
  timer0::setup();
  swd::setup();
  usart0::setup();
  led::setup();
  ym::setup();
  usb::setup();
}

int main(int argc, char ** argv) {
  (void)argc;
  (void)argv;

  setup();

  while (true) {
  }

}
