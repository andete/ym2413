#ifndef BOARD_HH
#define BOARD_HH

#include "ym.hh"

// write /IC (false->active, true->not active)
inline void setIC(bool ic)
{
	if (ic) {
		ym::high(ym::IC);
	} else {
		ym::low (ym::IC);
	}
}

// write /CS (false->active, true->not active)
inline void setCS(bool cs)
{
	if (cs) {
		ym::high(ym::CS);
	} else {
		ym::low (ym::CS);
	}
}

// write /CS (false->active, true->not active)
inline void setWE(bool we)
{
	if (we) {
		ym::high(ym::WE);
	} else {
		ym::low (ym::WE);
	}
}

// write XOUT/XIN
//  true  -> positive edge
//  false -> negative edge
inline void setClk(bool clk)
{
	// TODO should we write XOUT or XIN, or both?
	if (clk) {
		ym::high(ym::XOUT);
	} else {
		ym::low (ym::XOUT);
	}
}

// write D7-D0
inline void setData(int data)
{
	// TODO write 8 bits at-a-time
	for (int i = 0; i < 8; ++i) {
		if (data & (1 << i)) {
			ym::high(ym::D0 + i);
		} else {
			ym::low (ym::D0 + i);
		}
	}
}

// write A0
inline void setAddr(int addr)
{
	if (addr) {
		ym::high(ym::A0);
	} else {
		ym::low (ym::A0);
	}
}

#endif
