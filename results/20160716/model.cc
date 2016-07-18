#include <algorithm>
#include <iostream>
#include <cmath>
#include <cstdint>

using namespace std;

uint16_t expTable[256];
uint16_t logsinTable[256];

void initTables()
{
	for (int i = 0; i < 256; ++i) {
		logsinTable[i] = round(-log2(sin((double(i) + 0.5) * M_PI / 256.0 / 2.0)) * 256.0);
		expTable[i] = round((exp2(double(i) / 256.0) - 1.0) * 1024.0);
	}
}

// input:  'val' 0..1023  (10 bit)
// output: 1+15 bits (sign-magnitude representation)
uint16_t lookupSin(uint16_t val, bool wf)
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

int16_t lookupExp(uint16_t val)
{
	bool sign = val & 0x8000;
	int t = (expTable[(val & 0xFF) ^ 0xFF] << 1) | 0x0800;
	int result = t >> ((val & 0x7F00) >> 8);
	if (sign) result = ~result;
	return result;
}

int main()
{
	initTables();

	int tl = 0; // 0..63
	int fb = 0; // 0..7
	int vol = 0; // 0..15
	int kslM = 0; // 0 .. 112
	int kslC = 0; // 0 .. 112
	int envM = 127; // 0 .. 127
	int envC = 120; // 0 .. 127
	int amM = 0; // 0 .. 13
	int amC = 0; // 0 .. 13
	bool wfM = 0;
	bool wfC = 1;
	int16_t p0 = 0;
	int16_t p1 = 0;

	for (int i = 0; i < 1024; ++i) {
		// modulator
		auto f = fb ? (p0 + p1) >> (8 - fb) : 0;
		int m = 0; // corresponds to '+0'
		if ((envM & 0x7C) != 0x7C) {
			auto s = lookupSin((i - 1) + f, wfM);
			auto att = min(127, 2 * tl  + kslM + envM + amM);
			m = lookupExp(s + 16 * att) >> 1;
		}
		p1 = p0; p0 = m;

		// carrier
		int c = 0; // corresponds to '+0'
		if ((envC & 0x7C) != 0x7C) {
			auto s = lookupSin(i + 2 * m, wfC);
			auto att = min(127, 8 * vol + kslC + envC + amC);
			c = lookupExp(s + 16 * att) >> 4;
		}

		cout << 255 - c << endl;
	}
}
