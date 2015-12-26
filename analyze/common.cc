#include "common.hh"
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <cstdio>
#include <cassert>
#include <memory>
#include <deque>
#include <iostream>

using namespace std;

Patch getSinePatch()
{
	Patch sine;
	sine.mod.AM =  0;
	sine.mod.PM =  0;
	sine.mod.EG =  1;
	sine.mod.KR =  0;
	sine.mod.ML =  0;
	sine.mod.KL =  0;
	sine.mod.TL = 63;
	sine.mod.WF =  0;
	sine.mod.FB =  0;
	sine.mod.AR =  0; // 15
	sine.mod.DR = 15;
	sine.mod.SL =  0;
	sine.mod.RR = 15;
	sine.car.AM =  0;
	sine.car.PM =  0;
	sine.car.EG =  1;
	sine.car.KR =  0;
	sine.car.ML =  0;
	sine.car.KL =  0;
	sine.car.WF =  0;
	sine.car.AR = 15;
	sine.car.DR = 15;
	sine.car.SL =  0;
	sine.car.RR = 15;
	return sine;
}

void convert(const Patch& patch, uint8_t regs[8])
{
	regs[0] = (patch.mod.AM &  1) << 7
	        | (patch.mod.PM &  1) << 6
	        | (patch.mod.EG &  1) << 5
	        | (patch.mod.KR &  1) << 4
	        | (patch.mod.ML & 15) << 0;
	regs[1] = (patch.car.AM &  1) << 7
	        | (patch.car.PM &  1) << 6
	        | (patch.car.EG &  1) << 5
	        | (patch.car.KR &  1) << 4
	        | (patch.car.ML & 15) << 0;
	regs[2] = (patch.mod.KL &  3) << 6
	        | (patch.mod.TL & 63) << 0;
	regs[3] = (patch.car.KL &  3) << 6
	        | ( /*not used*/   0) << 5
	        | (patch.car.WF &  1) << 4
	        | (patch.mod.WF &  1) << 3
	        | (patch.mod.FB &  7) << 0;
	regs[4] = (patch.mod.AR & 15) << 4
	        | (patch.mod.DR & 15) << 0;
	regs[5] = (patch.car.AR & 15) << 4
	        | (patch.car.DR & 15) << 0;
	regs[6] = (patch.mod.SL & 15) << 4
	        | (patch.mod.RR & 15) << 0;
	regs[7] = (patch.car.SL & 15) << 4
	        | (patch.car.RR & 15) << 0;
}


void writeRegister(uint8_t reg, uint8_t val, Script& script)
{
	// bit 11  ~IC
	// bit 10  ~CS
	// bit  9  ~WE
	// bit  8  A0
	// bit 7-0 D7-D0
	script.push_back(0xC00 | reg); script.push_back(1);      // WE
	script.push_back(0x800 | reg); script.push_back(1);      // WE CS 
	script.push_back(0xE00 | reg); script.push_back(12 - 2); //
	script.push_back(0xD00 | val); script.push_back(1);      // WE A0
	script.push_back(0x900 | val); script.push_back(1);      // WE CS A0 
	script.push_back(0xF00 | val); script.push_back(84 - 2); // A0
}
void writeRegisterDelay(uint8_t reg, uint8_t val, int delay, Script& script)
{
	int d = std::max(1, delay - 95);
	while (d > 65535) {
		script.push_back(0xC00 | reg); script.push_back(65535); // WE
		d -= 65535;
	}
	script.push_back(0xC00 | reg); script.push_back(d);      // WE
	script.push_back(0x800 | reg); script.push_back(1);      // WE CS 
	script.push_back(0xE00 | reg); script.push_back(12 - 2); //
	script.push_back(0xD00 | val); script.push_back(1);      // WE A0
	script.push_back(0x900 | val); script.push_back(1);      // WE CS A0 
	script.push_back(0xF00 | val); script.push_back(84 - 2); // A0
}

void setInstrument(const Patch& patch, Script& script)
{
	uint8_t regs[8];
	convert(patch, regs);
	for (int i = 0; i < 8; ++i) {
		writeRegister(i, regs[i], script);
	}
}

void addReset(Script& script)
{
	script.push_back(0x700); script.push_back(80); // IC active for at least 80 cycles
	script.push_back(0xF00); script.push_back(1); // deactivate IC
}
void endScript(Script& script)
{
	script.push_back(0xF00); script.push_back(0);
}


void printPatch(const Patch& p)
{
	printf("mod={AM=%d PM=%d EG=%d KR=%d ML=%02d KL=%d TL=%02d WF=%d FB=%d AR=%02d DR=%02d SL=%02d RR=%02d}\n"
	       "car={AM=%d PM=%d EG=%d KR=%d ML=%02d KL=%d       WF=%d      AR=%02d DR=%02d SL=%02d RR=%02d}\n",
	       p.mod.AM, p.mod.PM, p.mod.EG, p.mod.KR, p.mod.ML, p.mod.KL, p.mod.TL,
	       p.mod.WF, p.mod.FB, p.mod.AR, p.mod.DR, p.mod.SL, p.mod.RR,
	       p.car.AM, p.car.PM, p.car.EG, p.car.KR, p.car.ML, p.car.KL,
	       p.car.WF,           p.car.AR, p.car.DR, p.car.SL, p.car.RR);
}

void printRegisters(const uint8_t regs[8])
{
	for (int i = 0; i < 8; ++i) {
		printf("%02X ", regs[i]);
	}
	printf("\n");
}

void printScript(const Script& script)
{
	assert((script.size() % 2) == 0);
	for (size_t i = 0; i < script.size(); i += 2) {
		printf("%03X %2d\n", script[i + 0] , script[i + 1]);
	}
}





int fd;
struct pollfd fds[1];

vector<uint8_t> send;
uint8_t wait[256] = {};

bool capture = false;
vector<uint16_t> capData;


void setPending(uint8_t w)
{
	wait[w] = true;
}
void resetPending(uint8_t w)
{
	wait[w] = false;
}
bool isPending(uint8_t w)
{
	return wait[w];
}

const vector<uint16_t>& getCapturedData()
{
	return capData;
}

void handleRead(uint16_t data)
{
	if ((data >> 8) == 0x80) {
		uint8_t d = data & 0xFF;
		//printf("--%d-- ", d);
		resetPending(d);
	} else if (capture) {
		capData.push_back(data + 0x8000);
	}
}

void handleRead()
{
	static bool even = true; // even/odd
	static uint8_t half = 0; // half of a 16-bit value

	//printf("I");
	while (true) {
		uint8_t buf[4096];
		auto r = read(fd, buf, sizeof(buf));
		if (r == -1) {
			if (errno == EAGAIN) break;
			assert(false);
		}
		//printf("%d ", int(r));
		for (decltype(r) i = 0; i < r; ++i) {
			if (even) {
				half = buf[i];
			} else {
				handleRead(buf[i] << 8 | half);
			}
			even = !even;
		}
	}
}

void handleWrite()
{
	while (!send.empty()) {
		//printf("O");
		auto r = write(fd, send.data(), send.size());
		if (r == -1) {
			printf("write error");
		}
		send.erase(send.begin(), send.begin() + r);
		//send.clear();
	}
}

void pollIO()
{
	fflush(stdout);
	int p = poll(fds, 1, 100);
	if (p == 0) {
		//printf(".");
		return;
	}
	if (fds[0].revents & POLLIN) {
		handleRead();
	}
	if (fds[0].revents & POLLOUT) {
		handleWrite();
	}
}

void executeScript(const Script& script)
{
	send.push_back('R');
	for (uint16_t s : script) {
		send.push_back(s & 0xFF);
		send.push_back(s  >> 8);
	}
}

void stopCapture()
{
	send.push_back('s');
	capture = false;
}

void startCapture()
{
	send.push_back('S');
	capture = true;
	capData.clear();
}

void toggleLedA()
{
	send.push_back('A');
}
void toggleLedB()
{
	send.push_back('B');
}
void toggleLedC()
{
	send.push_back('C');
}

void echo(uint8_t e)
{
	send.push_back('E');
	send.push_back(e);
}

void init(const char* dev)
{
	fd = open(dev, O_RDWR | O_NONBLOCK);
	if (fd == -1) {
		printf("Error opening %s\n", dev);
		exit(1);
	}
	fds[0].fd = fd;
	fds[0].events = POLLIN | POLLOUT;
	fds[0].revents = 0;
}
