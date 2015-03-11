#ifndef COMMON_HH
#define COMMON_HH

#include <cstdint>
#include <vector>

struct Patch {
	struct Modulator {
		int AM;   // R#0,bit7
		int PM;   // R#0,bit6
		int EG;   // R#0,bit5 
		int KR;   // R#0,bit4
		int ML;   // R#0,bit3-0
		int KL;   // R#2,bit7-6
		int TL;   // R#2,bit5-0
		int WF;   // R#3,bit3
		int FB;   // R#3,bit2-0
		int AR;   // R#4,bit7-4
		int DR;   // R#4,bit3-0
		int SL;   // R#6,bit7-4
		int RR;   // R#6,bit3-0
	} mod;
	struct Carrier {
		int AM;   // R#1,bit7
		int PM;   // R#1,bit6
		int EG;   // R#1,bit5 
		int KR;   // R#1,bit4
		int ML;   // R#1,bit3-0
		int KL;   // R#3,bit7-6
		int WF;   // R#3,bit4
		int AR;   // R#5,bit7-4
		int DR;   // R#5,bit3-0
		int SL;   // R#7,bit7-4
		int RR;   // R#7,bit3-0
		// no TL,FB
	} car;
	// R#3,bit5 not used (only 63 bits instead of 64)
};

Patch getSinePatch();
void printPatch(const Patch& p);

using Script = std::vector<uint16_t>;

void init(const char* dev);

void writeRegister(uint8_t reg, uint8_t val, Script& script);
void setInstrument(const Patch& patch, Script& script);
void addReset(Script& script);
void endScript(Script& script);
void executeScript(const Script& script);

void setPending(uint8_t w);
void resetPending(uint8_t w);
bool isPending(uint8_t w);
const std::vector<uint16_t>& getCapturedData();
void pollIO();

void stopCapture();
void startCapture();
void toggleLedA();
void toggleLedB();
void toggleLedC();
void echo(uint8_t e);

#endif
