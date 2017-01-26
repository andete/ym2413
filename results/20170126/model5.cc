#include <algorithm>
#include <iostream>
#include <cassert>
#include <cmath>
#include <cstdint>

using namespace std;

using uint1  = uint8_t;
using uint2  = uint8_t;
using uint3  = uint8_t;
using uint4  = uint8_t;
using uint6  = uint8_t;
using uint7  = uint8_t;
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
	uint1 am;  // bit  7   in R#0/1
	uint1 pm;  // bit  6   in R#0/1
	uint1 eg;  // bit  5   in R#0/1
	uint1 ksr; // bit  4   in R#0/1
	uint4 ml;  // bits 3-0 in R#0/1

	uint2 ksl; // bits 7-6 in R#2/3
	uint1 wf;  // bit  3/4 in R#3

	uint4 ar;  // bits 7-4 in R#4/5
	uint4 dr;  // bits 3-0 in R#4/5
	uint4 sl;  // bits 7-4 in R#6/7
	uint4 rr;  // bits 3-0 in R#6/7

	enum EgState : uint2 { DAMP, ATTACK, DECAY, SUSTAIN } egState = DAMP;
	uint7 env = 0; // 0..127
	uint19 phase = 0; // 10.9 fixed point

	int calcPhase(YM2413& ym2413, Channel& ch);
	void calcEnv(YM2413& YM2413, Channel& ch, bool isCarrier);
	int16_t calcOp(YM2413& ym2413, Channel& ch, bool isCarrier, int extraPhase, int extraLevel);
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
	uint9 fnum;   // bits 7-0 in R#10-18  +  bit 0 in R#20-28
	uint1 sustain;// bit  5   in R#20-28
	uint1 keyon;  // bit  4   in R#20-28
	uint3 block;  // bits 3-1 in R#20-28
	uint4 instr;  // bits 7-4 in R#30-28   TODO not yet used
	uint4 vol;    // bits 3-0 in R#30-38

	int16_t calcChan(YM2413& ym2413);
};

struct YM2413
{
	static const int NUM_CHANNELS = 1; // should be 9
	Channel ch[NUM_CHANNELS];
	uint32_t counter = 0;
	uint8 amCounter = 0; // 0..105, lower 3 bits are dropped on use (so 0..13 on use)
	uint1 amDirection = 0;

	void calc();
};


int Operator::calcPhase(YM2413& ym2413, Channel& ch)
{
	// In real HW very likely not stored in a table, but calculated along these lines:
	//  pmTable[x][0]    = 0
	//  pmTable[x][2]    = x
	//  pmTable[x][1,3]  =  pmTable[x][2] >> 1
	//  pmTable[x][4..7] = -pmTable[x][0..3]
	static const int8_t pmTable[8][8] = {
		{ 0, 0, 0, 0, 0, 0, 0, 0 }, // FNUM = 000xxxxxx
		{ 0, 0, 1, 0, 0, 0,-1, 0 }, // FNUM = 001xxxxxx
		{ 0, 1, 2, 1, 0,-1,-2,-1 }, // FNUM = 010xxxxxx
		{ 0, 1, 3, 1, 0,-1,-3,-1 }, // FNUM = 011xxxxxx
		{ 0, 2, 4, 2, 0,-2,-4,-2 }, // FNUM = 100xxxxxx
		{ 0, 2, 5, 2, 0,-2,-5,-2 }, // FNUM = 101xxxxxx
		{ 0, 3, 6, 3, 0,-3,-6,-3 }, // FNUM = 110xxxxxx
		{ 0, 3, 7, 3, 0,-3,-7,-3 }, // FNUM = 111xxxxxx
	};

	static const uint8_t mlTab[16] = {
		1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 20, 24, 24, 30, 30
	};

	int pmValue = pm ? pmTable[ch.fnum >> 6][(ym2413.counter >> 10) & 7] : 0; // -7..+7
	auto step = (((2 * ch.fnum + pmValue) * mlTab[ml]) << ch.block) >> 2;
	phase += step; // TODO already use incremented value for this sample?
	return phase >> 9; // drop 9 fractional bits
}

void Operator::calcEnv(YM2413& ym2413, Channel& ch, bool isCarrier)
{
	// Maximum envelope level reached at env=124 (-45dB) or higher
	bool maxEnv = (env >> 2) == 0x1F;

	// note: no need to check keyon (egState doesn't matter when keyon==0)
	if ((egState == DAMP) && maxEnv && isCarrier) {
		// switch DAMP->ATTACK (or skip attack for rate=15)
		EgState next = (ar == 15) ? DECAY : ATTACK;
		egState = next;
		phase = 0;
		// also change corresponding modulator operator
		ch.mod.egState = next;
		ch.mod.phase = 0;
	} else if ((egState == ATTACK) && (env == 0)) {
		egState = DECAY;
	} else if ((egState == DECAY) && ((env >> 3) == sl)) {
		egState = SUSTAIN;
	}

	// Attack or decay, and at what rate?
	bool attack;
	uint4 basicRate;
	if (!ch.keyon && isCarrier) {
		// release state
		// note: modulator does NOT have a release state!
		attack = false;
		basicRate = eg ? rr
		               : ch.sustain ? 5 : 7;
	} else {
		switch (egState) {
		case DAMP:
			attack = false;
			basicRate = 12;
			break;
		case ATTACK:
			attack = true;
			basicRate = ar;
			break;
		case DECAY:
			attack = false;
			basicRate = dr;
			break;
		case SUSTAIN:
			attack = false;
			basicRate = eg ? 0 : rr;
			break;
		default:
			assert(false);
			attack = false;
			basicRate = 0;
		}
	}

	// Calculate effective rate
	uint4 bf = (ch.block << 1) | (ch.fnum >> 8); // block(2:1):fnum(8)
	uint4 rks = ksr ? bf : (bf >> 2);
	uint6 rate = max(63, 4 * basicRate + rks);
	uint2 row = rate & 3;

	// Table used for envelope-advance calculations. In real hardware there
	// likely is no such table. Instead there could be logic along these lines:
	//   egTable[x][1,3,5,7] = 1
	//   egTable[x][0      ] = 0
	//   egTable[x][2,6    ] = x & 2
	//   egTable[x][4      ] = x & 1
	static const uint8_t egTable[4][8] = {
		{ 0, 1, 0, 1, 0, 1, 0, 1 }, //  4 out of 8
		{ 0, 1, 0, 1, 1, 1, 0, 1 }, //  5 out of 8
		{ 0, 1, 1, 1, 0, 1, 1, 1 }, //  6 out of 8
		{ 0, 1, 1, 1, 1, 1, 1, 1 }, //  7 out of 8
	};

	uint3 column;
	if (attack) {
		switch (rate / 4) {
		case 15: // verified on real YM2413: rate=15 behaves the same as rate=0
			 // rate=15 is normally handled by skipping attack completely,
			 // so this case can only trigger when rate was changed during attack.
		case 0: // rates 0..3, envelope doesn't change
			break;
		default: { // rates 4..47
			// perform a 'env' step this iteration?
			uint4 shift = 13 - (rate / 4);
			int mask = ((1 << shift) - 1) & ~3; // ignore lower 2 bits!
			if ((ym2413.counter & mask) != 0) break;
			column = (ym2413.counter >> shift) & 7;
			if (egTable[row][column]) {
				env = env - (env >> 4) - 1;
			}
			break;
		}
		case 12: // rates 48..51
		case 13: // rates 52..55
		case 14: { // rates 56..59
			column = (ym2413.counter & 0xc) >> 1;
			int m = 16 - (rate / 4);
			m -= egTable[row][column];
			env = env - (env >> m) - 1;
			break;
		}
		}
	} else {
		switch (rate / 4) {
		case 0: // rates 0..3, envelope doesn't change
			break;
		default: { // rates 4..51
			// perform a 'env' step this iteration?
			uint4 shift = 13 - (rate / 4);
			int mask = (1 << shift) - 1;
			if ((ym2413.counter & mask) != 0) break;
			column = (ym2413.counter >> shift) & 7;
			env = max(127, env + egTable[row][column]);
			break;
		}
		case 13: // rates 52..55, weird 16-sample period
			column = ((ym2413.counter & 0xc) >> 1) | (ym2413.counter & 1);
			env = max(127, env + egTable[row][column]);
			break;
		case 14: // rates 56..59, 16-sample period, incr by either 1 or 2
			column = ((ym2413.counter & 0xc) >> 1);
			env = max(127, env + egTable[row][column] + 1);
			break;
		case 15: // rates 60..63, always increase by 2
			env = max(127, env + 2);
			break;
		}
	}
}

int16_t Operator::calcOp(YM2413& ym2413, Channel& ch, bool isCarrier, int extraPhase, int extraLevel)
{
	auto p = calcPhase(ym2413, ch) + extraPhase;
	calcEnv(ym2413, ch, isCarrier);
	// TODO use updated env already in this cycle?

	if ((env & 0x7C) == 0x7C) {
		// enevlope level reached stop-condition, return +0
		return 0;
	}
	
	// am
	auto amValue = am ? (ym2413.amCounter >> 3) : 0; // 0..13

	// ksl
	static const uint7 kslTable1[16] = {
		112, 64, 48, 38, 32, 26, 22, 18, 16, 12, 10, 8, 6, 4, 2, 0
	};
	int tmpKsl = 16 * ch.block - kslTable1[ch.fnum >> 5];
	auto kslValue = (ksl == 0) ? 0 : (max(0, tmpKsl) >> (3 - ksl));
	// alternative that might be better for software implementation:
	//   auto kslValue = kslTable[ksl][ch.block][ch.fnum >> 5];

	// total attenuation
	auto att = min(127, extraLevel + kslValue + env + amValue);
	
	auto s = lookupSin(p, wf);
	return lookupExp(s + 16 * att);
}

void ModOperator::calcMod(YM2413& ym2413, Channel& ch)
{
	auto f = fb ? ((p0 + p1) >> (8 - fb)) : 0;
	// ******* drop this *******
	f -= 1; // TODO probably a side effect of how the phase counters in
	        //      mod/car are reset. This isn't implemented yet
	auto m = calcOp(ym2413, ch, false, f, 2 * tl) >> 1; // -2047..+2047
	p1 = p0; p0 = m;
}

int16_t CarOperator::calcCar(YM2413& ym2413, Channel& ch, ModOperator& mod)
{
	return calcOp(ym2413, ch, true, 2 * mod.p0, 8 * ch.vol) >> 4;
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
		// TODO use amCounter before or after incrementing counter
		//      for this sample?
		// TODO same question for PM
		if (amDirection) {
			++amCounter;
			if (amCounter == 105) amDirection = false;
		} else {
			--amCounter;
			if (amCounter == 0) amDirection = true;
		}
		// use 'amCounter >> 3'.

		// Alternative, might be better for a software implementation:
		//++amCounter;
		//if (amCounter == sizeof(amTable)) amCounter = 0;
		//  use amTable[amCounter]
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

	y.ch[0].mod.am = 0;
	y.ch[0].mod.pm = 0;
	y.ch[0].mod.eg = 0;
	y.ch[0].mod.ksr = 0;
	y.ch[0].mod.tl = 63;
	y.ch[0].mod.fb = 0;
	y.ch[0].mod.ksl = 0;
	y.ch[0].mod.env = 127;
	y.ch[0].mod.ml = 2;
	y.ch[0].mod.wf = 0;
	y.ch[0].mod.ar = 15;
	y.ch[0].mod.dr = 15;
	y.ch[0].mod.sl = 15;
	y.ch[0].mod.rr = 15;

	y.ch[0].car.am = 0;
	y.ch[0].car.pm = 0;
	y.ch[0].car.eg = 1;
	y.ch[0].car.ksr = 0;
	y.ch[0].car.ksl = 0;
	y.ch[0].car.env = 60;
	y.ch[0].car.ml = 2;
	y.ch[0].car.wf = 0;
	y.ch[0].car.ar =  0;
	y.ch[0].car.dr =  0;
	y.ch[0].car.sl =  0;
	y.ch[0].car.rr =  0;

	for (int i = 0; i < 1024; ++i) {
		y.calc();
	}
}
