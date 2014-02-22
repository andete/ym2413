#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstdio>
#include <cassert>

using namespace std;

// Keep track of the virtual hardware status
static int ledStatus;
static int ticks; // number of YM2413 clock ticks
static int lastIcActivate; // moment in time (ym2413 ticks) when reset was activated
static int lastWrite;      // moment in time (ym2413 ticks) when a write was executed
static int nextAllowedWrite; // earliest moment in time (ym2413 ticks) when a new
                             // write can be accepted
static bool sIc;   // YM2413 reset pin
static bool sCs;   // YM2413 CS (chip select) pin
static bool sWe;   // YM2413 WE (write enable) pin
static bool sClk;  // YM2413 clock level
static int  sData; // YM2413 data pins D7-D0
static int  sAddr; // YM2413 address pin A0
static int  latch; // YM2413 register number
static int  regs[0x40]; // YM2413 internal registers


#define PRT_ERROR(mes) \
	do { \
		cout << setw(4) << dec << ticks << "  ERROR: " << mes << endl; \
	} while (0)

#define PRT_INFO(mes) \
	do { \
		cout << setw(4) << dec << ticks << "  INFO : " << mes << endl; \
	} while (0)


static void setup()
{
	ledStatus = 0; // all LEDs off
	ticks = 0;
	lastIcActivate   = -999;
	lastWrite        = -999;
	nextAllowedWrite = -999;
	sIc = true;
	sCs = true;
	sData = 0;
	sAddr = 0;
}

static void delay(int millisec)
{
}

static void setLed(int num, bool status)
{
	assert(num < 10);
	if (status) {
		ledStatus |=   1 << num;
	} else {
		ledStatus &= ~(1 << num);
	}

	printf("LEDS: ");
	for (int i = 0; i < 10; ++i) {
		putchar(ledStatus & (1 << i) ? 'O' : '.');
	}
	putchar('\n');
}

static void printUsb(const char* message)
{
	printf("USB: %s", message);
}

static void end()
{
	exit(0);
}



// write /IC
// 0 -> active     (should remain active for at least 80 cycles)
// 1 -> not active
void setIC(bool ic)
{
	sIc = ic;
	if (sIc) {
		// deactivate
		int delta = ticks - lastIcActivate;
		if (delta < 80) PRT_ERROR("/IC only remained active for " << dec << delta << " ticks");
		PRT_INFO("deactivate reset");
	} else {
		// activate
		lastIcActivate = ticks;
		PRT_INFO("activate reset");
	}
}

// write /CS (false->active, true->not active)
void setCS(bool cs)
{
	if (sCs == cs) return;
	sCs = cs;
	if (sWe && ticks) { // ignore 'error' at startup
		PRT_ERROR("write to /CS, but /WE wasn't active");
	}
	if (!sCs && !sWe) {
		if (ticks < nextAllowedWrite) {
			int delta = ticks - lastWrite;
			PRT_ERROR("only " << dec << delta << " cycles since last write");
		}
		lastWrite = ticks;
		if (sAddr == 0) {
			nextAllowedWrite = max(nextAllowedWrite, ticks + 12);
			if (sData & 0xC0) PRT_ERROR("set register addr > 0x40");
			latch = sData & 0x3F;
			PRT_INFO("set latch = 0x" << hex << latch);
		} else {
			assert(sAddr == 1);
			assert(latch < 0x40);
			nextAllowedWrite = max(nextAllowedWrite, ticks + 84);
			regs[latch] = sData;
			PRT_INFO("reg[0x" << hex << latch << "] = 0x" << sData);
		}
	}
}

void setWE(bool we)
{
	sWe = we;
	if (!sCs && ticks) { // ignore 'error' at startup
		PRT_ERROR("write to /WE with /CS active");
	}
}

// true  -> positive edge
// false -> negative edge
void setClk(bool clk)
{
	if ((sClk != clk) && clk) {
		// positive clock edge
		++ticks;
	}
	sClk = clk;
}

// write D7-D0
void setData(int data)
{
	sData = data;
}

// write A0
void setAddr(int addr)
{
	sAddr = addr;
	if (!sCs && ticks) { // ignore 'error' at startup
		PRT_ERROR("write to A0 with /CS active");
	}
}

short sampleMO()
{
	return 0; // dummy
}

short sampleRO()
{
	return 0; // dummy
}

void sendDataUsb(const uint8_t* buf, uint16_t len)
{
	// dummy, do nothing
}
