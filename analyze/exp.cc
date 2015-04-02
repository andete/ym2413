// https://docs.google.com/document/d/18IGx18NQY_Q1PJVZ-bHywao9bhsDoAqoIn1rIm42nwo/edit

#include <iostream>
#include <cmath>
#include <cstdint>

using namespace std;

uint16_t expTable[256];
uint16_t logsinTable[256];

void initTables()
{
	for (int i = 0; i < 256; ++i) {
		expTable[i] = round((pow(2.0, double(i) / 256.0) - 1.0) * 1024.0);
		logsinTable[i] = round(-log(sin((double(i) + 0.5) * M_PI / 256.0 / 2.0)) / log(2.0) * 256.0);
	}
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
	for (int i = 0; i < 2048; i += 16) {
		int l = lookupExp(i) / 16;
		cout << i / 16 << '\t' << i << '\t' << l << '\t' << l + 256 << endl;
	}
}

// Verified that EG levels are exact (take steps of 16, divide result by 16)
