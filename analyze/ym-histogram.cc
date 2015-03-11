#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

vector<int> load(const char* file)
{
	vector<int> result;
	ifstream is(file);
	while (1) {
		int i;
		is >> i;
		if (!is.good()) break;
		result.push_back(i);
	}
	return result;
}

int main(int argc, char** argv)
{
	if (argc != 2) {
		printf("Usage %s: <ym-data>\n", argv[0]);
		exit(1);
	}

	auto data = load(argv[1]);

	vector<int> histogram(512, 0);
	int min = 512;
	int max = -1;
	for (auto& i : data) {
		assert(i >= 0);
		assert(i < 512);
		++histogram[i];
		min = std::min(min, i);
		max = std::max(max, i);
	}
	for (int i = 0; i < 512; ++i) {
		cout << i << '\t' << histogram[i] << endl;
	}
	cout << "count = " << data.size() << endl;
	cout << "min = " << min << endl;
	cout << "max = " << max << endl;

}
