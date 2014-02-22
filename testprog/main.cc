// Both headers offer the same functions:
// - The ones in board.hh so actual hardware stuff
// - The ones in virtual.hh do something equivalent on an emulated board
//   This allows for easier offline testing.
#ifdef VIRTUAL
#include "virtual.hh"
#else
#include "board.hh"
#endif


// High level helper routines (independent of real/virtual board)

// Toggle the YM2413 for some amount of cycles
void waitCycles(int n)
{
	for (int i = 0; i < n; ++i) {
		setClk(1);
		delay(1); // TODO should actually only wait ~280ns
		setClk(0);
		delay(1); // TODO
	}
}

void writeRegister(int reg, int value)
{
	static const int EXTRA = 10; // more time between writes than strictly required

	setData(reg);
	setAddr(0);
	setWE(0);
	waitCycles(1);
	setCS(0);
	waitCycles(1);
	setCS(1);
	setWE(1);

	waitCycles(12-1-1 + EXTRA);
	
	setData(value);
	setAddr(1);
	setWE(0);
	waitCycles(1);
	setCS(0);
	waitCycles(1);
	setCS(1);
	setWE(1);
	
	waitCycles(84-1-1 + EXTRA);
}



static const int BUF_SIZE = 0x7F00;
short buf[BUF_SIZE]; // must be smaller than 64kB?


// Main test driver routine
int main()
{
	setup();
	delay(100);

	// Test LEDs
	for (int i = 0; i < 10; ++i) {
		setLed(i, true);
		delay(500);
		setLed(i, false);
	}

	// Test sending data over USB
	printUsb("Hello, world\r\n");

	// Reset YM2413
	setLed(0, true);
	setIC(false);    // activate reset
	waitCycles(100); // keep for 100 cycles (should be at least 80)
	setIC(1);        // deactive reset
	delay(200);

	// write 3 registers to produce a test sound
	setLed(1, true);
	writeRegister(16, 171); // frequency (8 lower bits)
	writeRegister(48,  48); // select instrument (piano), volume (maximum)
	                        // alternatives: 16 -> violin, 32 -> guitar
	                        //               48 -> piano, 64->flute, ...
	writeRegister(32,  28); // write frequency (upper 4 bits), set key-on


	// Every ym2413 cycle sample both MO an RO
	// The expectation is that RO remains constant (=neutral) and that MO
	// only contains a non-neutral vale for 1-4 cycles every 72 cycles.
	setLed(2, true);
	for (int i = 0; i < BUF_SIZE; i += 2) {
		buf[i + 0] = sampleMO();
		buf[i + 1] = sampleRO();
		waitCycles(1);
	}

	// Send buffer over USB
	setLed(3, true);
	sendDataUsb(reinterpret_cast<uint8_t*>(buf), sizeof(buf));

	// Done
	setLed(4, true);
	end();
}
