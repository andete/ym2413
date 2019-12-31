#include "model6.hh"
#include <cassert>
#include <functional>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <random>
#include <set>
#include <vector>

using namespace std;

inline std::minstd_rand0& global_urng()
{
	static std::minstd_rand0 u;
	return u;
}

int fromHexDigit(char c)
{
	if (('0' <= c) && (c <= '9')) return c - '0';
	if (('a' <= c) && (c <= 'f')) return c - 'a' + 10;
	if (('A' <= c) && (c <= 'F')) return c - 'A' + 10;
	assert(false); return -1;
}

vector<int> parse_vector(istream& is)
{
	vector<int> result;
	int i;
	while (is >> i) {
		result.push_back(i);
	}
	return result;
}

vector<int> load_vector(const string& filename)
{
	ifstream is(filename);
	if (!is) {
		std::cout << "ERROR: couldn't open file" << std::endl;
		abort();
	}
	return parse_vector(is);
}

template<typename T>
T sqr(T x)
{
	return x * x;
}

struct State
{
	uint8 fHigh = 0x7;
	uint8 fnum = 0x25;
	uint8 regs[8];
	unsigned counter;
	unsigned amCounter;
	unsigned noise = 1;
	size_t start = 1;
	int m_phase = 0;
	int c_phase = 0;
	size_t length = 40000;
	int pmOfst;
	int amOfst;
	int update;
	int p0 = 0;
	int p1 = 0;
	int m_env = 124;
	int c_env = 127;
	int hh1 = 0xd0;
	int hh2 = 0x34;
};

template<bool PRINT>
size_t match(const State& state, const vector<int>& gold, size_t best_a)
{
	YM2413 ym;
	ym.counter = state.counter;
	ym.noise_rng = state.noise;
	ym.amCounter = state.amCounter % 105;
	ym.amDirection = state.amCounter / 105;
	ym.pmOfst = state.pmOfst;
	ym.amOfst = state.amOfst;
	ym.carUpdatePhaseBeforeUse = state.update & 8;
	ym.modUpdatePhaseBeforeUse = state.update & 4;
	ym.carUpdateEnvBeforeUse   = state.update & 2;
	ym.modUpdateEnvBeforeUse   = state.update & 1;
	ym.hh1 = state.hh1;
	ym.hh2 = state.hh2;
	for (int i = 0; i < 8; ++i) {
		ym.writeReg(i, state.regs[i]);
	}

#if 0
	int state_fHigh = 0x0;
	uint8 keyOff = state_fHigh;
	uint8 keyOn  = state_fHigh | 0x10;
#else
	uint8 keyOff = state.fHigh;
	uint8 keyOn  = state.fHigh | 0x10;
#endif

	// for rhythm
	//ym.writeReg(0x17, 0x00);
	//ym.writeReg(0x27, 0x01);
	ym.writeReg(0x18, 0x00);
	ym.writeReg(0x28, 0x00);

	ym.writeReg(0x10, state.fnum);
	ym.writeReg(0x30, 0x00);
	ym.writeReg(0x20, keyOff);
	for (unsigned i = 0; i < state.start; ++i) ym.calc();

	if (0) {
		ym.writeReg(0x20, keyOn);
		for (int i = 0; i < 10000; ++i) ym.calc();
		ym.writeReg(0x20, keyOff);
		for (int i = 0; i < 10000; ++i) ym.calc();
	}

	ym.writeReg(0x20, keyOn);
	ym.ch[0].mod.env = state.m_env; //124;
	ym.ch[0].car.env = state.c_env; //127;
	ym.ch[0].mod.p0 = state.p0;
	ym.ch[0].mod.p1 = state.p1;

	if (1) { // hack for rhythm
		ym.ch[0].mod.phase = state.m_phase;
		ym.ch[0].car.phase = state.c_phase;
		ym.ch[7].mod.phase = state.m_phase; // rhythm
		ym.ch[7].car.phase = state.c_phase; //
		ym.ch[8].mod.phase = state.m_phase; // rhythm
		ym.ch[8].car.phase = state.c_phase; //
	}

	size_t i = 0;
	int c;
	for (/**/; i < 10000; ++i) {
		c = ym.calc();
		//std::cout << "DEBUG " << c << ' ' << i << '\n';
		if (c != 255) break;
	}

	// !!! for TOM !!!
	//c = ym.calc();

	size_t a = 0;
	for (int g : gold) {
		//int c = ym.calc();
		//if (PRINT) cout << g << ' ' << c << ' ' << (ym.counter & 0x7fff) << ' ' << ((ym.counter + ym.pmOfst) & 8191) << '\n';
		if (PRINT) cout << g << ' ' << c << '\n';
		a += sqr(c - g);
		if (a > best_a) return a;
		++i;
		if (i == state.length) {
			ym.writeReg(0x20, keyOff);
		}
		c = ym.calc();
	}
	return a;
}

struct Row {
	int min, max, step;
	const char* name;
	std::function<void(State&, int)> setState;
};

size_t sweep1(State& state, const vector<int>& gold, size_t best_a, const Row& r)
{
	State orig = state;
	bool improved = false;

	int best_val = -1;
	for (int i = r.min; i < r.max; i += r.step) {
		r.setState(state, i);
		auto a = match<false>(state, gold, best_a);
		if (a < best_a) {
			best_a = a;
			best_val = i;
			improved = true;
		}
	}
	if (improved) {
		cerr << "best a=" << best_a << ' ' << r.name << '=' << best_val << '\n';
		r.setState(state, best_val);
	} else {
		cerr << "not improved " << r.name << '\n';
		state = orig;
	}
	return best_a;
}

size_t sweep3(State& state, const vector<int>& gold, size_t best_a,
              const Row& r1, const Row& r2, const Row& r3)
{
	State orig = state;
	bool improved = false;

	size_t n1 = (r1.max - r1.min) / r1.step;
	size_t n2 = (r2.max - r2.min) / r2.step;
	size_t n3 = (r3.max - r3.min) / r3.step;

	int best_val1 = -1;
	int best_val2 = -1;
	int best_val3 = -1;

	if ((n1 * n2 * n3) < 10000) {
		for (int i = r1.min; i < r1.max; i += r1.step) {
			r1.setState(state, i);
			for (int j = r2.min; j < r2.max; j += r2.step) {
				r2.setState(state, j);
				for (int k = r3.min; k < r3.max; k += r3.step) {
					r3.setState(state, k);
					auto a = match<false>(state, gold, best_a);
					if (a < best_a) {
						best_a = a;
						best_val1 = i;
						best_val2 = j;
						best_val3 = k;
						improved = true;
					}
				}
			}
		}
	} else {
		auto& generator = global_urng();
		std::uniform_int_distribution<int> dist1(0, n1 - 1);
		std::uniform_int_distribution<int> dist2(0, n2 - 1);
		std::uniform_int_distribution<int> dist3(0, n3 - 1);
		auto random1 = [&]() { return dist1(generator); };
		auto random2 = [&]() { return dist2(generator); };
		auto random3 = [&]() { return dist3(generator); };

		for (int t = 0; t < 10000; ++t) {
			int i = r1.min + r1.step * random1();
			int j = r2.min + r2.step * random2();
			int k = r3.min + r3.step * random3();
			r1.setState(state, i);
			r2.setState(state, j);
			r3.setState(state, k);
			auto a = match<false>(state, gold, best_a);
			if (a < best_a) {
				best_a = a;
				best_val1 = i;
				best_val2 = j;
				best_val3 = k;
				improved = true;
			}
		}
	}

	if (improved) {
		cerr << "best a=" << best_a << ' ' << r1.name << '=' << best_val1
					    << ' ' << r2.name << '=' << best_val2
					    << ' ' << r3.name << '=' << best_val3 << '\n';
		r1.setState(state, best_val1);
		r2.setState(state, best_val2);
		r3.setState(state, best_val3);
	} else {
		cerr << "not improved " << r1.name << ' ' << r2.name << ' ' << r3.name << '\n';
		state = orig;
	}
	return best_a;
}

size_t sweep4(State& state, const vector<int>& gold, size_t best_a,
              const Row& r1, const Row& r2, const Row& r3, Row& r4)
{
	State orig = state;
	bool improved = false;

	size_t n1 = (r1.max - r1.min) / r1.step;
	size_t n2 = (r2.max - r2.min) / r2.step;
	size_t n3 = (r3.max - r3.min) / r3.step;
	size_t n4 = (r4.max - r4.min) / r4.step;

	int best_val1 = -1;
	int best_val2 = -1;
	int best_val3 = -1;
	int best_val4 = -1;

	if ((n1 * n2 * n3 * n4) < 200000) {
		for (int i = r1.min; i < r1.max; i += r1.step) {
			r1.setState(state, i);
			for (int j = r2.min; j < r2.max; j += r2.step) {
				r2.setState(state, j);
				for (int k = r3.min; k < r3.max; k += r3.step) {
					r3.setState(state, k);
					for (int l = r4.min; l < r4.max; l += r4.step) {
						r4.setState(state, l);
						auto a = match<false>(state, gold, best_a);
						if (a < best_a) {
							best_a = a;
							best_val1 = i;
							best_val2 = j;
							best_val3 = k;
							best_val4 = l;
							improved = true;
						}
					}
				}
			}
		}
	} else {
		auto& generator = global_urng();
		std::uniform_int_distribution<int> dist1(0, n1 - 1);
		std::uniform_int_distribution<int> dist2(0, n2 - 1);
		std::uniform_int_distribution<int> dist3(0, n3 - 1);
		std::uniform_int_distribution<int> dist4(0, n4 - 1);
		auto random1 = [&]() { return dist1(generator); };
		auto random2 = [&]() { return dist2(generator); };
		auto random3 = [&]() { return dist3(generator); };
		auto random4 = [&]() { return dist4(generator); };

		for (int t = 0; t < 200000; ++t) {
			int i = r1.min + r1.step * random1();
			int j = r2.min + r2.step * random2();
			int k = r3.min + r3.step * random3();
			int l = r4.min + r4.step * random4();
			r1.setState(state, i);
			r2.setState(state, j);
			r3.setState(state, k);
			r4.setState(state, l);
			auto a = match<false>(state, gold, best_a);
			if (a < best_a) {
				best_a = a;
				best_val1 = i;
				best_val2 = j;
				best_val3 = k;
				best_val4 = l;
				improved = true;
			}
		}
	}

	if (improved) {
		cerr << "best a=" << best_a << ' ' << r1.name << '=' << best_val1
					    << ' ' << r2.name << '=' << best_val2
					    << ' ' << r3.name << '=' << best_val3
					    << ' ' << r4.name << '=' << best_val4 << '\n';
		r1.setState(state, best_val1);
		r2.setState(state, best_val2);
		r3.setState(state, best_val3);
		r4.setState(state, best_val4);
	} else {
		cerr << "not improved " << r1.name << ' ' << r2.name << ' ' << r3.name << ' ' << r4.name << '\n';
		state = orig;
	}
	return best_a;
}

size_t sweep5(State& state, const vector<int>& gold, size_t best_a,
              const Row& r1, const Row& r2, const Row& r3, Row& r4, Row& r5)
{
	State orig = state;
	bool improved = false;

	size_t n1 = (r1.max - r1.min) / r1.step;
	size_t n2 = (r2.max - r2.min) / r2.step;
	size_t n3 = (r3.max - r3.min) / r3.step;
	size_t n4 = (r4.max - r4.min) / r4.step;
	size_t n5 = (r5.max - r5.min) / r5.step;

	int best_val1 = -1;
	int best_val2 = -1;
	int best_val3 = -1;
	int best_val4 = -1;
	int best_val5 = -1;

	const size_t big = 900000;
	size_t n12 = n1 * n2;
	size_t n34 = n3 * n4;
	bool small = (n12 < big) && (n34 < big) && ((n12 * n34) < big) && ((n12 * n34 * n5) < big);
	if (small) {
		for (int i = r1.min; i < r1.max; i += r1.step) {
			r1.setState(state, i);
			for (int j = r2.min; j < r2.max; j += r2.step) {
				r2.setState(state, j);
				for (int k = r3.min; k < r3.max; k += r3.step) {
					r3.setState(state, k);
					for (int l = r4.min; l < r4.max; l += r4.step) {
						r4.setState(state, l);
						for (int m = r5.min; m < r5.max; m += r5.step) {
							r5.setState(state, m);
							auto a = match<false>(state, gold, best_a);
							if (a < best_a) {
								best_a = a;
								best_val1 = i;
								best_val2 = j;
								best_val3 = k;
								best_val4 = l;
								best_val5 = m;
								improved = true;
							}
						}
					}
				}
			}
		}
	} else {
		auto& generator = global_urng();
		std::uniform_int_distribution<int> dist1(0, n1 - 1);
		std::uniform_int_distribution<int> dist2(0, n2 - 1);
		std::uniform_int_distribution<int> dist3(0, n3 - 1);
		std::uniform_int_distribution<int> dist4(0, n4 - 1);
		std::uniform_int_distribution<int> dist5(0, n5 - 1);
		auto random1 = [&]() { return dist1(generator); };
		auto random2 = [&]() { return dist2(generator); };
		auto random3 = [&]() { return dist3(generator); };
		auto random4 = [&]() { return dist4(generator); };
		auto random5 = [&]() { return dist5(generator); };

		for (size_t t = 0; t < big; ++t) {
			int i = r1.min + r1.step * random1();
			int j = r2.min + r2.step * random2();
			int k = r3.min + r3.step * random3();
			int l = r4.min + r4.step * random4();
			int m = r5.min + r5.step * random5();
			r1.setState(state, i);
			r2.setState(state, j);
			r3.setState(state, k);
			r4.setState(state, l);
			r5.setState(state, m);
			auto a = match<false>(state, gold, best_a);
			if (a < best_a) {
				best_a = a;
				best_val1 = i;
				best_val2 = j;
				best_val3 = k;
				best_val4 = l;
				best_val5 = m;
				improved = true;
			}
		}
	}

	if (improved) {
		cerr << "best a=" << best_a << ' ' << r1.name << '=' << best_val1
					    << ' ' << r2.name << '=' << best_val2
					    << ' ' << r3.name << '=' << best_val3
					    << ' ' << r4.name << '=' << best_val4
					    << ' ' << r5.name << '=' << best_val5 << '\n';
		r1.setState(state, best_val1);
		r2.setState(state, best_val2);
		r3.setState(state, best_val3);
		r4.setState(state, best_val4);
		r5.setState(state, best_val5);
	} else {
		cerr << "not improved " << r1.name << ' ' << r2.name << ' ' << r3.name << ' ' << r4.name << ' ' << r5.name << '\n';
		state = orig;
	}
	return best_a;
}

Row list[] = {
	//{39900, 40100, 1, "length", [](State& s, int i) { s.length = i; }},
	{ 0, 32768>>0, 1, "counter",    [](State& s, int i) { s.counter = i; }},
	{ 1, 8388608, 1, "noise",    [](State& s, int i) { s.noise = i; }},
	//{ 0, 8192, 1, "pmOfst", [](State& s, int i) { s.pmOfst = i; }},
	//{ 0, 210, 1, "amCounter",    [](State& s, int i) { s.amCounter = i; }},
	//{ 0,  64,  1,     "amOfst",  [](State& s, int i) { s.amOfst = i; }},
	//{ 1, 32768,  1,     "start",  [](State& s, int i) { s.start = i; }},
	{ 0, 512,  1,      "m_phase_lo",  [](State& s, int i) { s.m_phase = (s.m_phase & 0x7fe00) | i; }},
	{ 0, 0x80000, 512, "m_phase_hi",  [](State& s, int i) { s.m_phase = (s.m_phase & 0x001ff) | i; }},
	{ 0, 512,  1,      "c_phase_lo",  [](State& s, int i) { s.c_phase = (s.c_phase & 0x7fe00) | i; }},
	{ 0, 0x80000, 512, "c_phase_hi",  [](State& s, int i) { s.c_phase = (s.c_phase & 0x001ff) | i; }},
	//
	//{ 0, 256,128,  "mod.am", [](State& s, int i) { s.regs[0] = (s.regs[0] & 0x7F) | i; }},
	//{ 0, 128, 64,  "mod.pm", [](State& s, int i) { s.regs[0] = (s.regs[0] & 0xBF) | i; }},
	//{ 0,  64, 32,  "mod.eg", [](State& s, int i) { s.regs[0] = (s.regs[0] & 0xDF) | i; }},
	//{ 0,  32, 16, "mod.ksr", [](State& s, int i) { s.regs[0] = (s.regs[0] & 0xEF) | i; }},
	//{ 0,  16,  1, "mod.mul", [](State& s, int i) { s.regs[0] = (s.regs[0] & 0xF0) | i; }},

	//{ 0, 256,128,  "car.am", [](State& s, int i) { s.regs[1] = (s.regs[1] & 0x7F) | i; }},
	//{ 0, 128, 64,  "car.pm", [](State& s, int i) { s.regs[1] = (s.regs[1] & 0xBF) | i; }},
	//{ 0,  64, 32,  "car.eg", [](State& s, int i) { s.regs[1] = (s.regs[1] & 0xDF) | i; }},
	//{ 0,  32, 16, "car.ksr", [](State& s, int i) { s.regs[1] = (s.regs[1] & 0xEF) | i; }},
	//{ 0,  16,  1, "car.mul", [](State& s, int i) { s.regs[1] = (s.regs[1] & 0xF0) | i; }},

	//{ 0, 256, 64, "mod.ksl", [](State& s, int i) { s.regs[2] = (s.regs[2] & 0x3F) | i; }},
	//{ 0,  64,  1,  "tl",     [](State& s, int i) { s.regs[2] = (s.regs[2] & 0xC0) | i; }},

	//{ 0, 256, 64, "car.ksl", [](State& s, int i) { s.regs[3] = (s.regs[3] & 0x3F) | i; }},
	//{ 0,  32, 16,  "car.wf", [](State& s, int i) { s.regs[3] = (s.regs[3] & 0xEF) | i; }},
	//{ 0,  16,  8,  "mod.wf", [](State& s, int i) { s.regs[3] = (s.regs[3] & 0xF7) | i; }},
	//{ 0,   8,  1,  "fb",     [](State& s, int i) { s.regs[3] = (s.regs[3] & 0xF8) | i; }},

	//{ 0, 256, 16,  "mod.ar", [](State& s, int i) { s.regs[4] = (s.regs[4] & 0x0F) | i; }},
	//{ 0,  16,  1,  "mod.dr", [](State& s, int i) { s.regs[4] = (s.regs[4] & 0xF0) | i; }},

	//{16, 256, 16,  "car.ar", [](State& s, int i) { s.regs[5] = (s.regs[5] & 0x0F) | i; }},
	//{ 0,  16,  1,  "car.dr", [](State& s, int i) { s.regs[5] = (s.regs[5] & 0xF0) | i; }},

	//{ 0, 256, 16,  "mod.sl", [](State& s, int i) { s.regs[6] = (s.regs[6] & 0x0F) | i; }},
	//{ 0,  16,  1,  "mod.rr", [](State& s, int i) { s.regs[6] = (s.regs[6] & 0xF0) | i; }},

	//{ 0, 256, 16,  "car.sl", [](State& s, int i) { s.regs[7] = (s.regs[7] & 0x0F) | i; }},
	//{ 0,  16,  1,  "car.rr", [](State& s, int i) { s.regs[7] = (s.regs[7] & 0xF0) | i; }},

	{0,  16,  1, "update-use",  [](State& s, int i) { s.update = i; }},
	{-2048,2048,1, "mod.p0", [](State& s, int i) { s.p0 = i; }},
	{-2048,2048,1, "mod.p1", [](State& s, int i) { s.p1 = i; }},
	{124,128,  1, "mod.env", [](State& s, int i) { s.m_env = i; }},
	{124,128,  1, "car.env", [](State& s, int i) { s.c_env = i; }},

	//{0x00,0x100, 1, "hh1", [](State& s, int i) { s.hh1 = i; }},
	//{0x00,0x100, 1, "hh2", [](State& s, int i) { s.hh2 = i; }},

	//{0,1, 1, "dummy1", [](State& s, int i) { }},
	//{0,1, 1, "dummy2", [](State& s, int i) { }},
	//{0,1, 1, "dummy3", [](State& s, int i) { }},
	//{0,1, 1, "dummy4", [](State& s, int i) { }},
};

int main(int argc, char** argv)
{
	initTables();

	State state;

	std::string name = argv[1];
	auto gold = load_vector(name);

	auto l = name.size();
	assert(l >= 3);
	state.fHigh = fromHexDigit(name[l - 3]);
	state.fnum  = fromHexDigit(name[l - 2]) * 16 + fromHexDigit(name[l - 1]);

	auto pos = name.find('L');
	if (pos != std::string::npos) {
		state.length = atoi(name.c_str() + pos + 1);
	}
	if (state.length > gold.size()) {
		std::cerr << "!!!!! ERROR !!!!! length !!!!!\n"
		          << state.length << "\n"
		          << gold.size() << std::endl;
	}

	//state.fHigh = 0x7;
	//state.fnum = 0x25;
	//state.length = 40000;
	state.regs[0] = 0x01;
	state.regs[1] = 0x01;
	state.regs[2] = 0x00;
	state.regs[3] = 0x00;
	state.regs[4] = 0x00;
	state.regs[5] = 0xd8;
	state.regs[6] = 0x00;
	state.regs[7] = 0x68;
	state.counter = 21972;
	state.amCounter = 184;
	state.pmOfst = 6665;
	state.amOfst = 47;
	state.update = 12; // 12;
	state.start = 1;
	state.m_phase = 0; //0;
	state.c_phase = 144282;
	state.noise = 4790016;
	state.hh1 = 228;
	state.hh2 = 53;
// 1: 61 61 1e 17  f0 78 00 17
// 2: 13 41 1a 0d  f8 f7 23 13
// 3: 13 01 99 00  b2 c4 11 23 ??
// 4: 01 61 0e 07  8a 64 70 27
// 6: 21 22 16 05  c0 71 00 18
// BD:  01 01 18 0f df f8 6a 6d
// TOM: 05 .. 00 00 f8 .. 59 ..
// SD:  .. 01 00 00 .. d8 .. 68

	auto num_r = std::size(list);

	auto& generator = global_urng();
	std::uniform_int_distribution<int> row_dist(0, num_r - 1);
	auto random_row = [&]() { return row_dist(generator); };

	auto best_a = match<false>(state, gold, std::numeric_limits<size_t>::max());

	if (argc == 2) // no options
	while (true) {
		std::set<int> tried;
		while (tried.size() < num_r) {
			int r;
			do {
				r = random_row();
			} while (tried.count(r) != 0);

			auto new_a = sweep1(state, gold, best_a, list[r]);
			if (new_a < best_a) {
				best_a = new_a;
				tried.clear();
			}
			tried.insert(r);
		}

		int tries = 0;
		bool improved = false;
		while (tries < 3) {
			int r1 = random_row();
			int r2, r3, r4, r5;
			do {
				r2 = random_row();
			} while (r2 == r1);
			do {
				r3 = random_row();
			} while ((r3 == r1) || (r3 == r2));
			do {
				r4 = random_row();
			} while ((r4 == r1) || (r4 == r2) || (r4 == r3));
			do {
				r5 = random_row();
			} while ((r5 == r1) || (r5 == r2) || (r5 == r3) || (r5 == r4));

			std::cerr << tries << "  ";
			auto new_a = sweep5(state, gold, best_a, list[r1], list[r2], list[r3], list[r4], list[r5]);
			if (new_a < best_a) {
				best_a = new_a;
				improved = true;
			} else {
				++tries;
			}
		}
		if (!improved) break;
	}

	std::cerr << "best_a=" << best_a << "\n"
	             "length=" << state.length << '\n';
	for (int i = 0; i < 8; ++i) {
		fprintf(stderr, "%02x ", (int)state.regs[i]);
	}
	std::cerr << "\n"
	             "counter=" << state.counter << "\n"
	             "amCounter=" << state.amCounter << "\n"
	             "pmOfst=" << state.pmOfst << "\n"
	             "amOfst=" << state.amOfst << "\n"
	             "p0=" << state.p0 << "\n"
	             "p1=" << state.p1 << "\n"
	             "m_env=" << state.m_env << "\n"
	             "c_env=" << state.c_env << "\n"
	             "update=" << state.update << "\n"
	             "m_phase=" << state.m_phase << "\n"
	             "c_phase=" << state.c_phase << "\n"
	             "start=" << state.start << "\n"
	             "noise=" << state.noise << "\n"
	             "hh1=" << state.hh1 << "\n"
	             "hh2=" << state.hh2 << '\n';

	match<true>(state, gold, std::numeric_limits<size_t>::max());
}
