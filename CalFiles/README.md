# Analysis Cal Files
GRSISort Specific macros are contained in this folder, the `.cal` files included here are for example.
The file names correspond to the ILL experiment numbers.
Creating the `.cal` file is integral to identifying the detectors in various channels, and in turn, also creating the analysis trees used in GRSISort.

### Instructions
For instructions on how to convert the FIPPS LUTs provided with experiments to GRSI Cals see the readme located [here](https://github.com/UoG-Nuclear-Physics-Group/ILLData/blob/master/README.md)

### Tips and Tricks
#### Addressing
Most digitizers being used with FIPPS have 16 or less channels available.
The first four bits of the detector's address correspond to `index-1`, and thus range from `0x0` to `0xf`.

The next eight bits correspond to the address of the digitizer.
Each digitizer usually is some multiple of four shifted left by one, ie. `0x000`, `0x040`, `0x080` and so on.

For example, the detector connected to the eleventh channel (index of eleventh) on the fourth digitizier has an address of `0x0cb`.
