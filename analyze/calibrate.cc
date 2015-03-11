#include "common.hh"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>

using namespace std;

auto createScript()
{
	Script script;
	//addReset(script);
	setInstrument(getSinePatch(), script);
	writeRegister(0x10, 0x61, script);
	writeRegister(0x30, 0x00, script);
	writeRegister(0x20, 0x12, script);
	endScript(script);
	return script;
}

template<typename T>
T get(const vector<T>& buf, int i)
{
	if (i < 0) return T(0);
	if (i >= int(buf.size())) return T(0);
	return buf[i];
}

// Given the y-coordinates of 3 points, x-coorcoordinates are assumed to be
// respectively -1, 0, 1. Fit a parabola through these points. And return the
// x-coordinate of the vertex (= maxima/minima) of the parabola.
template<typename T>
double quadraticPeak(T p, T q, T r)
{
	auto n = 2 * p + 2 * r - 4 * q;
	if (n == 0) return 0.0;
	return double(p - r) / n;
}

// Calculate the histogram for a given signal. That signal may only contain
// values in the range [0, n). The result is vector<int> of size n.
template<typename T>
vector<int> calcHistogram(const vector<T>& data, int n)
{
	vector<int> result(n); // zero-filled
	for (auto& x : data) {
		assert(0 <= x); assert(x < n);
		++result[x];
	}
	return result;
}

vector<double> filter(const vector<int>& data, int width)
{
	vector<double> result;
	result.reserve(data.size());
	int t = 0;
	for (int j = -width; j <= width; ++j) {
		t += width + 1 - abs(j);
	}
	for (int i = 0; i < int(data.size()); ++i) {
		int s = 0;
		for (int j = -width; j <= width; ++j) {
			int w = width + 1 - abs(j);
			s += w * get(data, i + j);
		}
		result.push_back(double(s) / t);
	}
	return result;
}

// Return vector of indices where the input data has local maxima.
template<typename T>
vector<double> detectPeaks(const vector<T>& data)
{
	vector<double> result;
	for (size_t i = 0; i < data.size(); ++i) {
		// We need a minimum threshold.
		if (data[i] < 1) continue;
		// Continue as long as the signal is rising.
		while (data[i + 1] >= data[i]) ++i;
		// Found a local maximum.
		double peak = i;
		// Adjust position by fitting a parabola.
		auto left  = data[i - 1];
		auto mid   = data[i    ];
		auto right = data[i + 1];
		peak += quadraticPeak(left, mid, right);
		result.push_back(peak);
		// Typically peaks will be +/- 6 units apart, so take alreay 3 steps
		i += 2*16;
		// Go to local minima before looking for maxima again.
		while (data[i + 1] <= data[i]) ++i;
	}
	return result;
}

vector<double> insertMissing(const vector<double>& x)
{
	vector<double> result;
	result.push_back(x[0]);
	for (size_t i = 1; i < x.size(); ++i) {
		double d = x[i] - x[i - 1];
		if (d > 10.0 * 16) {
			result.push_back((x[i - 1] + x[i]) / 2.0);
		}
		result.push_back(x[i]);
	}
	return result;
}

vector<int> intBoundaries(const vector<double>& peaks)
{
	vector<int> result;
	auto n = peaks.size();
	result.reserve(n);
	for (size_t i = 0; i < (n - 1); ++i) {
		double h = (peaks[i + 1] + peaks[i]) / 2.0;
		result.push_back(int(h));
	}
	result.push_back(65536);
	return result;
}

vector<int> createBins(const vector<uint16_t>& buf, int width, const string& prefix)
{
	assert(width > 1);

	string raw = prefix + ".raw";
	FILE* rawF = fopen(raw.c_str(), "w");
	for (auto& b : buf) {
		fprintf(rawF, "%d ", b);
	}
	fclose(rawF);

	auto histogram = calcHistogram(buf, 0x10000);
	string hist = prefix + ".histogram";
	FILE* histF = fopen(hist.c_str(), "w");
	for (auto& h : histogram) {
		fprintf(histF, "%d ", h);
	}
	fclose(histF);

	auto filtered = filter(histogram, width);
	string filt = prefix + ".filtered";
	FILE* filtF = fopen(filt.c_str(), "w");
	for (auto& f : filtered) {
		fprintf(filtF, "%f ", f);
	}
	fclose(filtF);

	auto peaks = detectPeaks(filtered);
	assert(!peaks.empty());
	auto peaks2 = insertMissing(peaks);
	string pks = prefix + ".peaks";
	FILE* peakF = fopen(pks.c_str(), "w");
	for (auto& p : peaks2) {
		fprintf(peakF, "%f ", p);
	}
	fclose(peakF);

	ofstream m(prefix + ".m");
	m << "r = load(\"" + prefix + ".raw\");\n"
	     "h = load(\"" + prefix + ".histogram\");\n"
	     "f = load(\"" + prefix + ".filtered\");\n"
	     "p = load(\"" + prefix + ".peaks\");\n"
	     "\n"
	     "plot(h,\"-\", f,\"-\",\"LineWidth\",1, p+1,f(round(p+1)),\"o\",\"LineWidth,\",2);\n";

	return intBoundaries(peaks2);
}

int main(int argc, char** argv)
{
	size_t required = 60000;
	if (argc >= 2) {
		required = atol(argv[1]);
	}

	string prefix = "calibrate";
	if (argc >= 3) {
		prefix = argv[2];
	}

	string dev = "/dev/ttyACM0";
	if (argc >= 4) {
		dev = argv[3];
	}

	init(dev.c_str());

	stopCapture();
	toggleLedA();
	echo(123);

	auto script = createScript();
	executeScript(script);
	setPending(0xD2);
	setPending(0xF2);

	printf("initializing ...\n");
	while (isPending(0xD2)) pollIO();
	while (isPending(0xF2)) pollIO();

	startCapture();
	while (1) {
		auto s = getCapturedData().size();
		if (s >= required) break;
		printf("\rcapturing data ... %zd/%zd", s, required);
		pollIO();
	}

	printf("\nanalyzing ...\n");
	auto bins = createBins(getCapturedData(), 30, prefix);
	printf(" #peaks = %d\n", int(bins.size()));
	if (bins.size() != 512) {
		printf("  Error: expected 512 peaks");
	}
	string name = prefix + ".bins";
	FILE* binsF = fopen(name.c_str(), "w");
	for (auto& b : bins) {
		fprintf(binsF, "%d ", b);
	}
	fclose(binsF);
}
