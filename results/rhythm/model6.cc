#include "model6.hh"
#include <algorithm>
#include <iostream>
#include <cassert>
#include <cmath>

using namespace std;

static uint10 expTable   [256]; // values between 0..1018  (last bit unused on YM2413?)
static uint12 logsinTable[256]; // values between 0..2137

void initTables()
{
	for (int i = 0; i < 256; ++i) {
		logsinTable[i] = round(-log2(sin((double(i) + 0.5) * M_PI / 256.0 / 2.0)) * 256.0);
		expTable[i] = round((exp2(double(i) / 256.0) - 1.0) * 1024.0);
	}
}

// input:  'val' 0..1023  (10 bit)
// output: 1+12 bits (sign-magnitude representation)
static uint16_t lookupSin(uint10 val, uint1 wf)
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
static int16_t lookupExp(uint16_t val)
{
	bool sign = val & 0x8000;
	int t = (expTable[(val & 0xFF) ^ 0xFF] << 1) | 0x0800;
	int result = t >> ((val & 0x7F00) >> 8);
	if (sign) result = ~result;
	return result;
}


void Operator::reset()
{
	egState = DAMP;
	env = 127;
	phase = 0;
}

int Operator::calcPhase(YM2413& ym2413, Channel& ch, const Patch& patch)
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

	int pmValue = patch.pm ? pmTable[ch.fnum >> 6][((ym2413.counter + ym2413.pmOfst) >> 10) & 7] : 0; // -7..+7
	auto step = (((2 * ch.fnum + pmValue) * mlTab[patch.ml]) << ch.block) >> 2;
	if (0 && mustReset) { // seems worse ??
		// TODO reset before/after use/increment?
		mustReset = false;
		phase = 0;
	}
	/* first tried this*/ phase += step; // TODO already use incremented value for this sample?
	int result = phase >> 9; // drop 9 fractional bits
	/* doesn't seem better (but also no worse?) */ //phase += step; // only increment after use
	if (1 && mustReset) { // always used this  .. seems better ??
		// TODO reset before/after use/increment?
		mustReset = false;
		phase = 0;
	}
	return result;
	//return phase >> 9; // drop 9 fractional bits
}

uint7 Operator::calcEnv(YM2413& ym2413, Channel& ch, const Patch& patch, bool isCarrier)
{
	// Maximum envelope level reached at env=124 (-45dB) or higher
	bool maxEnv = (env >> 2) == 0x1F;

	// note: no need to check keyon (egState doesn't matter when keyon==0)
	if ((egState == DAMP) && maxEnv && isCarrier && ch.keyon) {
		// switch DAMP->ATTACK (or skip attack for rate=15)
		if (patch.ar == 15) {
			env = 0;
			egState = DECAY;
		} else {
			egState = ATTACK;
		}
		// *** phase = 0; // *** NO reset for SD
		// also change corresponding modulator operator
		auto& mod = ch.mod;
		auto& modPatch = ym2413.modPatches[ch.instr];
		if (modPatch.ar == 15) {
			mod.env = 0;
			mod.egState = DECAY;
		} else {
			mod.egState = ATTACK;
		}
		//mod.mustReset = true;  // TODO not for rhythm !!!
	} else if ((egState == ATTACK) && (env == 0)) {
		egState = DECAY;
	} else if ((egState == DECAY) && ((env >> 3) == patch.sl)) {
		egState = SUSTAIN;
	}

	// Attack or decay, and at what rate?
	bool attack;
	uint4 basicRate;
	if (!ch.keyon && isCarrier) {
		// release state
		// note: modulator does NOT have a release state!
		attack = false;
		basicRate = patch.eg ? patch.rr
		                     : ch.sustain ? 5 : 7;
	} else {
		switch (egState) {
		case DAMP:
			attack = false;
			basicRate = 12;
			break;
		case ATTACK:
			attack = true;
			basicRate = patch.ar;
			break;
		case DECAY:
			attack = false;
			basicRate = patch.dr;
			break;
		case SUSTAIN:
			attack = false;
			basicRate = patch.eg ? 0 : patch.rr;
			break;
		default:
			assert(false);
			attack = false;
			basicRate = 0;
		}
	}

	// Calculate effective rate
	uint6 rate = 4 * basicRate;
	if (rate) {
		// Rks adjustment, but rate=0 stays zero
		uint4 bf = (ch.block << 1) | (ch.fnum >> 8); // block(2:1):fnum(8)
		uint4 rks = patch.ksr ? bf : (bf >> 2);
		rate = min(63, rate + rks);
	}
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
			if (((ym2413.counter + 3) & mask) != 0) break;
			column = ((ym2413.counter + 3) >> shift) & 7;
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
			env = min(127, env + egTable[row][column]);
			break;
		}
		case 13: // rates 52..55, weird 16-sample period
			column = ((ym2413.counter & 0xc) >> 1) | (ym2413.counter & 1);
			env = min(127, env + egTable[row][column]);
			break;
		case 14: // rates 56..59, 16-sample period, incr by either 1 or 2
			column = ((ym2413.counter & 0xc) >> 1);
			env = min(127, env + egTable[row][column] + 1);
			break;
		case 15: // rates 60..63, always increase by 2
			env = min(127, env + 2);
			break;
		}
	}

	auto result = env;
	/*if (mustReset) {
		if (patch.ar == 15) {
			env = 0;
			egState = DECAY;
		} else {
			egState = ATTACK;
		}
	}*/
	return result;
}

int16_t Operator::calcOp(YM2413& ym2413, Channel& ch, const Patch& patch, bool isCarrier, int extraPhase, int extraLevel)
{
	// TODO use update phase already in this cycle?
	int p;
	if (/*true ||*/
	    ( isCarrier && ym2413.carUpdatePhaseBeforeUse) ||
	    (!isCarrier && ym2413.modUpdatePhaseBeforeUse)) {
		// this seems best?
		p = calcPhase(ym2413, ch, patch) + extraPhase;
	} else {
		p = (phase >> 9) + extraPhase;
		calcPhase(ym2413, ch, patch);
	}
	return calcOp2(ym2413, ch, patch, isCarrier, p, extraLevel);
}
int16_t Operator::calcOp2(YM2413& ym2413, Channel& ch, const Patch& patch, bool isCarrier, int p, int extraLevel)
{
	// TODO use updated env already in this cycle?
	uint7 env2;
	if (/*false &&*/
	    (( isCarrier && ym2413.carUpdateEnvBeforeUse) ||
	     (!isCarrier && ym2413.modUpdateEnvBeforeUse))) {
		env2 = calcEnv(ym2413, ch, patch, isCarrier);
	} else {
		// this seems best?
		env2 = env;
		calcEnv(ym2413, ch, patch, isCarrier);
	}

	//mustReset = false;

	if ((env2 & 0x7C) == 0x7C) {
		// envelope level reached stop-condition, return +0
		return 0;
	}
	
	// am
	auto amValue = patch.am ? (ym2413.amCounter >> 3) : 0; // 0..13

	// ksl
	static const uint7 kslTable1[16] = {
		112, 64, 48, 38, 32, 26, 22, 18, 16, 12, 10, 8, 6, 4, 2, 0
	};
	int tmpKsl = 16 * ch.block - kslTable1[ch.fnum >> 5];
	auto kslValue = (patch.ksl == 0) ? 0 : (max(0, tmpKsl) >> (3 - patch.ksl));
	// alternative that might be better for software implementation:
	//   auto kslValue = kslTable[ksl][ch.block][ch.fnum >> 5];

	// total attenuation
	auto att = min(127, extraLevel + kslValue + env2 + amValue);
	
	auto s = lookupSin(p, patch.wf);
	return lookupExp(s + 16 * att);
}

void ModOperator::reset()
{
	Operator::reset();
	p0 = 0;
	p1 = 0;
}

void ModOperator::calcMod(YM2413& ym2413, Channel& ch)
{
	const auto& modPatch = ym2413.modPatches[ch.instr];
	auto f = modPatch.fb ? ((p0 + p1) >> (8 - modPatch.fb)) : 0;
	auto m = calcOp(ym2413, ch, modPatch, false, f, 2 * modPatch.tl) >> 1; // -2047..+2047
	p1 = p0; p0 = m;
}

int16_t CarOperator::calcCar(YM2413& ym2413, Channel& ch, ModOperator& mod)
{
	return calcOp(ym2413, ch, ym2413.carPatches[ch.instr], true, 2 * mod.p0, 8 * ch.vol) >> 4;
}

void Channel::reset()
{
	mod.reset();
	car.reset();
	fnum = 0;
	sustain = 0;
	keyon = 0;
	block = 0;
	instr = 0;
	vol = 0;
}

int16_t Channel::calcChan(YM2413& ym2413)
{
	mod.calcMod(ym2413, *this);
	return car.calcCar(ym2413, *this, mod);
}

int16_t Channel::calcTom(YM2413& ym2413)
{
	//int tomPatch = 18;
	int tomPatch = 0;
	int tomVol = instr;
	return mod.calcOp(ym2413, *this, ym2413.modPatches[tomPatch], true, 2 * mod.p0, 8 * tomVol) >> 4;
}

int16_t Channel::calcSD(YM2413& ym2413)
{
	//int sdPatch = 17;
	int sdPatch = 0;
	int sdVol = instr;

	auto& patch = ym2413.carPatches[sdPatch];
	int p1 = car.calcPhase(ym2413, *this, patch);
	//int p = ((p1 & 0x100) + 0x100) ^ (((ym2413.noise_rng & 1) ^ 1) << 8); // flip of noise bit seems correct
	//if (p == 0x200) p = 0;
	// verified: p != 0x200
	//         : noise bit NOT inverted
	int p = (((p1 >> 8) ^ ym2413.noise_rng) & 1)
	      ? ((ym2413.noise_rng & 1) ? 0x100 : 0x300)
	      : 0;

	return car.calcOp2(ym2413, *this, patch, true, p, 8 * sdVol) >> 4;
}

int16_t Channel::calcTop(YM2413& ym2413)
{
	//int patch = 18;
	int topPatch = 0;
	int topVol = instr;

	auto& mpatch = ym2413.modPatches[topPatch];
	auto& patch = ym2413.carPatches[topPatch];
	auto& ch7 = ym2413.ch[7];
	int pm7 = ch7.mod.calcPhase(ym2413, ch7, mpatch); // TODO actually different patch
	int pc8 = car.calcPhase(ym2413, *this, patch);

	bool b72 = (pm7 >> 2) & 1;
	bool b73 = (pm7 >> 3) & 1;
	bool b77 = (pm7 >> 7) & 1;
	bool b83 = (pc8 >> 3) & 1;
	bool b85 = (pc8 >> 5) & 1;
	//int p = (((b72 ^ b77) | b73) | (b83 |  b85)) ? 0x300 : 0x100;
	//int p = (((b72 ^ b77) | b73) ^ (b83 & !b85)) ? 0x300 : 0x100;
	//int p = (((b72 ^ b77) | b73) | ((b83 & b85) | b77)) ? 0x300 : 0x100;
	int p = ((b85 ^ b83) & (b77 ^ b72) & (b85 ^ b73)) ? 0x100 : 0x300;

	return car.calcOp2(ym2413, *this, patch, true, p, 8 * topVol) >> 4;
}

int16_t Channel::calcHH(YM2413& ym2413)
{
	//int patch = 18;
	int hhPatch = 0;
	int hhVol = instr;

	auto& patch = ym2413.modPatches[hhPatch];
	auto& cpatch = ym2413.carPatches[hhPatch];
	auto& ch8 = ym2413.ch[8];
	int pm7 = mod.calcPhase(ym2413, *this, patch);
	int pc8 = ch8.car.calcPhase(ym2413, ch8, cpatch); // TODO actually different patch

	bool b72 = (pm7 >> 2) & 1;
	bool b73 = (pm7 >> 3) & 1;
	bool b77 = (pm7 >> 7) & 1;
	bool b83 = (pc8 >> 3) & 1;
	bool b85 = (pc8 >> 5) & 1;
	bool pp = (b85 ^ b83) & (b77 ^ b72) & (b85 ^ b73);

	int p = (ym2413.noise_rng & 1) ? (pp ? ym2413.hh1 /*0xd0*/ : (0x200 + ym2413.hh2) /*0x234*/)
	                               : (pp ? ym2413.hh2 /*0x34*/ : (0x200 + ym2413.hh1) /*0x2d0*/);

	return mod.calcOp2(ym2413, *this, patch, true, p, 8 * hhVol) >> 4;
}

YM2413::YM2413()
{
	// Instrument ROM from YM2413Burczynski
	//  TODO verify this
	static const uint8 table[16 + 3][8] = {
		// MULT  MULT modTL DcDmFb AR/DR AR/DR SL/RR SL/RR
		//   0     1     2     3     4     5     6     7
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, //  0 user instrument
		{ 0x61, 0x61, 0x1e, 0x17, 0xf0, 0x7f, 0x00, 0x17 }, //  1 violin
		{ 0x13, 0x41, 0x16, 0x0e, 0xfd, 0xf4, 0x23, 0x23 }, //  2 guitar
		{ 0x03, 0x01, 0x9a, 0x04, 0xf3, 0xf3, 0x13, 0xf3 }, //  3 piano
		{ 0x11, 0x61, 0x0e, 0x07, 0xfa, 0x64, 0x70, 0x17 }, //  4 flute
		{ 0x22, 0x21, 0x1e, 0x06, 0xf0, 0x76, 0x00, 0x28 }, //  5 clarinet
		{ 0x21, 0x22, 0x16, 0x05, 0xf0, 0x71, 0x00, 0x18 }, //  6 oboe
		{ 0x21, 0x61, 0x1d, 0x07, 0x82, 0x80, 0x17, 0x17 }, //  7 trumpet
		{ 0x23, 0x21, 0x2d, 0x16, 0x90, 0x90, 0x00, 0x07 }, //  8 organ
		{ 0x21, 0x21, 0x1b, 0x06, 0x64, 0x65, 0x10, 0x17 }, //  9 horn
		{ 0x21, 0x21, 0x0b, 0x1a, 0x85, 0xa0, 0x70, 0x07 }, // 10 synthesizer
		{ 0x23, 0x01, 0x83, 0x10, 0xff, 0xb4, 0x10, 0xf4 }, // 11 harpsichord
		{ 0x97, 0xc1, 0x20, 0x07, 0xff, 0xf4, 0x22, 0x22 }, // 12 vibraphone
		{ 0x61, 0x00, 0x0c, 0x05, 0xc2, 0xf6, 0x40, 0x44 }, // 13 synthesizer bass
		{ 0x01, 0x01, 0x56, 0x03, 0x94, 0xc2, 0x03, 0x12 }, // 14 acoustic bass
		{ 0x21, 0x01, 0x89, 0x03, 0xf1, 0xe4, 0xf0, 0x23 }, // 15 electric guitar
		// drum instruments definitions
		// MULTI MULTI modTL  xxx  AR/DR AR/DR SL/RR SL/RR
		//   0     1     2     3     4     5     6     7
		//{ 0x07, 0x21, 0x14, 0x00, 0xee, 0xf8, 0xff, 0xf8 },
		//{ 0x01, 0x31, 0x00, 0x00, 0xf8, 0xf7, 0xf8, 0xf7 },
		//{ 0x25, 0x11, 0x00, 0x00, 0xf8, 0xfa, 0xf8, 0x55 }
		{ 0x01, 0x01, 0x16, 0x00, 0xfd, 0xf8, 0x2f, 0x6d },// 16 BD(multi verified, modTL verified, mod env - verified(close), carr. env verifed)
		{ 0x01, 0x01, 0x00, 0x00, 0xd8, 0xd8, 0xf9, 0xf8 },// 17 HH(multi verified), SD(multi not used)
		{ 0x05, 0x01, 0x00, 0x00, 0xf8, 0xba, 0x49, 0x55 },// 18 TOM(multi,env verified), TOP CYM(multi verified, env verified)
	};

	// Instrument ROM from YM2413Okazaki
	//	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // user instrument
	//	{ 0x61, 0x61, 0x1e, 0x17, 0xf0, 0x7f, 0x00, 0x17 }, // violin
	//	{ 0x13, 0x41, 0x16, 0x0e, 0xfd, 0xf4, 0x23, 0x23 }, // guitar
	//	{ 0x03, 0x01, 0x9a, 0x04, 0xf3, 0xf3, 0x13, 0xf3 }, // piano
	//	{ 0x11, 0x61, 0x0e, 0x07, 0xfa, 0x64, 0x70, 0x17 }, // flute
	//	{ 0x22, 0x21, 0x1e, 0x06, 0xf0, 0x76, 0x00, 0x28 }, // clarinet
	//	{ 0x21, 0x22, 0x16, 0x05, 0xf0, 0x71, 0x00, 0x18 }, // oboe
	//	{ 0x21, 0x61, 0x1d, 0x07, 0x82, 0x80, 0x17, 0x17 }, // trumpet
	//	{ 0x23, 0x21, 0x2d, 0x16, 0x90, 0x90, 0x00, 0x07 }, // organ
	//	{ 0x21, 0x21, 0x1b, 0x06, 0x64, 0x65, 0x10, 0x17 }, // horn
	//	{ 0x21, 0x21, 0x0b, 0x1a, 0x85, 0xa0, 0x70, 0x07 }, // synthesizer
	//	{ 0x23, 0x01, 0x83, 0x10, 0xff, 0xb4, 0x10, 0xf4 }, // harpsichord
	//	{ 0x97, 0xc1, 0x20, 0x07, 0xff, 0xf4, 0x22, 0x22 }, // vibraphone
	//	{ 0x61, 0x00, 0x0c, 0x05, 0xc2, 0xf6, 0x40, 0x44 }, // synthesizer bass
	//	{ 0x01, 0x01, 0x56, 0x03, 0x94, 0xc2, 0x03, 0x12 }, // acoustic bass
	//	{ 0x21, 0x01, 0x89, 0x03, 0xf1, 0xe4, 0xf0, 0x23 }, // electric guitar
	//	{ 0x07, 0x21, 0x14, 0x00, 0xee, 0xf8, 0xff, 0xf8 },
	//	{ 0x01, 0x31, 0x00, 0x00, 0xf8, 0xf7, 0xf8, 0xf7 },
	//	{ 0x25, 0x11, 0x00, 0x00, 0xf8, 0xfa, 0xf8, 0x55 }

	for (int i = 0; i < 16; ++i) {
		ModPatch& mp = modPatches[i];
		CarPatch& cp = carPatches[i];
		for (int j = 0; j < 8; ++j) {
			writePatch(j, table[i][j], mp, cp);
		}
	}

	reset();
}

void YM2413::reset()
{
	carPatches[0].am = 0;
	carPatches[0].pm = 0;
	carPatches[0].eg = 0;
	carPatches[0].ksr = 0;
	carPatches[0].ml = 0;

	carPatches[0].ksl = 0;
	carPatches[0].wf = 0;

	carPatches[0].ar = 0;
	carPatches[0].dr = 0;
	carPatches[0].sl = 0;
	carPatches[0].rr = 0;

	modPatches[0].am = 0;
	modPatches[0].pm = 0;
	modPatches[0].eg = 0;
	modPatches[0].ksr = 0;
	modPatches[0].ml = 0;

	modPatches[0].ksl = 0;
	modPatches[0].wf = 0;

	modPatches[0].ar = 0;
	modPatches[0].dr = 0;
	modPatches[0].sl = 0;
	modPatches[0].rr = 0;

	modPatches[0].tl = 0;
	modPatches[0].fb = 0;

	for (int c = 0; c < NUM_CHANNELS; ++c) {
		ch[c].reset();
	}
	counter = 0;
	amCounter = 0;
	amDirection = 0;
}

void YM2413::writePatch(uint6 reg, uint8 value, ModPatch& mp, CarPatch& cp)
{
	switch (reg) {
	case 0:
		mp.am  = (value >> 7) &  1;
		mp.pm  = (value >> 6) &  1;
		mp.eg  = (value >> 5) &  1;
		mp.ksr = (value >> 4) &  1;
		mp.ml  = (value >> 0) & 15;
		break;
	case 1:
		cp.am  = (value >> 7) &  1;
		cp.pm  = (value >> 6) &  1;
		cp.eg  = (value >> 5) &  1;
		cp.ksr = (value >> 4) &  1;
		cp.ml  = (value >> 0) & 15;
		break;
	case 2:
		mp.ksl = (value >> 6) &  3;
		mp.tl  = (value >> 0) & 63;
		break;
	case 3:
		cp.ksl = (value >> 6) & 3;
		cp.wf  = (value >> 4) & 1;
		mp.wf  = (value >> 3) & 1;
		mp.fb  = (value >> 0) & 7;
		break;
	case 4:
		mp.ar = (value >> 4) & 15;
		mp.dr = (value >> 0) & 15;
		break;
	case 5:
		cp.ar = (value >> 4) & 15;
		cp.dr = (value >> 0) & 15;
		break;
	case 6:
		mp.sl = (value >> 4) & 15;
		mp.rr = (value >> 0) & 15;
		break;
	case 7:
		cp.sl = (value >> 4) & 15;
		cp.rr = (value >> 0) & 15;
		break;
	}
}

void YM2413::writeReg(uint6 reg, uint8 value)
{
	assert(reg < 64);
	auto& c = ch[(reg & 0xF) % 9];
	switch (reg >> 4) {
	case 0: // 0x00-0x0F
		if (reg < 8) {
			writePatch(reg, value, modPatches[0], carPatches[0]);
		} else if (reg == 0x0E) {
			// TODO rhythm
		} else if (reg == 0x0F) {
			// TODO test register
		}
		break;
	case 1: // 0x10-0x1F
		c.fnum = (c.fnum & 0x100) | value;
		break;
	case 2: { // 0x20-0x2F
		bool old = c.keyon;
		c.fnum    = (c.fnum & 0x0FF) | ((value & 1) << 8);
		c.block   = (value >> 1) & 7;
		c.keyon   = (value >> 4) & 1;
		c.sustain = (value >> 5) & 1;

		if (!old && c.keyon) {
			// key-off -> key-on
			c.mod.egState = Operator::DAMP;
			c.car.egState = Operator::DAMP;
		}
		break;
	}
	case 3: // 0x30-0x3F
		c.instr = (value >> 4) & 15;
		c.vol   = (value >> 0) & 15;
		break;
	}
}

int YM2413::calc()
{
	++counter;

	// noise
	if (noise_rng & 1) {
		noise_rng ^= 0x800302;
	}
	noise_rng >>= 1;

	// LFO AM
	if ((counter & 63) == amOfst) {
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

	//auto out = ch[0].calcChan(*this);
	//auto out = ch[0].calcTom(*this);
	auto out = ch[0].calcSD(*this);
	//auto out = ch[0].calcTop(*this);
	//auto out = ch[0].calcHH(*this);
	//std::cerr << '\n';
	//std::cerr << '\n';
	return 255 - out;
}

