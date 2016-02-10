#include <iostream>
#include <random>
#include <set>

using namespace std;

unsigned counter;
unsigned level; // 0-127
unsigned rate; // 0-63

char incr[4][8] = {
/* 0 */	{ 0,1,0,1,0,1,0,1 }, // this matches the measurements
//      { 1,0,1,0,1,0,1,0 }, // this does not

//	{ 1,1,0,1,0,1,0,1 }, //
//	{ 0,1,1,1,0,1,0,1 }, //
	{ 0,1,0,1,1,1,0,1 }, // matches measurements
//	{ 0,1,0,1,0,1,1,1 }, //
//      { 1,1,1,0,1,0,1,0 }, //
//      { 1,0,1,1,1,0,1,0 }, //
//      { 1,0,1,0,1,1,1,0 }, //
//      { 1,0,1,0,1,0,1,1 }, //

//	{ 1,1,0,1,1,1,0,1 }, //
	{ 0,1,1,1,0,1,1,1 }, // matches measurements
//	{ 1,1,1,0,1,1,1,0 }, //
//	{ 1,0,1,1,1,0,1,1 }, //

	{ 0,1,1,1,1,1,1,1 }, // matches measurements
//	{ 1,1,0,1,1,1,1,1 }, //
//	{ 1,1,1,1,0,1,1,1 }, //
//	{ 1,1,1,1,1,1,0,1 }, //
//	{ 1,0,1,1,1,1,1,1 }, //
//	{ 1,1,1,0,1,1,1,1 }, //
//	{ 1,1,1,1,1,0,1,1 }, //
//	{ 1,1,1,1,1,1,1,0 }, //
};

inline std::minstd_rand0& global_urng()
{
	static std::minstd_rand0 u;
	return u;
}

inline uint32_t random_32bit()
{
	static std::uniform_int_distribution<uint32_t> d;
	using parm_t = decltype(d)::param_type;
	return d(global_urng(), parm_t{0, 0xffffffff});
}

void step()
{
	unsigned shift = 13 - (rate / 4);
	unsigned mask = (1 << shift) - 1;

	++counter;
	if ((counter & mask) != 0) return;
	unsigned sub = (counter >> shift) & 7;
	if (incr[rate & 3][sub] == 0) return;

	++level;
}

unsigned countSteps()
{
	unsigned l = level;
	unsigned s = 0;
	while (l == level) {
		++s;
		step();
	}
	return s;
}

int main()
{
	multiset<unsigned> p;
	for (int i = 0; i < 5000; ++i) {
		counter = random_32bit();
		level = 0;
		rate = 22;

		countSteps();
		for (int j = 0; j < 8; ++j) {
			cout << hex << " 0x" << countSteps();
		}
		cout << "  | ";
		rate = 10;
		unsigned s = countSteps();
		p.insert(s);
		cout << hex << "0x" << s;
		cout << " | ";
		for (int j = 0; j < 3; ++j) {
			cout << hex << " 0x" << countSteps();
		}
		cout << endl;
	}
	set<unsigned> p2(begin(p), end(p));
	for (unsigned s : p2) {
		cout << hex << " 0x" << s << '(' << p.count(s) << ')';
	}
	cout << endl;
}
