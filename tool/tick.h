/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil -*- */

namespace tick {

  static volatile uint32_t ms = 0;

  static void setup() {
    /* Setup SysTick Timer for 1 msec interrupts  */
    if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) while (1) ;
  }

  static void delay(const uint32_t delay_ms) {
    const uint32_t cur_ms = ms;
    while ((ms - cur_ms) < delay_ms) ;
  }

}
 

// systick interrupt handler
void SysTick_Handler(void) {
  tick::ms++;       /* increment counter necessary in Delay()*/
}
