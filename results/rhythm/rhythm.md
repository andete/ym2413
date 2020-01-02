# YM2413 Rhythm mode

Progress is slow, but I have not abandoned this project.
In this post we'll look at the YM2413 percussion sounds.

## Introduction

Most info in this introduction can be found in or deduced from the YM2413 application manual. So feel free to skip this introduction.

The YM2413 can operate in two modes, selected via bit 5 in register 0x0E.
- Melodic mode (bit5 = 0).
- Rhythm mode (bit5 = 1).

In melodic mode there are 9 melody channels. In rhythm mode the 3 last melody channels are replaced with 5 (fixed) percussion sounds. More specifically these percussion sounds are:
- Bass drum (BD)
- Snare drum (SD)
- Tom tom (TOM)
- Top cymbal (TCY)
- High hat (HH)

In melodic mode each channel is controlled by 3 registers (0x1?, 0x2?, 0x3? with '?' 0-8). These select the frequency, volume, instrument and key-on/off. In rhythm mode the same set of 3x3 registers now controls the properties of 5 percussion sounds. Moreover the YM2413 application manual has a set of recommended (fixed) values that should be written to some of these registers:

| register | value  |
|:--------:|:------:|
| 0x16     | 0x20   |
| 0x17     | 0x50   |
| 0x18     | 0xC0   |
| 0x26     | 0x05   |
| 0x27     | 0x05   |
| 0x28     | 0x01   |

So this only leaves registers 0x36-0x38 to control the percussion sounds.

These 'fixed' registers normally control the frequency and the key-on/off behavior of the melodic channels. It turns out that in rhythm mode these still control the frequency (of now 5 percussion sounds, more details on this below). But normally changing the frequency of a percussion sound is not useful, hence the table of (fixed) recommended settings. However it is important to control the on/off behavior of the percussion sounds, but there are only 3 key-on/off bits in the registers 0x26-0x28. Instead in rhythm mode the key on/off bits are accessible via register 0x0E:

| b7 | b6 | b5 | b4 | b3 | b2 | b1 | b0 |
|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|
|  - |  - |mode| BD | SD | TOM| TCY| HH |

Detail: it turns out that in rhythm mode _both_ the on/off bits in register 0x0E and the bits in registers 0x26-0x28 are taken into account. It's as-if the 'corresponding' bits are OR'ed together. _TODO investigate this further_.

In melodic mode registers 0x36-0x38 control the volume and the instrument selection. In rhythm mode we need to control 5 (instead of 3) volumes and there's no need to select an instrument. Therefor the function of these registers is slightly re-assigned in rhythm mode:

| volume | b7-b4 | b3-b0 |
|:------:|:-----:|:-----:|
|  0x36  |   -   |  BD   |
|  0x37  |  HH   |  SD   |
|  0x38  |  TOM  |  TCY  |

A YM2413 channel consists out of two operators. In melodic mode these two operators acts like a carrier and a modulator. Thus in rhythm mode there are 6 operators available to generate 5 percussion sounds. The operators are assigned like this:

| Instrument | operator(s) |
|:----------:|:-----------:|
|     BD     | mod6 + car6 |
|     SD     |    car7     |
|     TOM    |    mod8     |
|     TCY    |    car8     |
|     HH     |    mod7     |

Notation: _mod6_ refers to the operator that in melodic mode is used as the modulator for channel 6 (with channels numbered from 0 to 8).
For the cases where a percussion sound only has a single operator, that operator always acts like a carrier.


Section "3-4 Operator (OP) and DAC" of the YM2413 application manual states: "Since the level of percussion sounds seems lower, when compared to music sounds, the same percussion sounds are output twice". And this is shown in figure III-3. In table form that figure looks like this:

|     slot     |  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 |
|:------------:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|
|operator      |mod0|mod1|mod2|car0|car1|car2|mod3|mod4|mod5|car3|car4|car5|mod6|mod7|mod8|car6|car7|car8|
|melodic-output|  - |  - |  - | ch0| ch1| ch2|  - |  - |  - | ch3| ch4| ch5|  - |  - |  - | ch6| ch7| ch8|
|rhythm-output | HH2|TOM2| BD2| ch0| ch1| ch2| SD2|TCY2|  - | ch3| ch4| ch5|  - | HH | TOM| BD | SD | TCY|

In this table a _slot_ corresponds to a _time-slot_. Each slot is outputted in sequence over time in a cyclic way. Each slot lasts for 4 clock cycles. Thus when the YM2413 is clocked at the recommended frequency of 3.57MHz, each channel is visited at a rate of 3.57MHz / (4 * 18) = 49716Hz.

(After low-pass filtering) the effect of outputting a sound twice is the same as outputting it only once but with twice the amplitude, thus increasing the volume by 6dB.

I verified that the 2nd output of a percussion sound is _always_ identical to the first output (even when in between some parameter values have been changed). I also verified the positions of the original and the repeated percussion outputs. This output is quite logical if you compare how the (single) hardware operator is reused over time for mod/car-0/8 and how the percussion sounds are mapped to the logical operators.

So the YM2413 outputs percussion sounds twice. There are two main options for how this can be implemented:
- Recompute that output.
- Remember the previous output.

The YM2413 hardware cannot easily recompute an output (because the single shared operator hardware is occupied and because the required input data is stored in shift registers and is only accessible at specific moments in time). That leaves the second option, but that requires memory. And indeed when I look at the [YM2413 die shot](http://siliconpr0n.org/map/yamaha/fhb013/mz_ns50xu/#x=583&y=989&z=5), I do find a set of 9 shift registers of length 5. This is enough to store the 9-bit output of the 5 percussion sounds. I marked these shift registers as 'RHYTHM-DELAY' in orange in [this picture](die-shot.jpg). Also it's physically located close to the DAC, so that makes sense.



## Percussion waveforms

Most of the above information could be derived from the YM2413 application manual (or sometimes from my previous reverse engineering work). In this section we'll look in more detail at how the percussion sounds are actually generated.

I often used existing YM2413 emulators (written by Burczynski and Okazaki) as a starting point (the versions that are currently used in openMSX). But I can already reveal that these emulators were not always 100% correct.

I did not reverse engineer the percussion sounds in the same order as they are listed in the YM2413 application manual. Instead I ordered them from, what I thought, easier to more difficult.

### Base Drum (BD)

This sound uses two operators (mod6 + car6). The YM2413 software emulators generate this sound mostly the same as regular melodic sound channels. If this is correct then all we need to do is figure out the _instrument settings_.

Or in other words: if this is indeed how BD generation works, then it should be possible to perfectly replicate the BD waveform via the custom instrument settings. I verified that the following settings come very close to the real BD sound. This table shows the values for registers 0-7 that define the custom instrument:

|   0  |   1  |   2  |   3  |   4  |   5  |   6  |   7  |
|:----:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|
| 0x01 | 0x01 | 0x18 | 0x0F | 0xDF | 0xF8 | 0x6A | 0x6D |

I did an experiment where I used these settings for the custom instrument and then played 'simultaneously' the BD sound and custom sound (detail: I used fnum=0x1ff and block=0, this is lower than the recommended frequency). [This image](bd-custom.png) shows the result, BD in orange, custom instrument in blue. There's a pretty good match between the two. But if we [zoom in](bd-custom-zoom.png) near the start we also see some differences:
- The custom instrument seems 1 sample delayed compared to the BD sound.
- The custom instrument has a slightly higher peak amplitude.

The first point (1 sample delay) is fully expected: in my test program I write register 0x0E (set BD key-on) immediately followed by a write to register 0x25 (key-on custom instrument on channel 5). According to the YM2413 spec, writing a register requires a minimum of 84 cycles. That's longer than the duration of 1 sample (72 cycles). I experimented with using (invalid) faster register write timings, I tried swapping the order of the two register writes, I tried using different channels for the custom instrument, ... but so far all unsuccessful, I always have this 1 sample delay.

The second point (different peak amplitude) could be a consequence of the first. From previous reverse engineering work we know that the envelope is controlled by a _global counter_ (thus shared between these two channels). Shifting the signal w.r.t. this counter influences the exact moment when the envelope transitions from e.g. attack to decay. And e.g. a one sample longer attack phase can have these kind of effects on the amplitude of the signal (especially with such a high attack rate that is used by this instrument).

I tried many different alterations in these custom instrument settings, but the values shown in the above table gave the best result.

I also tried replicating the BD sound with the emulation model I've developed myself (as opposed to custom instrument captured on real hardware). This leads to the same conclusion: the above settings give the closest match. But I have to admit that the match is slightly worse than what the real hardware gives. So something is still wrong in my model, but I already knew that.


### Tom tom (TOM)

[Visually](tom-1c0.png) the TOM waveform looks fairly simple: a simple sine wave that's varying in amplitude. We know that the YM2413 only uses a single operator to generate this wave, so this waveform might be simply the output of the mod8 operator. And this is also how the existing emulators generate TOM.

A note about the frequency settings. The YM2413 application manual recommends fixed values for the registers 0x16-0x18 and 0x26-0x28. But that doesn't mean we have to stick to these recommendations for our reverse engineering. It turns out that (just as expected) the frequency of the TOM waveform varies with the settings from registers 0x18 and 0x28.

We still need to figure out the parameters for this (single) operator. A full instrument (2 operators) is configured via 8x8bit values, for a single operator we need _less_ than halve because the parameters that control the coupling between the operators (e.g. 'TL' and 'FB') are not needed.

After some experimentation I found that these values work well:

|   0  |   1  |   2  |   3  |   4  |   5  |   6  |   7  |
|:----:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|
| 0x05 |   /  |   /  |   /  | 0xF8 |   /  | 0x59 |   /  |

'/' is "don't care"

Above we saw that TOM uses the 'mod8' operator-slot. So it makes sense that only the setting values that control modulator aspects are relevant. Those are the even registers. Register 2 is also even but it's still irrelevant because it controls 'TL' (the coupling between modulator and carrier). Register 2 also controls 'KSL', but it turns out that for rhythm instrument settings KSL is always zero, so for simplicity I've just marked register 2 as don't care.

### Snare drum (SD)

[Visually](sd-300.png) the SD waveform is periodically positive and negative. When [zoomed in](sd-300-zoom.png) on the green rectangle, we see that the wave randomly varies between zero and maximum (positive or negative). The envelope can likely be described by the normal ADSR stuff.

Detail: this waveform was captured with fnum=0x100, block=1 instead of the recommended values fnum=0x150, block=2. This means that the recommended SD waveform alternates faster between positive and negative values than what is shown in these images.

#### Phase

In the [previous post](../lfsr/lfsr.txt) I had already reverse engineered the YM2413 pseudo-random generator (in fact it was reverse engineered via capturing this SD waveform (but with fnum=0)). The random generator turned out to be a LFSR with polynomial: x<sup>23</sup> + x<sup>9</sup> + x<sup>8</sup> + x + 1.

The existing YM2413 emulators agree on the general principle:
- Take the upper bit of the phase-counter and use this to toggle between positive and negative output.
- Take the noise output (single bit) and use this to toggle between zero and maximum amplitude.

Though they differ in detail:
- Burczynski: takes the phase of mod7, but envelope of car7
- Okazaki: takes both phase and envelope of car7.

*TODO design an experiment to test whether phase comes from mod7 or car7. I can't immediately think of something.*

Burczynski combines the information like this:

|         | phase(8)=0 | phase(8)=1 |
|:-------:|:----------:|:----------:|
| noise=0 |    +max    |    -0      |
| noise=1 |     +0     |   -max     |

And Okazaki combines it like this:

|         | phase(8)=0 | phase(8)=1 |
|:-------:|:----------:|:----------:|
| noise=0 |    -max    |   +max     |
| noise=1 |     -0     |    +0      |

But neither was an exact match for my measurements. After some trial and error I found that the following does work (2 possibilities):

|         | phase(8)=0 | phase(8)=1 | | | | | | |         | phase(8)=0 | phase(8)=1 |
|:-------:|:----------:|:----------:|-|-|-|-|-|-|:-------:|:----------:|:----------:|
| noise=0 |     +0     |   -max     | | | | | | | noise=0 |    -max    |    +0      |
| noise=1 |    +max    |    -0      | | | | | | | noise=1 |     -0     |   +max     |

Notice:
- Emulators output 4 distinct values +max, -max, +0, -0. But I don't observe the value -0 in my measurements. (Remember +0 and -0 result in a different output on the YM2413 DAC).
  <br/>**UPDATE:** I measured again (after re-calibration) and now I _do_ see both +0 and -0.
- +max and -max appear diagonally from each other in the table (Burczynski got this detail right).

There are two possibilities (2 columns swapped) because I can't observe the value of the phase counter (*TODO design an experiment for this ... Maybe capture SD soon after melodic->rhythm mode transition ??*).

But I also can't directly observe the noise output bit, so how come there are not 4 possibilities (with also the rows swapped)? This is because a LFSR is asymmetric. The period of the YM2413 LFSR is 2<sup>23</sup>-1, in general for any maximum-length LFSR it's *one less* than a power of two. This is an odd number, thus it cannot have the same number of 0 and 1 outputs (in fact there will always be exactly one more 1-output than 0-output). And this means that if you invert the output of a LFSR, the resulting sequence *cannot* be produced by the same LFSR (but with a different start-state).

But still, how do we know that we got the LFSR right? Can't there be a different LFSR that does produce the inverted output? I have to admit that I didn't consider this when reverse-engineering the LFSR. So I went back to my [previous programs](../lfsr/lfsr.cc), feed it the inverted noise sequence and I got this LFSR:

  x<sup>24</sup> + x<sup>23</sup> + x<sup>10</sup> + x<sup>8</sup> + x<sup>2</sup> + 1

I strongly believe this is **not** the LFSR used in the YM2413 because:
- It's more expensive to implement in hardware compared to the other LFSR.
- It's not maximal length. Depending on the start-state it reproduces the original sequence, or the inverted sequence or it's stuck in a state where it constantly outputs 1, or (as always) the zero-state where it constantly outputs 0.

#### Envelope

Apart from phase (+max, +0, -0, -max) we also have to reverse engineer the envelope. I found that the following gives a pretty good match. (Because SD is produced by 'car7', now only the odd registers are relevant, but register 3 is irrelevant).

|   0  |   1  |   2  |   3  |   4  |   5  |   6  |   7  |
|:----:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|
|   /  | 0x01 |   /  |   /  |   /  | 0xD8 |   /  | 0x68 |


### Top cymbal (TCY)

Let's again start by looking at some images of a captured TCY waveform. The [first image](top-300-300.png) is a fully zoomed-out version. This clearly shows the ADSR phases in the envelope. For this capture, key=on lasted for 10000 samples. If you look closely, at x=10000, you see the transition between decay and release.

When we [zoom in](top-300-300-zoom1.png) and [zoom in further](top-300-300-zoom2.png) the phase behavior becomes visible. Notice that (apart from the changing envelope) the amplitude only seems to take on two distinct values (maximum positive or maximum negative). The phase variations seem complex but regular, likely not the result of the (LFSR) random generator. There seem to be 2 effects in play:
- The signal periodically stays constant or varies rapidly.
- Within such a period of rapid variation, there are smaller segments where the signal does remain constant.

#### Phase

So the phase only takes on two values. That means we *only* need to figure out a formula for this boolean result: positive or negative. Seems simple enough. *In reality this wasn't easy at all (for me), this part took me quite some time.*

Let again look at what the existing YM2413 emulators do. (This is not how it's actually implemented in those emulators, I've rewritten the code to make comparisons with each other and with the final solution easier):

Common code:
```
    // phaseC8 is the phase-counter of operator 'car8'
    bool c85 = phaseC8 & 0x20;
    bool c83 = phaseC8 & 0x08;

    // phaseM7 is the phase-counter of operator 'mod7'
    bool m77 = phaseM7 & 0x80;
    bool m73 = phaseM7 & 0x08;
    bool m72 = phaseM7 & 0x04;
```
Burczynski:
```
    return (c85 || c83 || m73 || (m72 != m77)) ? negative_0dB
                                               : positive_0dB;
```
Okazaki:
```
    return ((m72 ^ m73) | m77) ^ (c83 & !c85) ? negative_3dB
                                              : positive_3dB;
```
Both formulas are very different, but at least they agree on which 5 bits influence the result. Notice how the phase-counters of *two different operators* (mod7 and car8) are used (the YM2413 application manual only states that car8 is used for TCY).

Both formulas differ in the amplitude they output (0dB versus -3dB). But measurements show that at the end of the attack phase the signal does span the full -255..+255 range. That means that, for this aspect, Burczynski is right.

Measurements also show that indeed both the frequency settings for channel 7 (0x17, 0x27) and for channel 8 (0x18, 0x28) influence how the TCY waveform looks.

When a to-be-reverse-engineered output is influenced by two different inputs it helps to study the effect on the output by only letting one of the two inputs vary. In this specific case that can be done by setting the frequency of channel 7 or of channel 8 to zero. Such measurements do match predictions made by the Burczynski formula, that's promising. However when both phases change simultaneously, the predictions no longer match the measurements.

This took me quite a long time, but in the end I figured out a simple formula that can explain all measurements. I did this by (in principle) constructing the 2<sup>2<sup>5</sup></sup> possible truth-tables for a 5-bit input boolean function and eliminating those that don't match the measurements. One extra complication is that we do know how the phase is incremented but we don't know the start value of the mod7 and car8 phase counters. So (again in principle) each truth-table has to be tested against a measurement for 1024x1024 possible start positions. (The algorithm I sketched here is way too slow, but it can be well optimized, see [the source code](top4.cc) in case you're interested). (_One of the reasons why this took me so long was that I made a mistake in two of my measurements and this prevented me from finding any solutions_).

The solution I found was this:
```
    return ((c85 ^ c83) & (m77 ^ m72) & (c85 ^ m73)) ? positive_0dB
                                                     : negative_0dB;
```

Actually I did not find a single solution but 16 possible solutions:
1. (c85 == c83) & (m77 == m72) & (c85 == m73)
2. (c85 == c83) & (m77 == m72) & (c85 != m73)
3. (c85 == c83) & (m77 != m72) & (c85 == m73)
4. (c85 == c83) & (m77 != m72) & (c85 != m73)
5. (c85 != c83) & (m77 == m72) & (c85 == m73)
6. (c85 != c83) & (m77 == m72) & (c85 != m73)
7. (c85 != c83) & (m77 != m72) & (c85 == m73)
8. (c85 != c83) & (m77 != m72) & (c85 != m73)
9. (c85 == c83) & (m77 == m72) & (m73 == (c85 ^ m77))
10. (c85 == c83) & (m77 == m72) & (m73 != (c85 ^ m77))
11. (c85 == c83) & (m77 != m72) & (m73 == (c85 ^ m77))
12. (c85 == c83) & (m77 != m72) & (m73 != (c85 ^ m77))
13. (c85 != c83) & (m77 == m72) & (m73 == (c85 ^ m77))
14. (c85 != c83) & (m77 == m72) & (m73 != (c85 ^ m77))
15. (c85 != c83) & (m77 != m72) & (m73 == (c85 ^ m77))
16. (c85 != c83) & (m77 != m72) & (m73 != (c85 ^ m77))

I arbitrarily picked solution 8 because that one has the most elegant software implementation. Obviously the real YM2413 hardware only implements one of these formulas. Then why did my program find 16 formulas. The reason for this is that the start positions of the mod7 and car8 phase-counters is left open. You can indeed mathematically prove that each of these 16 formulas can be transformed into any of the other by adding some fixed offset to the initial phases.

TODO *would it be possible to design an experiment that can rule out (some of) these formulas? Perhaps by triggering an attack event in melodic mode (this resets the phase counters) and shortly after switching to rhythm mode?*

#### Envelope

Again, in addition to the phase, we also need to know how the envelope behaves. The following values make the predicted envelope match pretty well with the measured envelope:

|   0  |   1  |   2  |   3  |   4  |   5  |   6  |   7  |
|:----:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|
|   /  | 0x01 |   /  |   /  |   /  | 0xAA |   /  | 0x55 |


### High hat (HH)

As before, let's start with showing some [captured data](hh-L2000-100-300.png) (details: fnum7=0x100, block7=0, fnum8=0x100, block8=1, keyon-duration=2000). This seems like a fairly standard ADSR envelope. If we [zoom in](hh-L2000-100-300-zoom.png) further, the phase behavior becomes visible.

#### Phase

The phase behavior of HH is the most complex all YM2413 rhythm waveforms. But it turns out to be a combination of stuff we've already reverse engineered for the SD and TCY waveforms. So after that it wasn't too hard anymore to figure out HH. This is the formula I obtained:
```
   bool pp = (b85 ^ b83) & (b77 ^ b72) & (b85 ^ b73);
   int phase = (noise & 1) ? (pp ? 0xd0 : 0x234)
	                   : (pp ? 0x34 : 0x2d0);
```
Notice how the sub-expression for ```pp``` is the same as for TCY and how the ```pp``` and ```noise``` bits are combined in a very similar way as is done for SD.

#### Envelope

These values make the predicted envelope match the measurement:

|   0  |   1  |   2  |   3  |   4  |   5  |   6  |   7  |
|:----:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|
| 0x01 |   /  |   /  |   /  | 0xC8 |   /  | 0xA7 |   /  |


## Other

- TODO move key-on OR behavior from introduction to here?
- TODO What exactly happens when we switch rhythm mode on/off while sounds are playing.
- TODO What with the sustain bit in regs 0x26-0x28?


## Nuke.YKT

Very recently Laurens Holst (Grauw) pointed me to a [new software YM2413 emulator](https://github.com/vgmrips/vgmplay/blob/master/VGMPlay/chips/opll.c) made by 'Nuke.YKT'. I did not yet fully study this new code, but it's already clear that this emulator is way more accurate than the emulators by Okazaki and Burczynski. So let's also compare my findings with Nuke.YKT.

### Envelope

Nuke.YKT uses these instrument settings for the rhythm sounds:

|       |   0  |   1  |   2  |   3  |   4  |   5  |   6  |   7  |
|:-----:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|
|BD(mod)| 0x01 | 0x00 | 0x18 | 0x0F | 0xDF | 0x00 | 0x6A | 0x00 |
|HH     | 0x01 | 0x00 | 0x00 | 0x00 | 0xC8 | 0x00 | 0xA7 | 0x00 |
|TOM    | 0x05 | 0x00 | 0x00 | 0x00 | 0xF8 | 0x00 | 0x59 | 0x00 |
|BD(car)| 0x00 | 0x01 | 0x00 | 0x00 | 0x00 | 0xF8 | 0x00 | 0x6D |
|SD     | 0x00 | 0x01 | 0x00 | 0x00 | 0x00 | 0xD8 | 0x00 | 0x68 |
|TCY    | 0x00 | 0x01 | 0x00 | 0x00 | 0x00 | 0xAA | 0x00 | 0x55 |

'BD(mod)' and 'BD(car)' can be combined (without conflict) into a single set of settings. And it are the same values we got above. Also for the other 4 sounds, if we replace the "don't care" values with 0x00 we obtain the same values as in this table.

To be completely honest: when Grauw showed me this new emulator, I wasn't 100% finished yet with reverse engineering the instrument settings. (For me) this isn't a simple task because, a) my measurements are a bit noisy and b) my emulation model isn't 100% accurate. So for some of the setting values I still had a range of possible candidates. However I can say that the values for my best candidates were never more than +/-1 away from the Nuke.YKT values. So for now I going to assume that the Nuke.YKT values are correct (and I cheated a bit and already used these values in the sections above).

### Phase

It's more interesting to compare the formulas for the phases. And in particular for the SD, TCY and HH instruments.


#### Snare drum (SD)

Nuke.YKT uses this formula:
```
    pg_out = (chip->rm_hh_bit8 << 9)
           | ((chip->rm_hh_bit8 ^ (chip->rm_noise & 1)) << 8);
```

So the possible outputs are: 0x000, 0x100, 0x200 and 0x300. Transformed to the table-form I used above that results in:

|         | phase(8)=0 | phase(8)=1 |
|:-------:|:----------:|:----------:|
| noise=0 |     +0     |   -max     |
| noise=1 |    +max    |  **-0**    |

This matches one of the two possibilities I got, _except_ that this formula produces both +0 and -0, while I only measured +0.
_TODO double check this_.

**UPDATE:** I measured again, and my new results do show +0 and -0. So Nuke.YKT is correct.


#### Top cymbal (TCY)

Nuke.YKT uses the formula:
```
    rm_bit = (chip->rm_hh_bit2 ^ chip->rm_hh_bit7)
           | (chip->rm_hh_bit3 ^ chip->rm_tc_bit5)
           | (chip->rm_tc_bit3 ^ chip->rm_tc_bit5);
    pg_out = (rm_bit << 9) | 0x80;

```

Let's first focus on the part where ```rm_bit``` gets calculated. The variable ```rm_hh_bit2``` corresponds to what I've called ```m72``` and similar for the others. After translating the names and rearranging the terms that results in:

```
    rm_bit = (c85 ^ c83) | (m77 ^ m72) | (c85 ^ m73);
```

I got a similar formula, except that I got AND instead of OR operations.
```
             (c85 ^ c83) & (m77 ^ m72) & (c85 ^ m73)
```
But remember that I got 16 solutions from which I arbitrarily picked the 8th solution. If instead we pick the 1st solution, and then invert that result, we get:
```
             !((c85 == c83) & (m77 == m72) & (c85 == m73))
```
And with some boolean simplifications:
```
             !((c85 == c83) &  (m77 == m72) &  (c85 == m73))
              !(c85 == c83) | !(m77 == m72) | !(c85 == m73)
    rm_bit =   (c85 ^  c83) |  (m77 ^  m72) |  (c85 ^  m73)
```
Which is exactly the same as what Nuke.YKT got. Actually it's exactly the inverse, but we still have to see how this intermediate result is used further on.

Now for the second part:
```
    pg_out = (rm_bit << 9) | 0x80;
```
This results in two possible phase values: 0x80 and 0x280. We have 10 bits (1024 values) for a full sine period, so these values correspond to 'sin(45)' and 'sin(225)', which is '+3dB' and '-3dB'. This differs from what I saw in my measurements (I got +0dB and -0dB), but it _does_ match the Okazaki behavior.

I'm fairly confident in my measurements: I clearly see the maximum TCY amplitude going all the way to -255 which is only possible with phase = 0x300 (-> sin(270) = -1). The Nuke.YKT emulation code handles 3 chip types: YM2413, YM2413B and VRC VII, my measurements are exclusively done on a YM2413. _Could it be that the 3 chip types differ in this detail? And that the Nuke.YKT emulation overlooked this difference?_

**UPDATE:** Nuke.YKT confirmed my result and fixed his code. His formula is now:
```
    pg_out = (rm_bit << 9) | 0x100;
```

#### High hat (HH)

```
    rm_bit = (chip->rm_hh_bit2 ^ chip->rm_hh_bit7)
           | (chip->rm_hh_bit3 ^ chip->rm_tc_bit5)
           | (chip->rm_tc_bit3 ^ chip->rm_tc_bit5);
    pg_out = rm_bit << 9;
    if (rm_bit ^ (chip->rm_noise & 1)) {
        pg_out |= 0xd0;
    } else {
        pg_out |= 0x34;
    }
```
I got:
```
   bool pp = (b85 ^ b83) & (b77 ^ b72) & (b85 ^ b73);
   int phase = (noise & 1) ? (pp ? 0xd0 : 0x234)
                           : (pp ? 0x34 : 0x2d0);
```
In the previous subsection I already explained that I had 16 choices for the ```pp``` formula. I arbitraily picked choice 8, but with choice 1 I get the inverse of Nuke's ```rm_bit``` formula. And with that change Nuke's ```pg_out``` value becomes _exactly_ the same as my ```phase``` value.



## Possible next steps

* A long standing item is reverse engineering the 15 (melodic) instrument settings. Maybe with the new Nuke.YKT emulation code this will be easier. (E.g. I could verify that his settings indeed match my measurements).

* Another problem is to fix the known inaccuracies in my own emulation model. I hope this will become easier by comparing my model with the Nuke.YKT emulation core.

* Grauw also pointed me to new information regarding the YM2413 test register. [This page](https://github.com/openMSX/openMSX/issues/581) has the details (or links further to even more details). It would be nice to confirm information this with my own measurements. And, once verified, it could possibly make futher reverse engineering easier.

Let's hope it won't take another year before the next update ;-)
