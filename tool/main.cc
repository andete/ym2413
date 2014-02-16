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
#include "em_usb.h"
#include "em_int.h"
#include "em_timer.h"
#include "efm32gg_uart.h"

// own code
#include "dma.hh"
#include "tick.hh"
#include "clock.hh"
#include "adc0.hh"
#include "leuart0.hh"
#include "timer0.hh"
#include "swd.hh"
#include "usart0.hh"
#include "led.hh"
#include "usbcdc.hh"
#include "ym.hh"

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
  usbcdc::setup();

}

int main(int argc, char ** argv) {
  (void)argc;
  (void)argv;

  setup();

  tick::delay(100);
  usbcdc::print("Hello, world  \r\n");

  while (true) {
  }
  return 0;
}
