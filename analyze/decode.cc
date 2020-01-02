#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <map>
#include <vector>
#include <iostream>

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

int median(vector<int> v)
{
	auto mid = begin(v) + v.size() / 2;
	std::nth_element(begin(v), mid, end(v));
	return *mid;
}

// also called 'mode'
int most_frequent(const vector<int>& v)
{
	std::map<int, int> m;
	for (auto& e : v) m[e]++;
	auto it = std::max_element(begin(m), end(m),
		[](auto& p, auto& q) { return p.second < q.second; });
	return it->first;
}

template <typename T>
T sqr(T t)
{
	return t * t;
}

template<typename T>
bool contains(const std::vector<T>& v, const T& t)
{
	return std::find(std::begin(v), std::end(v), t) != std::end(v);
}

int wrap72(int x)
{
	return (x + 72) % 72;
}

vector<int> select_channel(const vector<int>& v, int num_channels)
{
	//auto mid = median(v);
	auto mid = most_frequent(v);
	//cout << mid << endl;

	int b = 0;
	std::pair<uint64_t, int> energy[72];
	for (int i = 0; i < 72; ++i) {
		energy[i].first = 0;
		energy[i].second = i;
	}
	for (auto& e : v) {
		energy[b].first += sqr(int64_t(e) - mid);
		++b; if (b == 72) b = 0;
	}
	//b = 0;
	//for (auto& e : energy) {
	//	cout << b << ' ' << e << endl;
	//	++b;
	//}
	
	//int s = std::max_element(energy, energy + 72) - energy;
	//cout << s << endl;
	std::sort(energy, energy + 72,
	          [](auto& p1, auto& p2) { return p1.first > p2.first; });
	//for (auto& e : energy) {
	//	cout << e.first << ' ' << e.second << '\n';
	//}
	int ofst = wrap72(energy[0].second - 2);
	vector<int> selected;
	int s = 0;
	for (int i = 0; i < num_channels; ++i) {
		while (true) {
			int c = energy[s++].second; // round down
			if (std::find_if(begin(selected), end(selected),
			           [&](int s) { return (wrap72(c - ofst) / 4) == (wrap72(s - ofst) / 4); }) ==
			    end(selected)) {
				selected.push_back(c);
				break;
			}
		}
	}
	for (auto& s : selected) cout << s << '\n';

	vector<int> result;
	b = 0;
	for (auto& e : v) {
		if (contains(selected, b)) result.push_back(e);
		++b; if (b == 72) b = 0;
	}
	return result;
}


int main(int argc, char** argv)
{
	if (argc != 4) {
		printf("Usage %s: <bins> <raw> <out>\n", argv[0]);
		exit(1);
	}

	int num_channels = 1;

	auto bins = load(argv[1]);
	auto raw  = load(argv[2]);
	raw = select_channel(raw, num_channels);
	//raw = average(raw, 1);
	//printf("%zd %zd\n", bins.size(), raw.size());

	assert(is_sorted(bins.begin(), bins.end()));

	FILE* outF = fopen(argv[3], "w");
	int c = 0;
	auto n = raw.size() - (raw.size() % num_channels);
	for (size_t i = 0; i < n; ++i) {
		auto r = raw[i];
		//printf("%d -> %d\n", r, lookup(bins, r));
		fprintf(outF, "%d ",       lookup(bins, r)); // melodic
		//fprintf(outF, "%d ", 511 - lookup(bins, r)); // rhythm
		if (++c == num_channels) {
			c = 0;
			fprintf(outF, "\n");
		}
	}
	fclose(outF);
}
