#include "tables.hh"
#include <algorithm>
#include <iostream>
#include <cmath>
#include <cstdint>

using namespace std;

using uint1  = uint8_t;
using uint3  = uint8_t;
using uint4  = uint8_t;
using uint6  = uint8_t;
using uint8  = uint8_t;
using uint9  = uint16_t;
using uint10 = uint16_t;
using uint12 = uint16_t;
using uint19 = uint32_t;
using int12  = int16_t;

uint10 expTable   [256]; // values between 0..1018  (last bit unused on YM2413?)
uint12 logsinTable[256]; // values between 0..2137

void initTables()
{
	for (int i = 0; i < 256; ++i) {
		logsinTable[i] = round(-log2(sin((double(i) + 0.5) * M_PI / 256.0 / 2.0)) * 256.0);
		expTable[i] = round((exp2(double(i) / 256.0) - 1.0) * 1024.0);
	}
}

// input:  'val' 0..1023  (10 bit)
// output: 1+12 bits (sign-magnitude representation)
uint16_t lookupSin(uint10 val, uint1 wf)
{
	bool sign   = val & 512;
	bool mirror = val & 256;
	val &= 255;
	uint16_t result = logsinTable[mirror ? val ^ 0xFF : val];
	if (sign) {
		if (wf) result = 0xFFF; // zero (absolute value)
		result |= 0x8000; // negate
	}
	return result;
}

// input: sign / exponent / magnitude
// output: 1-complements linear value in range -4095..+4095
int16_t lookupExp(uint16_t val)
{
	bool sign = val & 0x8000;
	int t = (expTable[(val & 0xFF) ^ 0xFF] << 1) | 0x0800;
	int result = t >> ((val & 0x7F00) >> 8);
	if (sign) result = ~result;
	return result;
}

struct YM2413;
struct Channel;
struct ModOperator;

struct Operator
{
	int ksl; // 0 .. 112   controlled by KSL in R#2/3 and OCT/FNUM
	int env; // 0 .. 127
	uint1 am; // bit  7   in R#0/1
	uint1 pm; // bit  6   in R#0/1
	uint4 ml; // bits 3-0 in R#0/1
	uint1 wf; // bit 4/5 in R#3
	uint19 phase = 0; // 10.9 fixed point

	int calcPhase(YM2413& ym2413, Channel& ch);
	int16_t calcOp(YM2413& ym2413, Channel& ch, int extraPhase, int extraLevel);
};

struct ModOperator : Operator
{
	uint6 tl; // bits 5-0 in R#2
	uint3 fb; // bits 2-0 in R#3
	int12 p0 = 0; // delayed values for feedback calculation
	int12 p1 = 0; //  -2047..+2047

	void calcMod(YM2413& ym2413, Channel& ch);
};

struct CarOperator : Operator
{
	int16_t calcCar(YM2413& ym2413, Channel& ch, ModOperator& mod);
};

struct Channel
{
	ModOperator mod;
	CarOperator car;
	uint9 fnum;  // bits 7-0 in R#10-18  +  bit 0 in R#20-28
	uint3 block; // bits 3-1 in R#20-28
	uint4 vol; // bits 3-0 in R#30-38

	int16_t calcChan(YM2413& ym2413);
};

struct YM2413
{
	static const int NUM_CHANNELS = 1; // should be 9
	Channel ch[NUM_CHANNELS];
	uint32_t counter = 0;
	uint8 amCounter = 0; // 0..209 for SW implementation, probably doesn't exist in real HW

	void calc();
};


int Operator::calcPhase(YM2413& ym2413, Channel& ch)
{
	int pmValue = pm ? pmTable[ch.fnum >> 6][(ym2413.counter >> 10) & 7] : 0; // -7..+7
	auto step = (((2 * ch.fnum + pmValue) * mlTab[ml]) << ch.block) >> 2;
	phase += step; // TODO already use incremented value for this sample?
	return phase >> 9; // drop 9 fractional bits
}

int16_t Operator::calcOp(YM2413& ym2413, Channel& ch, int extraPhase, int extraLevel)
{
	auto p = calcPhase(ym2413, ch) + extraPhase;

	if ((env & 0x7C) == 0x7C) {
		// enevlope level reached stop-condition, return +0
		return 0;
	}
	auto s = lookupSin(p, wf);
	auto amValue = am ? (amTable[ym2413.amCounter] >> 1) : 0; // 0..13
	auto att = min(127, extraLevel + ksl + env + amValue);
	return lookupExp(s + 16 * att);
}

void ModOperator::calcMod(YM2413& ym2413, Channel& ch)
{
	auto f = fb ? ((p0 + p1) >> (8 - fb)) : 0;
	f -= 1; // TODO probably a side effect of how the phase counters in
	        //      mod/car are reset. This isn't implemented yet
	auto m = calcOp(ym2413, ch, f, 2 * tl) >> 1; // -2047..+2047
	p1 = p0; p0 = m;
}

int16_t CarOperator::calcCar(YM2413& ym2413, Channel& ch, ModOperator& mod)
{
	return calcOp(ym2413, ch, 2 * mod.p0, 8 * ch.vol) >> 4;
}

int16_t Channel::calcChan(YM2413& ym2413)
{
	mod.calcMod(ym2413, *this);
	return car.calcCar(ym2413, *this, mod);
}

void YM2413::calc()
{
	++counter;
	
	// LFO AM
	if ((counter & 63) == 0) {
		// TODO use amTable[] before or after incrementing counter
		//      for this sample?
		// TODO same question for PM
		++amCounter;
		if (amCounter == sizeof(amTable)) amCounter = 0;
	}

	for (int c = 0; c < NUM_CHANNELS; ++c) {
		auto out = ch[c].calcChan(*this);
		cout << 255 - out << endl;
	}
}

int main()
{
	initTables();

	YM2413 y;

	y.ch[0].vol = 0;
	y.ch[0].fnum = 256;
	y.ch[0].block = 0;

	y.ch[0].mod.tl = 63;
	y.ch[0].mod.fb = 0;
	y.ch[0].mod.ksl = 0;
	y.ch[0].mod.env = 127;
	y.ch[0].mod.ml = 2;
	y.ch[0].mod.am = 0;
	y.ch[0].mod.pm = 0;
	y.ch[0].mod.wf = 0;

	y.ch[0].car.ksl = 0;
	y.ch[0].car.env = 0;
	y.ch[0].car.ml = 2;
	y.ch[0].car.am = 0;
	y.ch[0].car.pm = 0;
	y.ch[0].car.wf = 0;

	for (int i = 0; i < 1024; ++i) {
		y.calc();
	}
}
