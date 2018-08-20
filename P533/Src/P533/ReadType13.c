#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Local includes
#include "Common.h"
#include "P533.h"
// End local includes

DLLEXPORT void SetAntennaPatternVal(struct PathData * path, int TXorRX, int azimuth, int elevation, double value) {
	/*
	SetAntennaPatternVal() - Set a value in an antenna pattern. This is especially useful for
		when you're calling this from managed code.

	INPUT
		struct PathData
		int TXorRX
		int azimuth
		int elevation
		double value

	*/
	int frequencyIndex = 0;

	//If TXorRX == 0 set the transmitter's antenna pattern value.
	if (TXorRX == 0){
		path->A_tx.pattern[frequencyIndex][azimuth][elevation] = value;
	}
	//At the moment anything but 0 is the RX.
	else {
		path->A_rx.pattern[frequencyIndex][azimuth][elevation] = value;
	}
};

int ReadType13(struct Antenna *Ant, char * DataFilePath, double bearing, int silent) {

	char line[256];		// Read input line
	char instr[256];	// String temp

	int i, j, m, n;			// Loop counters
	int azin, elen;		// Number of elevations and azimuths
	int iazi;			// Offset azimuth counter
	int iMBOS;			// Integer offset of the main beam azimuth
	int iI = 0;			// Temp

	double MaxG = 0.0;	// Maximum gain
	double *freqList;   // List of frequencies for which we have pattern data
	double ***antpat;

	FILE * fp;
	azin = 360;			// Fixed number of azimuths at 1-degree intervals
	elen = 91;			// Fixed number of elevations at 1-degree intervals

	Ant->numFreqs = 1;
	freqList = (double *) malloc(Ant->numFreqs * sizeof(double *));
	if(freqList != NULL) {
		Ant->freqs = freqList;
	} else {
		return RTN_ERRALLOCATEANT;
	}
	// freq of 0 indicates the pattern is not frequency sensitive.
	Ant->freqs[0] = 0;

	antpat = (double ***) malloc(Ant->numFreqs * sizeof(double *));
 	for (m=0; m < Ant->numFreqs; m++) {
 		antpat[m] = (double **) malloc(azin * sizeof(double *));
 		for (n=0; n<azin; n++) {
 			antpat[m][n] = (double*) malloc(elen * sizeof(double));
 		}
 	}
	if(antpat != NULL) {
		Ant->pattern = antpat;
	} else {
		return RTN_ERRALLOCATEANT;
	}

	// Determine the azimuth direction that the antenna is pointing to find the index offset.
	// Ideally the antenna pattern could be rotated to any position and then every gain value in the
	// pattern would be interpolated. In this implementation the pattern will be rotated to the nearest
	// integer azimuth degree. This approximate method was chosen because of the error of having the
	// pattern off by maximally +- 1/2 degree is considered to be minimal.
	iMBOS = (int)(bearing*R2D);

	// Read a VOACAP antenna pattern Type 13 file
	/*
	 * Typically, the header will look like the following:
	 *
	 * DeMinco Antenna ITS 2010 (ASCUH15)       :Sample type 13  360-degree gain table
	 * 4     4 parameters
	 * 9.450  [ 1] Max Gain dBi..:
	 *   13    [ 2] Antenna Type..: 360 x 91 gain values follow
	 *   0.0   [ 3] Efficiency (for IONCAP)
	 * 15.000  [ 4] Frequency
	 *
	 */

	fp = fopen(DataFilePath, "r");

	if (fp == NULL) {
		return RTN_ERRCANTOPENANTFILE;
	};


	if (fp == NULL) {
		return -1;
	};

	// The first line is the name of the antenna.
	// fgets will return a string that has a trailing "\n" which needs to be stripped off
	if (fgets(line, sizeof(line), fp) != NULL) {
		size_t len = strlen(line);
		if (len > 0 && line[len - 1] == '\n') {
			line[--len] = '\0';
		};
	};

	//Ant->numFreqs = 1;
	strcpy(Ant->Name, line);	// Store it to the path structure.

	// User feedback
	if(silent != TRUE) {
		printf("ReadType13: Reading antenna %.35s\n", Ant->Name);
	};

	fgets(line, sizeof(line), fp);		// Number of parameters

	// The next lines are parameters
	fgets(line, sizeof(line), fp);		// Max Gain
	sscanf(line, " %lf %s\n", &MaxG, &instr);
	fgets(line, sizeof(line), fp);		// Antenna type
	sscanf(line, " %d %s\n", &iI, &instr);
	// Make sure this is a VOACAP "Type 13" antenna file
	if(iI != 13) {
		return RTN_ERRNOTTYPE13;
	};
	fgets(line, sizeof(line), fp); // Efficiency
	fgets(line, sizeof(line), fp); // Frequency
	Ant->freqs[0] = atof(line);
	//printf("Frequency: %f MHz",Ant->freqs[0]);

	/*
	 * There are 360 azimuth blocks that look like the following. The leading zero is the azimuth.
	 *
	 *	0    -99.999-15.670 -9.730 -6.320 -3.960 -2.180 -0.790  0.320  1.240  1.980
	 *		   2.590  3.080  3.470  3.760  3.970  4.100  4.160  4.150  4.060  3.910
	 *         3.680  3.390  3.030  2.590  2.070  1.460  0.770 -0.030 -0.950 -1.990
	 *        -3.200 -4.590 -6.200 -8.070-10.220-12.470-14.120-14.080-12.590-10.760
	 *        -9.130 -7.760 -6.650 -5.750 -5.030 -4.450 -4.000 -3.660 -3.420 -3.260
	 *        -3.190 -3.190 -3.260 -3.400 -3.610 -3.880 -4.230 -4.640 -5.120 -5.680
	 *        -6.330 -7.060 -7.890 -8.830 -9.900-11.120-12.510-14.080-15.810-17.570
	 *       -18.950-19.300-18.460-17.020-15.490-14.090-12.880-11.830-10.930-10.160
	 *        -9.500 -8.930 -8.450 -8.050 -7.710 -7.430 -7.200 -7.030 -6.910 -6.840
	 *        -6.820
	 */

	for(i=0; i<azin; i++) {
		// Advance to the next azimuth and roll it over if necessary.
		iazi = (iMBOS+i)%360;

		fgets(line, sizeof(line), fp);
		sscanf(line, " %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
			&iI, &Ant->pattern[0][iazi][0], &Ant->pattern[0][iazi][1], &Ant->pattern[0][iazi][2], &Ant->pattern[0][iazi][3], &Ant->pattern[0][iazi][4],
			     &Ant->pattern[0][iazi][5], &Ant->pattern[0][iazi][6], &Ant->pattern[0][iazi][7], &Ant->pattern[0][iazi][8], &Ant->pattern[0][iazi][9]);
		for(j=10; j<90; j += 10) {
			fgets(line, sizeof(line), fp);
			sscanf(line, " %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
				&Ant->pattern[0][iazi][j],   &Ant->pattern[0][iazi][j+1], &Ant->pattern[0][iazi][j+2], &Ant->pattern[0][iazi][j+3], &Ant->pattern[0][iazi][j+4],
				&Ant->pattern[0][iazi][j+5], &Ant->pattern[0][iazi][j+6], &Ant->pattern[0][iazi][j+7], &Ant->pattern[0][iazi][j+8], &Ant->pattern[0][iazi][j+9]);
		};
		fgets(line, sizeof(line), fp);
		sscanf(line, " %lf\n", &Ant->pattern[0][iazi][90]);
	};

	fclose(fp);
	return RTN_READTYPE13OK;

};


int ReadType14(struct Antenna *Ant, char * DataFilePath, double bearing, int silent) {
	char line[256];			// Read input line
	char instr[256];		// String temp

	int i, j, f, m, n;	// Loop counters
	int azin, elen;			// Number of elevations and azimuths
	double *freqList;   // List of frequencies for which we have pattern data
	double ***antpat;   // Antenna Pattern

	/*
	 * todojw What do I need to do with antenna efficiency?  These appear to be
	 * in dB.  I suspect that this needs to be added to the pattern gain values.
	 */

	double efficiency;	// Antenna efficiency
	int iI = 0;					// Temp

	double MaxG = 0.0;	// Maximum gain

	FILE * fp;

	azin = 360;					// Fixed number of azimuths at 1-degree intervals
	elen = 91;					// Fixed number of elevations at 1-degree intervals

  /* Assume we have frequency data available for 1-30MHz in 1MHz intervals,
	 * as is the case with standard voacap files.
	 */

	Ant->numFreqs = 30;
	freqList = (double *) malloc(Ant->numFreqs * sizeof(double *));
	if(freqList != NULL) {
		Ant->freqs = freqList;
	} else {
		return RTN_ERRALLOCATEANT;
	}
	antpat = (double ***) malloc(Ant->numFreqs * sizeof(double *));
	for (m=0; m < Ant->numFreqs; m++) {
 		antpat[m] = (double **) malloc(azin * sizeof(double *));
 		for (n=0; n<azin; n++) {
 			antpat[m][n] = (double*) malloc(elen * sizeof(double));
 		}
 	}
	if(antpat != NULL) Ant->pattern = antpat;

	// Read a VOACAP antenna pattern Type 14 file
	/*
	 * Typically, the header will look like the following:
	 *
	 * 3EL Yagi @10M
 	 *  3     3 parameters
   *  0.00  [ 1] Max Gain dBi..:
   *  14    [ 2] Antenna Type..: 30 x (efficiency + 91 gain values) follow
   *  14.0  [ 3] Frequency
	 *
	 */

	fp = fopen(DataFilePath, "r");

	if (fp == NULL) {
		return RTN_ERRCANTOPENANTFILE;
	};


	if (fp == NULL) {
		return -1;
	};

	// The first line is the name of the antenna.
	// fgets will return a string that has a trailing "\n" which needs to be stripped off
	if (fgets(line, sizeof(line), fp) != NULL) {
		size_t len = strlen(line);
		if (len > 0 && line[len - 1] == '\n') {
			line[--len] = '\0';
		};
	};

	strcpy(Ant->Name, line);	// Store it to the path structure.

	// User feedback
	if(silent != TRUE) {
		printf("ReadType14: Reading antenna %.35s\n", Ant->Name);
	};

	fgets(line, sizeof(line), fp);		// Number of parameters

	// The next lines are parameters
	fgets(line, sizeof(line), fp);		// Max Gain
	sscanf(line, " %lf %s\n", &MaxG, &instr);
	fgets(line, sizeof(line), fp);		// Antenna type
	sscanf(line, " %d %s\n", &iI, &instr);
	// Make sure this is a VOACAP "Type 14" antenna file
	if(iI != 14) {
		return RTN_ERRNOTTYPE14;
	};
	fgets(line, sizeof(line), fp); // Frequency

	/*
	 * There are 30 elevation blocks (1-30) that look like the following. The first line
	 * of each block starts with the frequency and is followed by the efficiancy and the
	 * gain values for 0-9 degrees.
	 *
	 *  2 -0.57 -55.646 -4.202 -0.683  0.814  1.634  2.143  2.483  2.721  2.892  3.015
   *            3.104  3.167  3.209  3.234  3.244  3.242  3.230  3.207  3.176  3.138
	 *            3.091  3.038  2.979  2.913  2.842  2.764  2.681  2.593  2.499  2.400
   *            2.296  2.187  2.073  1.954  1.830  1.701  1.567  1.429  1.285  1.136
   *            0.982  0.822  0.658  0.488  0.313  0.133 -0.054 -0.245 -0.443 -0.647
   *           -0.857 -1.074 -1.297 -1.527 -1.764 -2.009 -2.261 -2.521 -2.791 -3.069
   *           -3.356 -3.654 -3.963 -4.283 -4.615 -4.960 -5.319 -5.694 -6.085 -6.494
   *           -6.923 -7.374 -7.849 -8.350 -8.882 -9.447-10.051-10.699-11.398-12.158
   *          -12.990-13.908-14.930-16.094-17.436-19.022-20.963-23.463-26.987-33.009
   *          -50.573
	 */
  for (f = 0; f<30; f++) {
		Ant->freqs[f] = (double)f+1.0;

		fgets(line, sizeof(line), fp);
		sscanf(line, " %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
				&iI, &efficiency, &Ant->pattern[f][0][0], &Ant->pattern[f][0][1], &Ant->pattern[f][0][2], &Ant->pattern[f][0][3], &Ant->pattern[f][0][4],
				&Ant->pattern[f][0][5], &Ant->pattern[f][0][6], &Ant->pattern[f][0][7], &Ant->pattern[f][0][8], &Ant->pattern[f][0][9]);
		for(j=10; j<90; j += 10) {
			fgets(line, sizeof(line), fp);
			sscanf(line, " %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
				&Ant->pattern[f][0][j],   &Ant->pattern[f][0][j+1], &Ant->pattern[f][0][j+2], &Ant->pattern[f][0][j+3], &Ant->pattern[f][0][j+4],
				&Ant->pattern[f][0][j+5], &Ant->pattern[f][0][j+6], &Ant->pattern[f][0][j+7], &Ant->pattern[f][0][j+8], &Ant->pattern[f][0][j+9]);
		};
		fgets(line, sizeof(line), fp);
		sscanf(line, " %lf\n", &Ant->pattern[f][0][90]);

		// Copy the array of elevation data to the rest of the data structure.
		// This seems a little wasteful of memory; maybe we should define a 'directional'
		// parameter the Ant structure that would allow the Gain application to
		// use a single slice for omni-directional antennas.  todojw
		for (j=1; j<azin; j++) {
			memcpy(Ant->pattern[f][j], Ant->pattern[f][0], elen * sizeof(double));
		}
	}
	fclose(fp);
	return RTN_READTYPE14OK;
};


void IsotropicPattern(struct Antenna *Ant, double G) {

	int azin, elen;		// Number of elevations and azimuths
	int i, j, m, n;			// Loop counters
	double *freqList;   // List of frequencies for which we have pattern data
	double ***antpat;

	azin = 360;			// Fixed number of azimuths at 1-degree intervals
	elen = 91;			// Fixed number of elevations at 1-degree intervals

	Ant->numFreqs = 1;
	freqList = (double *) malloc(Ant->numFreqs * sizeof(double *));
	if(freqList != NULL) {
		Ant->freqs = freqList;
	} else {
		return RTN_ERRALLOCATEANT;
	}
	// freq of 0 indicates the pattern is not frequency sensitive.
	Ant->freqs[0] = 0;

	antpat = (double ***) malloc(Ant->numFreqs * sizeof(double *));
 	for (m=0; m<Ant->numFreqs; m++) {
 		antpat[m] = (double **) malloc(azin * sizeof(double *));
 		for (n=0; n<azin; n++) {
 			antpat[m][n] = (double*) malloc(elen * sizeof(double));
 		}
 	}
	if(antpat != NULL) {
		Ant->pattern = antpat;
	} else {
		return RTN_ERRALLOCATEANT;
	}

	for(i=0; i<azin; i++) {
		for(j=0; j<elen; j++) {
			Ant->pattern[0][i][j] = G;
		};
	};

	return;
};

//////////////////////////////////////////////////////////////////////////////
//      Copyright  International Telecommunication Union (ITU) 2018         //
//                     All rights reserved.                                 //
// No part of this publication may be reproduced, by any means whatsoever,  //
//              without written permission of ITU                           //
//////////////////////////////////////////////////////////////////////////////
