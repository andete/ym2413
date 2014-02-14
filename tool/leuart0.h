namespace leuart0 {

  // extra serial interface for debugging

  // PD4: TX
  // PD5: RX

  static void setup() {
    // To avoid false start, configure output LEU0_TX as high on PD4
    GPIO->P[3].DOUT |= (1 << 4);
    // Pin PD4 is configured to Push-pull
    GPIO->P[3].MODEL = (GPIO->P[3].MODEL & ~_GPIO_P_MODEL_MODE4_MASK) | GPIO_P_MODEL_MODE4_PUSHPULL;
    // Pin PD5 is configured to Input enabled
    GPIO->P[3].MODEL = (GPIO->P[3].MODEL & ~_GPIO_P_MODEL_MODE5_MASK) | GPIO_P_MODEL_MODE5_INPUT;


    // Enable signals TX, RX
    LEUART0->ROUTE |= LEUART_ROUTE_TXPEN | LEUART_ROUTE_RXPEN;
  }

}
