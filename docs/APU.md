# APU: 2A03 audio

The APU combines two pulse channels, triangle, noise, and DMC. The output stage mixes channel amplitudes, applies the NES nonlinear pulse/TND mix, and resamples into the host audio device's rate.

## Register ranges

- $4000-$4003: Pulse 1
- $4004-$4007: Pulse 2
- $4008-$400B: Triangle
- $400C-$400F: Noise
- $4010-$4013: DMC
- $4015: channel enables/status
- $4017: frame counter and IRQ inhibit

## Channel state

Channels require timer reloads, length counters, envelopes, pulse sweep, triangle linear counter, noise LFSR, DMC sample buffer, address/length counters and interrupt behavior. Timing is tied to CPU cycles and the APU frame sequencer.

The current milestone stores I/O writes but produces no audio and does not model APU channel behavior.
