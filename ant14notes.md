###Background
ITURHFProp currently supports VOACAP Type 13 pattern files, containing a tables of antenna gain vs. azimuth, elevation, and frequency. These files are commonly used to represent pattern data for fixed directional antennas.  VOACAP type 14 files contain tables of antenna gain vs. the elevation angle and frequency. These files are commonly used for omni-directional antennas, typically verticals, and for directional rotated antennas. In the latter case, we are not interested in the horizontal pattern because we always point the antenna at the target site, so only the vertical pattern of the main lobe is of interest.

Type 14 files typically contain pattern data for multiple frequencies in the range 1->30MHz.

###Overview of Changes 
In order to represent pattern data for multiple frequencies, the existing pattern data structure has been modified to include an additional layer.  Pattern data is now contained in a 3D array accessed by \[frequency\]\[azimuth\]\[elevation\].  In addition to the pattern, we need to store a complementary list of frequencies in the 1D array `Ant.freqs`.  As this array is dynamically allocated, we also need to use an integer variable, `Ant.numFreqs`, to track the size of `Ant.freqs`.  The index of the frequency array corresponds to the index of the pattern array.

e.g. If we have pattern data available for an antenna at three frequencies the Ant structures would contain the following;

numFreqs = 3

freqs = \[5.0, 7.35, 14.1\]

pattern = \[0\]\[azi\]\[ele\] (Gain at 5.0MHz), \[1\]\[azi\]\[ele\] (Gain at 7.35MHz), \[2\]\[azi\]\[ele\] (Gain at 14.1MHz)

###Memory Allocation 
As each antenna type (13, 14, Isotropic) can support different numbers of frequencies which are only known at run time, memory allocation for the pattern data structure has been moved from `AllocatePathMemory()` in PathMemory.c to the function responsible of parsing the data file in ReadType13.c. Type 13 and Isotropic files currently only support a single frequency, type 14 files support 30 frequencies. The corresponding calls to `free()` the pattern data structure remain in `FreePathMemory()` in PathMemory.c.  `FreePathMemory()` has been modified to accommodate the additional layer in the pattern data structure and also the `Ant.freqs` array.

###ReadInputConfiguration
The `ReadInputConfiguration()` function has been modified to look for files with the suffix '.t14' and if so call up the `ReadType14()` function.  All other suffixes will call up `ReadType13()`.

###ReadType13() & IsotropicPattern()
These functions have been modified to include memory allocation.  In the case of the `ReadType13()` function, the Ant.freqs array is populated with the frequency value read from forth parameter (line 6) in the input file.  In the case of the Isotropic antenna, a frequency of 0.00MHz has been used to indicate that the pattern does not change with frequency.  This convention is not actually used any where at the moment but may be useful later.

###ReadType14()
The function `ReadType14()` has been added to the file ReadType13.c.  This function is based on the original `ReadType13()` function, adapted to include memory allocation and handling pattern data for multiple (30) frequencies.

###Antenna Gain
The `AntennaGain()` function has been modified to handle multiple frequencies.  The frequency of operation is read from the Path data structure and used to determine the index of the closest frequency available in the data structure.  

###Issues

* The Type 14 antenna files include a value of antenna efficiency for each frequency.  At the moment, this value is not actually used.  Should this value be added to the gain before insertion into the pattern data array?
* As memory is allocated in the ReadTypexx() functions, it is possible that `SetAntennaPatternVal()` can be called before the pattern data structure is allocated.  Maybe this function could be modified to check for pattern==NULL and create a fresh structure accordingly.  The parameters list does not currently accommodate frequency to be defined and all entries are directed to frequency index 0.

###Future Work/Improvements
* It seems a little wasteful to duplicate the pattern data across 360 degrees when dealing with omni-directional antenna patterns.  It may be an idea to add a flag in the Ant structure to indicate that the antenna is directional.  If TRUE, the second dimension of the pattern data structure comprises 360 elements as it does now.  If set FALSE (omni-directional), the second dimension of the pattern data structure is an array of length one.  This flag can be used by the `AntennaGain()` function when accessing the gain for the specified azimuth/elevation.
* Interpolate between gains if multiple frequency data is available. e.g. if path.freq = 15.6MHz, return a value of G derived by interpolating between gains of (say) 15 and 16MHz.  I'm not sure if this will offer significant or even measurable improvements in prediction accuracy.
* At the moment `ReadType14()` only handles 'standard' VOACAP type 14 files containing 30 frequencies.  This could be improved to a double pass approach capable of reading an arbitrary number of frequencies.
* Some references indicate that it is possible to include data for more than one frequency in a type 13 file.  I've never actually seen this 
* Move the pattern memory allocation to a separate function that accepts three args representing the dimensions.  This would reduce some code duplication.  This could also be called from `SetAntennaPatternVal()` in the event the function is called prior to memory being allocated.
