// write /IC (false->active, true->not active)
static inline void setIC(bool ic)
{
	if (ic) {
		ym::high(YM_IC);
	} else {
		ym::low (YM_IC);
	}
}

// write /CS (false->active, true->not active)
static inline void setCS(bool cs)
{
	if (cs) {
		ym::high(YM_CS);
	} else {
		ym::low (YM_CS);
	}
}

// write /CS (false->active, true->not active)
static inline void setWE(bool we)
{
	if (we) {
		ym::high(YM_WE);
	} else {
		ym::low (YM_WE);
	}
}

// write XOUT/XIN
//  true  -> positive edge
//  false -> negative edge
static inline void setClk(bool clk)
{
	// TODO should we write XOUT or XIN, or both?
	if (clk) {
		ym::high(YM_XOUT);
	} else {
		ym::low (YM_XOUT);
	}
}

// write D7-D0
static inline void setData(int data)
{
	// TODO write 8 bits at-a-time
	for (int i = 0; i < 8; ++i) {
		if (data & (1 << i)) {
			ym::high(YM_D0 + i);
		} else {
			ym::low (YM_D0 + i);
		}
	}
}

// write A0
static inline void setAddr(int addr)
{
	if (addr) {
		ym::high(YM_A0);
	} else {
		ym::low (YM_A0);
	}
}

short sampleMO()
{
	return 0; // TODO
}

short sampleRO()
{
	return 0; // TODO
}
