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

	//If TXorRX == 0 set the transmitter's antenna pattern value.
	if (TXorRX == 0){
		path->A_tx.pattern[azimuth][elevation] = value;
	}
	//At the moment anything but 0 is the RX.
	else {
		path->A_rx.pattern[azimuth][elevation] = value;
	}
};

int ReadType13(struct Antenna *Ant, char * DataFilePath, double bearing, int silent) {

	char line[256];		// Read input line
	char instr[256];	// String temp

	int i, j;			// Loop counters
	int azin, elen;		// Number of elevations and azimuths
	int iazi;			// Offset azimuth counter
	int iMBOS;			// Integer offset of the main beam azimuth
	int iI = 0;			// Temp

	double MaxG = 0.0;	// Maximum gain

	FILE * fp;

	azin = 360;			// Fixed number of azimuths at 1-degree intervals
	elen = 91;			// Fixed number of elevations at 1-degree intervals

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
	 *
	 */

	for(i=0; i<azin; i++) {
		// Advance to the next azimuth and roll it over if necessary.
		iazi = (iMBOS+i)%360;

		fgets(line, sizeof(line), fp);
		sscanf(line, " %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", 
			&iI, &Ant->pattern[iazi][0], &Ant->pattern[iazi][1], &Ant->pattern[iazi][2], &Ant->pattern[iazi][3], &Ant->pattern[iazi][4],
			     &Ant->pattern[iazi][5], &Ant->pattern[iazi][6], &Ant->pattern[iazi][7], &Ant->pattern[iazi][8], &Ant->pattern[iazi][9]);
		for(j=10; j<90; j += 10) {
			fgets(line, sizeof(line), fp);
			sscanf(line, " %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", 
				&Ant->pattern[iazi][j],   &Ant->pattern[iazi][j+1], &Ant->pattern[iazi][j+2], &Ant->pattern[iazi][j+3], &Ant->pattern[iazi][j+4],
				&Ant->pattern[iazi][j+5], &Ant->pattern[iazi][j+6], &Ant->pattern[iazi][j+7], &Ant->pattern[iazi][j+8], &Ant->pattern[iazi][j+9]);
		};
		fgets(line, sizeof(line), fp);
		sscanf(line, " %lf\n", &Ant->pattern[iazi][90]);
	};

	return RTN_READTYPE13OK;
	
};

void IsotropicPattern(struct Antenna *Ant, double G) {
	
	int azin, elen;		// Number of elevations and azimuths
	int i, j;			// Loop counters
	
	azin = 360;			// Fixed number of azimuths at 1-degree intervals
	elen = 91;			// Fixed number of elevations at 1-degree intervals

	for(i=0; i<azin; i++) {
		for(j=0; j<elen; j++) {
			Ant->pattern[i][j] = G;
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