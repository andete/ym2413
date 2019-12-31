#include <cassert>
#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <optional>
#include <set>
#include <tuple>
#include <vector>

using namespace std;

using T = uint32_t;

struct LogFunc {
	T op = 0;
	T mask = 0;
        vector<pair<int, int>> p;
};

bool operator<(const LogFunc& x, const LogFunc& y)
{
	return std::tie(x.op, x.mask) < std::tie(y.op, y.mask);
}

std::ostream& operator<<(std::ostream& os, const LogFunc& func)
{
	os << std::hex << '(' << func.op << ' ' << func.mask << ')' << std::dec;
        if (1) {
            os << " [";
            for (auto& p : func.p) {
                os << '(' << p.first << ',' << p.second << ')';
            }
            os << ']';
        }
	return os;
}

bool calcLogop(LogFunc& func, int phase7, int phase8, int incr7, int incr8, const vector<bool>& gold)
{
    int p7 = phase7;
    int p8 = phase8;
	for (bool g : gold) {
		bool b7_2 = (phase7 >> 2) & 1;
		bool b7_3 = (phase7 >> 3) & 1;
		bool b7_7 = (phase7 >> 7) & 1;

		bool b8_3 = (phase8 >> 3) & 1;
		bool b8_5 = (phase8 >> 5) & 1;

		unsigned idx = (b7_2 << 0) | (b7_3 << 1) | (b7_7 << 2)
		             | (b8_3 << 3) | (b8_5 << 4);
		assert(idx < 32);
		T mask = T(1) << idx;

		if (func.mask & mask) {
			// already set
			bool l = func.op & mask;
			if (l != g) return false; // conflict
		} else {
			// set now
			assert((func.op & mask) == 0);
			if (g) func.op |= mask;
			func.mask |= mask;
		}

		phase7 += incr7;
		phase8 += incr8;
	}
	//std::cout << "DEBUG " << phase7 << ' ' << phase8 << '\n';
        func.p.emplace_back(p7, p8);
	return true;
}

void calcLogop(set<LogFunc>& result, LogFunc func, int incr7, int incr8, const vector<bool>& gold)
{
	for (int phase7 = 0; phase7 < 256; ++phase7) {
		for (int phase8 = 0; phase8 < 256; ++phase8) {
			auto copy = func;
			if (calcLogop(copy, phase7, phase8, incr7, incr8, gold)) {
				result.insert(copy);
			}
		}
	}
}

set<LogFunc> calcLogop(const set<LogFunc>& in, int incr7, int incr8, const vector<bool>& gold)
{
	set<LogFunc> result;
	for (auto func : in) {
		calcLogop(result, func, incr7, incr8, gold);
	}
	return result;
}


void insert(vector<bool>& result, size_t count, bool b)
{
	result.insert(result.end(), count, b);
}

vector<bool> create(size_t len, std::initializer_list<int> list, int repeat = 1)
{
	vector<bool> result;
	result.reserve(len);
	bool b = true;
	for (int i = 0; i < repeat; ++i) {
		for (int i : list) {
			insert(result, i, b);
			b = !b;
		}
	}
	if (result.size() != len) {
            std::cout << "ERROR " << result.size() << ' ' << len << '\n';
            assert(false);
        }
	return result;
}

vector<bool> get_000_100()
{
	return create(256, {8, 8, 8, 40}, 4);
}

vector<bool> get_000_300()
{
	return create(256, {4, 4, 4, 20}, 8);
}

vector<bool> get_100_000()
{
	return create(256, {16, 4, 12, 4, 12, 4, 12, 4, 12, 4, 12, 4, 12, 4, 12, 4,
                             8, 4, 12, 4, 12, 4, 12, 4, 12, 4, 12, 4, 12, 4, 12, 4});
}

vector<bool> get_100_000_b()
{
	return create(256, {4, 12, 4, 12, 4, 12, 4, 12, 4, 12, 4, 12, 4, 12, 4,  8,
                            4, 12, 4, 12, 4, 12, 4, 12, 4, 12, 4, 12, 4, 12, 4, 16});
}

vector<bool> get_300_000()
{
	return create(256, {8, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2,
                            4, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2}, 2);
}

vector<bool> get_300_000_b()
{
	return create(256, {2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 4,
                            2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 8}, 2);
}

vector<bool> get_100_100()
{
	return create(256, {2, 22, 2, 14, 2, 6, 2, 14, 2, 22, 2, 14, 2, 6, 2, 142});
}

vector<bool> get_300_300()
{
    // TODO re-measure
	return create(256, {2, 6, 2, 2, 2, 6, 2, 10, 2, 6, 2, 2, 2, 6, 2, 74}, 2);
}

vector<bool> get_100_300()
{
	return create(256, {1, 20, 3, 8, 1, 20, 3, 8, 1, 20, 3, 8, 1, 20, 3, 9,
                            3, 8, 1, 20, 3, 8, 1, 20, 3, 8, 1, 20, 3, 8, 1, 19});
}

vector<bool> get_300_100()
{
	return create(256, {2, 14,  2, 2,  2, 16,  2, 18,  2, 14,  2, 10,  2, 12,  2, 26}, 2);
}


bool ym2413(int phase7, int phase8)
{
    bool b7_2 = (phase7 >> 2) & 1;
    bool b7_3 = (phase7 >> 3) & 1;
    bool b7_7 = (phase7 >> 7) & 1;

    bool b8_3 = (phase8 >> 3) & 1;
    bool b8_5 = (phase8 >> 5) & 1;

    //return (b8_5 == b8_3) && (b7_7 == b7_2) && (b8_5 == b7_3);
    //return (b8_5 == b8_3) && (b7_7 == b7_2) && (b8_5 != b7_3);
    //return (b8_5 == b8_3) && (b7_7 != b7_2) && (b8_5 == b7_3);
    //return (b8_5 == b8_3) && (b7_7 != b7_2) && (b8_5 != b7_3);
    //return (b8_5 != b8_3) && (b7_7 == b7_2) && (b8_5 == b7_3);
    //return (b8_5 != b8_3) && (b7_7 == b7_2) && (b8_5 != b7_3);
    //return (b8_5 != b8_3) && (b7_7 != b7_2) && (b8_5 == b7_3);
    return (b8_5 != b8_3) && (b7_7 != b7_2) && (b8_5 != b7_3);
    //return (b8_5 == b8_3) && (b7_7 == b7_2) && (b7_3 == (b8_5 ^ b7_7));
    //return (b8_5 == b8_3) && (b7_7 == b7_2) && (b7_3 != (b8_5 ^ b7_7));
    //return (b8_5 == b8_3) && (b7_7 != b7_2) && (b7_3 == (b8_5 ^ b7_7));
    //return (b8_5 == b8_3) && (b7_7 != b7_2) && (b7_3 != (b8_5 ^ b7_7));
    //return (b8_5 != b8_3) && (b7_7 == b7_2) && (b7_3 == (b8_5 ^ b7_7));
    //return (b8_5 != b8_3) && (b7_7 == b7_2) && (b7_3 != (b8_5 ^ b7_7));
    //return (b8_5 != b8_3) && (b7_7 != b7_2) && (b7_3 == (b8_5 ^ b7_7));
    //return (b8_5 != b8_3) && (b7_7 != b7_2) && (b7_3 != (b8_5 ^ b7_7));
}

bool verify1(int phase7, int phase8, int incr7, int incr8, const vector<bool>& gold)
{
    for (bool g : gold) {
        bool f = ym2413(phase7, phase8);
        if (f != g) return false;
        phase7 += incr7;
        phase8 += incr8;
    }
    return true;
}

std::optional<pair<int, int>> verify(int incr7, int incr8, const vector<bool>& gold)
{
    for (int phase7 = 0; phase7 < 256; ++phase7) {
        for (int phase8 = 0; phase8 < 256; ++phase8) {
            if (verify1(phase7, phase8, incr7, incr8, gold)) return pair{phase7, phase8};
        }
    }
    return {};
}

void verify(const char* name, int incr7, int incr8, const vector<bool>& gold)
{
    std::cout << name << ": ";
    if (auto v = verify(incr7, incr8, gold)) {
        std::cout << "ok " << v->first << ' ' << v->second;
    } else {
        std::cout << "NOK";
    }
    std::cout << '\n';
}

int main()
{
	auto g_000_100   = get_000_100();
	auto g_000_300   = get_000_300();
	auto g_100_000   = get_100_000();
	auto g_100_000_b = get_100_000_b();
	auto g_300_000   = get_300_000();
	auto g_300_000_b = get_300_000_b();
	auto g_100_100   = get_100_100();
	auto g_300_300   = get_300_300();
	auto g_100_300   = get_100_300();
	auto g_300_100   = get_300_100();

#if 0
	for (int phase7 = 0; phase7 < 256; ++phase7) {
		LogFunc func;
		bool b = calcLogop(func, phase7, 0, 1, 0, g_100_000);
		if (b) {
			std::cout << phase7 << ' ' << func << '\n';
		}
	}
#endif

#if 0
	for (int phase8 = 0; phase8 < 256; ++phase8) {
		LogFunc func;
		bool b = calcLogop(func, 0, phase8, 0, 1, g_000_100);
		if (b) {
			std::cout << phase8 << ' ' << func << '\n';
		}
	}
#endif

#if 0
	for (int phase7 = 0; phase7 < 256; ++phase7) {
		for (int phase8 = 0; phase8 < 256; ++phase8) {
			LogFunc func;
			if (calcLogop(func, phase7, phase8, 1, 1, g_100_100)) {
				std::cout << phase7 << ' ' << phase8 << ' ' << func << '\n';
			}
		}
	}
#endif

#if 0
	WRONG
	for (int phase7 = 0; phase7 < 256; ++phase7) {
		for (int phase8 = 0; phase8 < 256; ++phase8) {
			LogFunc func;
			if (calcLogop(func, phase7, phase8, 1, 0, g_100_000) &&
			    calcLogop(func, phase7, phase8, 0, 1, g_000_100) &&
			    calcLogop(func, phase7, phase8, 1, 1, g_100_100)) {
				std::cout << phase7 << ' ' << phase8 << ' ' << func << '\n';
			}
		}
	}
#endif


	set<LogFunc> funcs = {LogFunc{}};

	////funcs = calcLogop(funcs, 1, 0, g_100_000);
	////funcs = calcLogop(funcs, 2, 0, g_300_000);
	funcs = calcLogop(funcs, 1, 0, g_100_000_b);
        if (0) {
	funcs = calcLogop(funcs, 2, 0, g_300_000_b);
	funcs = calcLogop(funcs, 0, 1, g_000_100);
	funcs = calcLogop(funcs, 0, 2, g_000_300);
	funcs = calcLogop(funcs, 1, 1, g_100_100);
	funcs = calcLogop(funcs, 2, 2, g_300_300);
	funcs = calcLogop(funcs, 1, 2, g_100_300);
	funcs = calcLogop(funcs, 2, 1, g_300_100);
        }

	for (const auto& func : funcs) {
		std::cout << func << '\n';
	}
	std::cout << '\n';

        verify("g_100_000", 1, 0, g_100_000_b);
        verify("g_300_000", 2, 0, g_300_000_b);
        verify("g_000_100", 0, 1, g_000_100);
        verify("g_000_300", 0, 2, g_000_300);
        verify("g_100_100", 1, 1, g_100_100);
        verify("g_300_300", 2, 2, g_300_300);
        verify("g_100_300", 1, 2, g_100_300);
        verify("g_300_100", 2, 1, g_300_100);
}




// (7bffffde ffffffff)
// 
// 8[5] 8[3]  7[7] 7[3] 7[2] -> out
//  0    0     0    0    0       0
//  0    0     0    0    1       1
//  0    0     0    1    0       1
//  0    0     0    1    1       1
//  0    0     1    0    0       1
//  0    0     1    0    1       0
//  0    0     1    1    0       1
//  0    0     1    1    1       1
//  0    1     0    0    0       1
//  0    1     0    0    1       1
//  0    1     0    1    0       1
//  0    1     0    1    1       1
//  0    1     1    0    0       1
//  0    1     1    0    1       1
//  0    1     1    1    0       1
//  0    1     1    1    1       1
//  1    0     0    0    0       1
//  1    0     0    0    1       1
//  1    0     0    1    0       1
//  1    0     0    1    1       1
//  1    0     1    0    0       1
//  1    0     1    0    1       1
//  1    0     1    1    0       1
//  1    0     1    1    1       1
//  1    1     0    0    0       1
//  1    1     0    0    1       1
//  1    1     0    1    0       0
//  1    1     0    1    1       1
//  1    1     1    0    0       1
//  1    1     1    0    1       1
//  1    1     1    1    0       1
//  1    1     1    1    1       0
// 
//   |000|001|011|010||110|111|101|100|
// --+---+---+---+---++---+---+---+---+
// 00| 0 |   |   |   ||   |   | 0 |   |
// 01|   |   |   |   ||   |   |   |   |
// 11|   |   |   | 0 ||   | 0 |   |   |
// 10|   |   |   |   ||   |   |   |   |
//
// 00'000 | 
// 00'101  +-> 0
// 11'010 |
// 11'111 |
//
//
//
//
// ---------------
//
// (124800 ffffffff) [(12,8)(12,8)(12,8)(12,8)(6,48)(12,8)(131,48)(184,14)]
// (184200 ffffffff) [(136,8)(136,8)(4,8)(4,8)(12,54)(4,12)(7,8)(68,14)]
// (218400 ffffffff) [(140,8)(140,8)(8,8)(8,8)(0,54)(8,12)(3,48)(56,14)]
// (248100 ffffffff) [(8,32)(8,32)(8,32)(8,32)(10,48)(0,8)(135,48)(188,14)]
// (421800 ffffffff) [(136,32)(136,32)(4,32)(4,32)(4,54)(12,12)(7,48)(60,14)]
// (481200 ffffffff) [(4,8)(4,8)(4,8)(4,8)(14,48)(4,8)(131,8)(192,14)]
// (812400 ffffffff) [(8,8)(8,8)(8,8)(8,8)(2,48)(8,8)(135,8)(196,14)]
// (842100 ffffffff) [(132,8)(132,8)(0,8)(0,8)(8,54)(0,12)(3,8)(64,14)]
// (12000048 ffffffff) [(12,0)(12,0)(12,0)(12,0)(14,16)(4,40)(131,40)(192,46)]
// (18000042 ffffffff) [(136,0)(136,0)(4,0)(4,0)(4,22)(12,44)(7,16)(60,46)]
// (21000084 ffffffff) [(140,0)(140,0)(8,0)(8,0)(8,22)(0,44)(3,40)(64,46)]
// (24000081 ffffffff) [(8,40)(8,40)(8,40)(8,40)(2,16)(8,40)(135,40)(196,46)]
// (42000018 ffffffff) [(136,40)(136,40)(4,40)(4,40)(12,22)(4,44)(7,40)(68,46)]
// (48000012 ffffffff) [(4,0)(4,0)(4,0)(4,0)(6,16)(12,40)(131,16)(184,46)]
// (81000024 ffffffff) [(8,0)(8,0)(8,0)(8,0)(10,16)(0,40)(135,16)(188,46)]
// (84000021 ffffffff) [(132,0)(132,0)(0,0)(0,0)(0,22)(8,44)(3,16)(56,46)]
//
//  logop     abcde  abcde  abcde  abcde
// 84000021   00000  00101  11010  11111     a==b   c==e   a==d
// 21000084   00010  00111  11000  11101     a==b   c==e   a!=d
// 48000012   00001  00100  11011  11110     a==b   c!=e   a==d
// 12000048   00011  00110  11001  11100     a==b   c!=e   a!=d
// 00842100   01000  01101  10010  10111     a!=b   c==e   a==d
// 00218400   01010  01111  10000  10101     a!=b   c==e   a!=d
// 00481200   01001  01100  10011  10110     a!=b   c!=e   a==d
// 00124800   01011  01110  10001  10100     a!=b   c!=e   a!=d
// 24000081   00000  00111  11010  11101     a==b   c==e   d==(a^c)
// 81000024   00010  00101  11000  11111     a==b   c==e   d!=(a^c)
// 18000042   00001  00110  11011  11100     a==b   c!=e   d==(a^c)
// 42000018   00011  00100  11001  11110     a==b   c!=e   d!=(a^c)
// 00248100   01000  01111  10010  10101     a!=b   c==e   d==(a^c)
// 00812400   01010  01101  10000  10111     a!=b   c==e   d!=(a^c)
// 00184200   01001  01110  10011  10100     a!=b   c!=e   d==(a^c)
// 00421800   01011  01100  10001  10110     a!=b   c!=e   d!=(a^c)
