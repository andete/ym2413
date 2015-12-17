#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <fstream>
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

int lookup(const vector<int>& bins, int value)
{
	return lower_bound(bins.begin(), bins.end(), value) - bins.begin();
}

vector<int> average(const vector<int>& data, size_t factor)
{
	assert((data.size() % factor) == 0);
	vector<int> result;
	for (size_t i = 0; i < data.size(); i += factor) {
		size_t sum = 0;
		for (size_t j = 0; j < factor; ++j) {
			sum += data[i + j];
		}
		result.push_back(sum / factor);
	}
	return result;
}

int main(int argc, char** argv)
{
	if (argc != 4) {
		printf("Usage %s: <bins> <raw> <out>\n", argv[0]);
		exit(1);
	}

	auto bins = load(argv[1]);
	auto raw  = load(argv[2]);
	raw = average(raw, 512);
	//printf("%zd %zd\n", bins.size(), raw.size());

	assert(is_sorted(bins.begin(), bins.end()));

	FILE* outF = fopen(argv[3], "w");
	for (auto& r : raw) {
		//printf("%d -> %d\n", r, lookup(bins, r));
		fprintf(outF, "%d ", lookup(bins, r));
	}
	fclose(outF);
}
