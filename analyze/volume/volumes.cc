#include "common.hh"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>

using namespace std;

auto createScript(uint8_t volume)
{
	assert(volume < 16);
	Script script;
	setInstrument(getSinePatch(), script);
	writeRegister(0x10, 0x61, script);
	writeRegister(0x30, volume, script);
	writeRegister(0x20, 0x12, script);
	endScript(script);
	return script;
}



int main(int argc, char** argv)
{
	size_t required = 20000;
	if (argc >= 2) {
		required = atol(argv[1]);
	}

	string prefix = "volume";
	if (argc >= 3) {
		prefix = argv[2];
	}

	string dev = "/dev/ttyACM0";
	if (argc >= 4) {
		dev = argv[3];
	}

	init(dev.c_str());


	for (int vol = 0; vol < 16; ++vol) {
		printf("volume=%d ...\n", vol);
		stopCapture();

		auto script = createScript(vol);
		executeScript(script);
		setPending(0xF3); // stop capture
		setPending(0xD2); // start script
		setPending(0xF2); // stop script

		while (isPending(0xF3)) pollIO();
		while (isPending(0xD2)) pollIO();
		while (isPending(0xF2)) pollIO();

		startCapture();
		while (1) {
			auto s = getCapturedData().size();
			if (s >= required) break;
			printf("\rcapturing data ... %zd/%zd", s, required);
			pollIO();
		}

		string base = prefix + std::to_string(vol);
		string raw = base + ".raw";
		FILE* rawF = fopen(raw.c_str(), "w");
		const auto& buf = getCapturedData();
		for (auto& b : buf) {
			fprintf(rawF, "%d ", b);
		}
		fclose(rawF);
	}
}
