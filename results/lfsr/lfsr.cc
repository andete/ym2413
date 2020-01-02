// https://en.wikipedia.org/wiki/Berlekamp%E2%80%93Massey_algorithm

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <set>
#include <vector>
using namespace std;


std::vector<int> BerlekampMassey(const std::vector<int>& s)
{
	int N = s.size();
	std::vector<int> c(N, 0);
	std::vector<int> b(N, 0);
	c[0] = true;
	b[0] = true;
	int l = 0;
	int m = -1;

	for (int n = 0; n < N; ++n) {
		bool d = false;
		for (int i = 0; i <= l; ++i) {
			d ^= c[i] & s[n - i];
		}
		if (d) {
			auto t = c;
			for (int i = 0; (i + n - m) < N; ++i) {
				c[i + n - m] ^= b[i];
			}
			if (l <= (n >> 1)) {
				l = n + 1 - l;
				m = n;
				b = t;
			}
		}
	}
	c.resize(l + 1);
	return c;
}

void generate_seq(std::vector<int>& s, const std::vector<int>& poly, size_t num)
{
	auto N = poly.size();
	for (int i = 0; i < num; ++i) {
		auto sn = s.size();
		bool t = false;
		for (int j = 1; j < N; ++j) {
			t ^= poly[j] & s[sn - j];
		}
		s.push_back(t);
	}
}

unsigned gen_constant(const std::vector<int>& poly)
{
	unsigned result = 0;
	unsigned bit = 1;
	for (int i = 1; i < poly.size(); ++i) {
		if (poly[i]) result |= bit;
		bit <<= 1;
	}
	return result;
}

bool step(unsigned& s, unsigned poly)
{
	bool result = s & 1;
	s >>= 1;
	if (result) s ^= poly;
	return result;
}

std::vector<int> gen_seq(unsigned c, unsigned s, unsigned num)
{
	vector<int> result;
	for (int i = 0; i < num; ++i) {
		result.push_back(step(s, c));
	}
	return result;
}

void print(const vector<int>& v)
{
	for (int i = 0; i < v.size(); ++i) {
		cout << v[i] << (((i % 10) == 9) ? '\n' : ' ');
	}
	cout << '\n';
}

bool testPeriod(const vector<int>& v, int p)
{
	int N = v.size();
	assert(p <= N);
	for (int i = 0; i < p; ++i) {
		int j = i;
		while (true) {
			j += p;
			if (j >= N) break;
			if (v[j] != v[i]) return false;
		}
	}
	return true;
}

int calcPeriod(const vector<int>& v)
{
	int N = v.size();
	for (int p = 1; p <= N; ++p) {
		if (testPeriod(v, p)) return p;
	}
	return -1;
}

void analyse(unsigned order, unsigned poly)
{
	cout << "Analyse\n";
	unsigned n = 1 << order;
	set<unsigned> seen;
	for (unsigned i = 0; i < n; ++i) {
		if (seen.count(i)) continue;
		int c = 0;
		unsigned t = i;
		while (!seen.count(t)) {
			seen.insert(t);
			++c;
			step(t, poly);
		}
		cout << i << ": " << c << endl;
	}
	cout << '\n';
}


int main()
{
#if 0
	vector<int> s = { 1,1,1,0,0,1,0,0,1,0,1 };
#elif 0
	vector<int> s = {
		0,1,1,1,1,1,0,0,0,0,
		1,0,1,1,1,0,1,0,1,0,
		1,1,1,1,0,1,0,1,0,1,
		0,1,1,0,0,1,0,1,1,0,
		1,0,1,0,1,0,0,0,1,1,
		0,0,1,1,1,1,1,0,1,0,
		1,1,0,0,1,0,0,1,1,0,
		0,0,1,1,0,1,1,0,0,0,
		0,0,0,1,1,1,0,0,1,0,
		0,1,0,1,1,1,0,0,0,1,
	};
	for (auto& e : s) e = 1-e;
#elif 0
	// measured on real ym2413
	vector<int> s = {
		1,1,0,1,0,1,0,0,1,0,
		0,1,1,1,0,1,1,0,0,1,
		0,0,1,1,0,1,0,1,1,1,
		0,1,0,1,1,1,0,0,0,0,
		1,0,1,1,0,1,0,0,0,1,
		1,0,0,1,1,1,0,0,1,0,
		0,1,1,0,1,0,0,1,0,1,
		1,0,0,0,0,1,1,1,0,1,
		1,0,0,1,0,1,0,0,0,1,
		0,1,1,1,0,0,0,1,1,1,
		0,0,0,1,1,0,1,1,1,1,
		1,0,1,0,0,1,0,0,0,1,
	};
#else
	// nuke
	/*vector<int> s = {
		1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
		0, 0, 0, 1, 0, 0, 0, 1, 0, 0,
		0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
		0, 1, 0, 0, 0, 1, 1, 0, 0, 0,
		0, 0, 0, 0, 1, 1, 0, 0, 0, 1,
		0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
		0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
		0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
		1, 1, 1, 0, 0, 1, 0, 0, 0, 1,
	};*/
	// nuke * 18
	vector<int> s = {
		0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
		0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
		0, 1, 1, 1, 0, 1, 1, 1, 1, 0,
		0, 1, 0, 1, 1, 0, 0, 1, 1, 1,
		0, 1, 1, 0, 1, 1, 0, 0, 0, 0,
		1, 1, 0, 0, 0, 0, 1, 0, 1, 1,
		1, 0, 1, 1, 1, 1, 0, 1, 0, 1,
		0, 0, 0, 1, 1, 0, 1, 0, 1, 0,
		0, 0, 1, 1, 0, 0, 0, 0, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 0, 0, 1,
	};
	//vector<int> s = {
	//	1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0,
	//};
	//vector<int> s = {
	//	1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0,
	//};
#endif
	auto p = BerlekampMassey(s);
	
	int order = p.size() - 1;
	cout << order << " : ";
	for (auto b : p) {
		cout << b << ' ';
	}
	cout << endl;
	
	vector<int> test = s; //{ 1,1,1,0,0,0};
	test.resize(p.size() - 1);
	//print(test);
	generate_seq(test, p, s.size() - p.size() + 1);
	//print(test);
	//print(s);
	assert(s == test);

	int c = gen_constant(p);
	cout << "cnst = " << std::hex << c << std::dec << endl;
	analyse(order, c);
	auto test2 = gen_seq(c, 1, 3 * (1 << order));
	//print(test2);
	cout << "period = " << calcPeriod(test2) << endl;

	auto it = std::search(test2.begin(), test2.end(), s.begin(), s.end());
	if (it == test2.end()) {
		cout << "NOT found" << endl;
	} else {
		cout << "found at offset " << std::distance(test2.begin(), it) << endl;
	}

}
