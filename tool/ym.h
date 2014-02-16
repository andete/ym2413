/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil -*- */

#define YM_D0  0
#define YM_D1  1
#define YM_D2  2
#define YM_D3  3
#define YM_D4  4
#define YM_D5  5
#define YM_D6  6
#define YM_D7  7
#define YM_A0  8
#define YM_WE  9
#define YM_CS 10
#define YM_IC 11
#define YM_XIN 12
#define YM_XOUT 13
#define YM_COUNT 14

namespace ym {

  static const struct {
    GPIO_Port_TypeDef port;
    unsigned int pin;
    bool negative_logic;
  } pins[YM_COUNT] = {
    { gpioPortC,  0, false }, // D0
    { gpioPortC,  1, false }, // D1
    { gpioPortC,  2, false }, // D2
    { gpioPortC,  3, false }, // D3
    { gpioPortC,  4, false }, // D4
    { gpioPortC,  5, false }, // D5
    { gpioPortC,  6, false }, // D6
    { gpioPortC,  7, false }, // D7
    { gpioPortC,  8, false }, // A0
    { gpioPortC,  9, true  }, // ~WE
    { gpioPortC, 10, true  }, // ~CS
    { gpioPortC, 11, true  }, // ~IC
    { gpioPortA,  0, false }, // XIN
    { gpioPortA,  1, false }, // XOUT
  };

  static void setup_timer0() {

    // Select CC channel parameters
    TIMER_InitCC_TypeDef timerCCInit = {
      .eventCtrl  = timerEventEveryEdge,
      .edge       = timerEdgeNone, // we don't care about input
      .prsSel     = timerPRSSELCh0, // not used
      .cufoa      = timerOutputActionNone,
      .cofoa      = timerOutputActionNone,
      .cmoa       = timerOutputActionNone,
      .mode       = timerCCModePWM,
      .filter     = false, // no filter
      .prsInput   = false, // no TIMERnCCx input
      .coist      = false, // don't clear output when counter disabled
      .outInvert  = false, // don't invert output
    };

    // Configure CC channel 0
    TIMER_InitCC(TIMER0, 0, &timerCCInit);

    // Route CC0 to location 0 (PA0) and enable pin
    TIMER0->ROUTE |= (TIMER_ROUTE_CC0PEN | TIMER_ROUTE_LOCATION_LOC0); 

    // TODO: actual timer setup to create our signal
    // Set Top Value
    //const uint32_t PWM_FREQ = 3*1000*1000; // 3 Mhz
    //const uint32_t topValue = CMU_ClockFreqGet(cmuClock_HFPER)/PWM_FREQ;
    //TIMER_TopSet(TIMER0, topValue);

    // 50% duty cycle
    // TIMER_CompareSet(TIMER0, 0, topValue/2);
 
    // TODO
    // Select timer parameters
    TIMER_Init_TypeDef timerInit = {
      .enable     = false,
      .debugRun   = false, // Keep counter during debug halt
      .prescale   = timerPrescale1, // no pre-scaling
      .clkSel     = timerClkSelHFPerClk, // Select HFPER clock
      .count2x    = false, // double count mode
      .ati        = false, // 
      .fallAction = timerInputActionNone, // no action
      .riseAction = timerInputActionNone, // no action
      .mode       = timerModeUp, // count up
      .dmaClrAct  = false, // no dma involved
      .quadModeX4 = false, // no quadrature mode
      .oneShot    = false, // keep running
      .sync       = false, // 
    };
  
    // TODO; we may want to have interrupts later to do 
    // some handling
    // Enable overflow interrupt
    // TIMER_IntEnable(TIMER0, TIMER_IF_OF);
  
    // Enable TIMER0 interrupt vector in NVIC
    // NVIC_EnableIRQ(TIMER0_IRQn);
  
    // Configure timer
    TIMER_Init(TIMER0, &timerInit);
  }

  static void setup() {
    for (int i = 0; i < YM_COUNT; ++i) {
      GPIO_PinModeSet(pins[i].port, pins[i].pin, gpioModePushPull, 
                      pins[i].negative_logic?1:0);
    }

    setup_timer0();
  }

  // todo clever access methods

  static inline void high(const uint8_t n) {
    GPIO_PinOutSet(pins[n].port, pins[n].pin);
  }

  static inline void low(const uint8_t n) {
    GPIO_PinOutClear(pins[n].port, pins[n].pin);
  }

  static inline void start_clock(const uint32_t freq=3579545) {
    const uint32_t topValue = CMU_ClockFreqGet(cmuClock_HFPER)/freq;
    TIMER_TopSet(TIMER0, topValue);
    TIMER_CompareSet(TIMER0, 0, topValue/2);
    TIMER_Enable(TIMER0, true);
  }

  static inline void stop_clock() {
    TIMER_Enable(TIMER0, false);
  }

}
