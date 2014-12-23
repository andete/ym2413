#ifndef EVENT_HH
#define EVENT_HH

namespace event {

enum Event {
	DMA_DONE,
	ADC_OVERFLOW,
	DMA_OVERFLOW,
	EVENT_OVERFLOW,
	USB_ERROR,
};

// Post a new event
void post(Event event);

// Same as above, but can be called from ISR context (slightly faster).
void postISR(Event event);

// The main event loop, should be called from main()
void loop();

} // namespace event

#endif
