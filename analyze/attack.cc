#include <cstdio>
#include <iostream>
#include <string>

using namespace std;

template<typename T, size_t N>
constexpr size_t countof(T(&)[N])
{
	return N;
}

string binary(int x, int y)
{
	string result = "xxxxxxx";
	int m = 64;
	for (int i = 0; i < 7; ++i) {
		if ((x & m) != (y & m)) break;
		result[i] = (x & m) ? '1' : '0';
		m >>= 1;
	}
	return result;
}
string binary(int x) { return binary(x, x); }

int main()
{
	int m[][2] = {
		{127, 97},
		{124, 94},
		{ 97, 73},
		{ 94, 71},
		{ 73, 55},
		{ 71, 53},
		{ 57, 42}, // C
		{ 55, 41},
		{ 53, 39},
		{ 42, 30}, // C
		{ 41, 29},
		{ 39, 28},
		{ 30, 22}, // C
		{ 29, 21},
		{ 28, 20},
		{ 22, 14}, // C
		{ 21, 14},
		{ 20, 13},
		{ 14, 10},
		{ 13,  9},
		{ 10,  6},
		{  9,  5},
		{  6,  2},
		{  5,  1},
	//      {  1,  0},
	//      57, 42, 30, 22, 14, 10, 6, 2, 0
	};

	struct S {
		int low;
		int high;
	} s[128];

	for (int i = 0; i < 128; ++i) {
		s[i].low  = 0;
		s[i].high = 127;
	}

	for (size_t i = 0; i < countof(m); ++i) {
		int from = m[i][0];
		int to   = m[i][1];
		s[from].low  = to;
		s[from].high = to;
	}

	if (1) {
	int l = s[0].low;
	for (int i = 0; i < 128; ++i) {
		l = max(l, s[i].low);
		s[i].low = l;

	}
	int h = s[127].high;
	for (int i = 127; i >= 0; --i) {
		h = min(h, s[i].high);
		s[i].high = h;

	}
	}

	if (1) {
	int l2 = s[127].low;
	for (int i = 127; i >= 0; --i) {
		l2 = max(l2 - 1, s[i].low);
		s[i].low = l2;

	}
	int h2 = s[0].high;
	for (int i = 0; i < 128; ++i) {
		h2 = min(h2 + 1, s[i].high);
		s[i].high = h2;

	}
	}

	if (1) {
	for (int i = 10; i < 128; ++i) {
		int l = s[i - 1].low;
		if (l == s[i - 2].low) {
			l += 1;
		}
		s[i].low = max(s[i].low, l);
	}
	for (int i = 125; i >= 10; --i) {
		int h = s[i + 1].high;
		if (h == s[i + 2].high) {
			h -= 1;
		}
		s[i].high = min(s[i].high, h);
	}
	}

	if (0) {
	int l9 = s[0].low;
	int h9 = s[0].high;
	for (int i = 0; i < 128; ++i) {
		cout << s[i].low  << ' '
		     << s[i].high << ' '
	//	     << s[i].low  - l9 << ' '
	//	     << s[i].high - h9 << ' '
	//	     << i << ' '
	             << endl;
		l9 = s[i].low;
		h9 = s[i].high;
	}
	}

	s[0].low = 0;
	s[0].high = 127;

	if (0) {
	for (int i = 0; i < 128; ++i) {
		cout << binary(s[i].low ) << ' '
		     << binary(s[i].high) << ' '
		     << binary(s[i].low, s[i].high) << endl;
	}
	}
	
	if (1) {
	for (int j = 0; j < 7; ++j) {
		cout << "qm" << j << " = QM(['b0','b1', 'b2', 'b3', 'b4', 'b5', 'b6'])\n";
		cout << "print qm" << j << ".get_function(qm" << j << ".solve([";
		for (int i = 0; i < 128; ++i) {
			string b = binary(s[i].low, s[i].high);
			if (b[6-j] == '1') cout << i << ',';
		}
		cout << "],[";
		for (int i = 0; i < 128; ++i) {
			string b = binary(s[i].low, s[i].high);
			if (b[6-j] == 'x') cout << i << ',';
		}
		cout << "])[1])\n";
	}
	}

	if (0) {
		cout << "7\n7\n128\n\nA\nB\nC\nD\nE\nF\nG\nf0\nf1\nf2\nf3\nf4\nf5\nf6\n\n";
		for (int i = 0; i < 128; ++i) {
			cout << binary(i) << binary(s[i].low, s[i].high) << '\n';
		}
	}

	/*
	for (int i = 0; i < countof(m); ++i) {
		int a = m[i][0];
		int b = m[i][1];
		int d = a - b;
		int n1 = (-a)  / 4;
		int n2 = (-a) >> 2;
		int n3 = (~a)  / 4;
		int n4 = (~a) >> 2;
		int n5 = (~(a|7)) >> 2;
		int n6 = (~(a&~7)) >> 2;
		int n7 = (a&~7) >> 2;
		printf("%3d->%3d  d=%2d  %3d(%2d) %3d(%2d) %3d(%2d) %3d(%2d) %3d(%2d) %3d(%2d) %3d(%2d)\n",
		       a, b, d, n1, d+n1, n2, d+n2, n3, d+n3, n4, d+n4, n5, d+n5, n6, d+n6, n7, d-n7);
	}
	*/



	int x = 124;
	cout << x << endl;
	do {
		x += ~x >> 2;
		//x += ~(x-1) >> 2;
		cout << x << endl;
	} while (x != 1);

	/*
	for (int i = 127; i > 0; --i) {
		cout << i << " -> " << (i + (~i >> 2)) << endl;
	}
	*/
}

// expected sequence:
// 112-127 -> 91- 95 -> 71-72 -> 53 -> 39 -> 28 -> 20 -> 13 ->  9 -> 5 -> 1 -> 0
//            96-102 -> 73-74 -> 55 -> 41 -> 29 -> 21 -> 14 -> 10 -> 6 -> 2 -> (0)
//            96-102 -> 75-77 -> 57 -> 42 -> 30 -> 22 -> 14 -> 10 -> 6 -> 2 -> 1
//
// measured levels:
//  124 -> 94 -> 71 -> 53 -> 39 -> 28 -> 20 -> 13 ->  9 -> 5 -> 1 -> 0
//  xxx -> 97 -> 73 -> 55 -> 41 -> 29 -> 21 -> 14 -> 10 -> 6 -> 2 -> 1/0




