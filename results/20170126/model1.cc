#include <algorithm>
#include <iostream>
#include <cmath>
#include <cstdint>

using namespace std;

using uint1  = uint8_t;
using uint3  = uint8_t;
using uint4  = uint8_t;
using uint6  = uint8_t;
using uint10 = uint16_t;
using uint12 = uint16_t;
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
	uint1 am; // bit 7 in R#0/1
	uint1 wf; // bit 4/5 in R#3

	int16_t calcOp(YM2413& ym2413, int extraLevel, int phase);
};

struct ModOperator : Operator
{
	uint6 tl; // bits 5-0 in R#2
	uint3 fb; // bits 2-0 in R#3
	int12 p0 = 0; // delayed values for feedback calculation
	int12 p1 = 0; //  -2047..+2047

	void calcMod(YM2413& ym2413, int phase);
};

struct CarOperator : Operator
{
	int16_t calcCar(YM2413& ym2413, Channel& ch, ModOperator& mod, int phase);
};

struct Channel
{
	ModOperator mod;
	CarOperator car;
	uint4 vol; // bits 3-0 in R#30-38

	int16_t calcChan(YM2413& ym2413, int phase);
};

struct YM2413
{
	Channel ch[1]; // should be 9
	uint4 amLevel; // 0..13
};


int16_t Operator::calcOp(YM2413& ym2413, int extraLevel, int phase)
{
	if ((env & 0x7C) == 0x7C) {
		// enevlope level reached stop-condition, return +0
		return 0;
	}
	auto s = lookupSin(phase, wf);
	auto amValue = am ? ym2413.amLevel : 0;
	auto att = min(127, extraLevel + ksl + env + amValue);
	return lookupExp(s + 16 * att);
}

void ModOperator::calcMod(YM2413& ym2413, int phase)
{
	auto f = fb ? ((p0 + p1) >> (8 - fb)) : 0;
	auto m = calcOp(ym2413, 2 * tl, phase + f) >> 1; // -2047..+2047
	p1 = p0; p0 = m;
}

int16_t CarOperator::calcCar(YM2413& ym2413, Channel& ch, ModOperator& mod, int phase)
{
	return calcOp(ym2413, 8 * ch.vol, phase + 2 * mod.p0) >> 4;
}

int16_t Channel::calcChan(YM2413& ym2413, int phase)
{
	mod.calcMod(ym2413, phase - 1);
	return car.calcCar(ym2413, *this, mod, phase);
}


int main()
{
	initTables();

	YM2413 y;
	y.amLevel = 0;

	y.ch[0].vol = 0;

	y.ch[0].mod.tl = 0;
	y.ch[0].mod.fb = 0;
	y.ch[0].mod.ksl = 0;
	y.ch[0].mod.env = 127;
	y.ch[0].mod.am = 0;
	y.ch[0].mod.wf = 0;

	y.ch[0].car.ksl = 0;
	y.ch[0].car.env = 120;
	y.ch[0].car.am = 0;
	y.ch[0].car.wf = 1;

	for (int phase = 0; phase < 1024; ++phase) {
		auto out = y.ch[0].calcChan(y, phase);
		cout << 255 - out << endl;
	}
}
