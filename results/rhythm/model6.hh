#ifndef MODEL6_HH
#define MODEL6_HH

#include <cstdint>


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
using uint23 = uint32_t;
using int12  = int16_t;

void initTables();

struct YM2413;
struct Channel;
struct ModOperator;

struct Patch
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
};

struct CarPatch : Patch
{
};

struct ModPatch : Patch
{
	uint6 tl; // bits 5-0 in R#2
	uint3 fb; // bits 2-0 in R#3
};

struct Operator
{
	enum EgState : uint2 { DAMP, ATTACK, DECAY, SUSTAIN } egState = DAMP;
	uint7 env = 127; // 0..127
	uint19 phase = 0; // 10.9 fixed point
	bool mustReset = false;

	void reset();
	int calcPhase(YM2413& ym2413, Channel& ch, const Patch& patch);
	uint7 calcEnv(YM2413& YM2413, Channel& ch, const Patch& patch, bool isCarrier);
	int16_t calcOp(YM2413& ym2413, Channel& ch, const Patch& patch, bool isCarrier, int extraPhase, int extraLevel);
	int16_t calcOp2(YM2413& ym2413, Channel& ch, const Patch& patch, bool isCarrier, int p, int extraLevel);
};

struct ModOperator : Operator
{
	int12 p0 = 0; // delayed values for feedback calculation
	int12 p1 = 0; //  -2047..+2047

	void reset();
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
	uint4 instr;  // bits 7-4 in R#30-28
	uint4 vol;    // bits 3-0 in R#30-38

	void reset();
	int16_t calcChan(YM2413& ym2413);
	int16_t calcTom(YM2413& ym2413);
	int16_t calcSD(YM2413& ym2413);
	int16_t calcTop(YM2413& ym2413);
	int16_t calcHH(YM2413& ym2413);
};

struct YM2413
{
	static const int NUM_CHANNELS = 9;
	Channel ch[NUM_CHANNELS];
	uint32_t counter = 0;
	uint8 amCounter = 0; // 0..105, lower 3 bits are dropped on use (so 0..13 on use)
	uint1 amDirection = 0;
	uint23 noise_rng = 1; // 23 bit

	CarPatch carPatches[16];
	ModPatch modPatches[16];

	YM2413();
	void reset();
	void writeReg(uint6 reg, uint8 value);
	void writePatch(uint6 reg, uint8 value, ModPatch& mp, CarPatch& cp);
	int calc();

	unsigned pmOfst = 0;
	unsigned amOfst = 0;
	bool carUpdatePhaseBeforeUse = true;
	bool modUpdatePhaseBeforeUse = true;
	bool carUpdateEnvBeforeUse = false;
	bool modUpdateEnvBeforeUse = false;
	//int hh1 = 0xd0, hh2 = 0x34;
	int hh1 = 228, hh2 = 53;
};

#endif
