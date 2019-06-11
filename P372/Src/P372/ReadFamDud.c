#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Local includes
#include "Noise.h"
// End local includes 

int ReadFamDud(struct PathData *path, const char * DataFilePath) {

	/*
	 * ReadFamDud() Reads the harmonized coefficient files from Peter Suessman. The files have been renamed from Suessman's
	 *		implementation to COEFFXX.TXT from itucofXX.txt, where XX is a two-digit number for the month. The names were 
	 *		changed so the binary names and the text file names correspond. For this implementation of ITU-R P.533-12 the 
	 *		only values that are used from these coefficient files are the arrays fakp[][], fakabp[][], dud[][][] and fam[][]
	 *		for the calculation of the atmospheric noise.   
	 *
	 *		INPUT
	 *			struct PathData *path
	 *
	 *		OUTPUT
	 *			path->fam
	 *			path->dud
	 *			path->fakp
	 *			path->fakabp
	 *
	 */
	 
	int i,j,k;
	int n;

	// A is the array that is read into from the file and aids in reshaping the target arrays
	// in the Coeff structure.
	double *A;
	
	char line[256];

	char CoeffFile[14];
	char InFilePath[270];
	
	FILE *fp;

	strcpy(InFilePath, DataFilePath);
	//We shouldn't modify the given path to the files, let the caller figure it out.
	//strcat(InFilePath, "coeffs/TXT/");
	sprintf(CoeffFile, "COEFF%02dW.txt", path->month+1);
	strcat(InFilePath, CoeffFile);

	/*if(ITURHFP.silent != TRUE) {
		printf("ReadFamDud: Opening File %s\n", InFilePath);
	};*/

	fp = fopen(InFilePath, "r"); 
	if(fp == NULL) {
		printf("ReadFamDud: ERROR Can't find input file - %s\n", InFilePath);
		// PMW Compatability- We can't use getchar() as there is noone to press "enter"
		// printf("Press Enter...");
		// getchar();
		return RTN_ERROPENCOEFFFILE;
	};
	

	// Read the first header line.
	retval = fgets(line, 256, fp);
	
	//************************************************************************************
	// Skip if2(10) & xf2(13,76,2)
	
	for(n=0; n<400; n++) {
		fgets(line, 256, fp); 
	};
	

	//************************************************************************************
	// Skip ifm3(10) & xfm3(9,49,2)
	
	for(n=0; n<181; n++) {
		fgets(line, 256, fp); 
	};
	
	//************************************************************************************
	// Skip ie(10) & xe(9,22,2)
	for(n=0; n<84; n++) {
		fgets(line, 256, fp); 
	};
	
	//************************************************************************************
	// Skip iesu(10) & xesu(5,55,2)
	
	for(n=0; n<114; n++) {
		fgets(line, 256, fp); 
	};

	//************************************************************************************
	// Skip ies(10) & xes(7,61,2)
	
	for(n=0; n<175; n++) {
		fgets(line, 256, fp); 
	};

	//************************************************************************************
	// Skip iels(10) & xels(5,55,2)
	
	for(n=0; n<114; n++) {
		fgets(line, 256, fp); 
	};

	//************************************************************************************
	// Skip ihpo1(10) & xhpo1(13,29,2)
	
	for(n=0; n<155; n++) {
		fgets(line, 256, fp); 
	};
	
	//************************************************************************************
	// Skip ihpo2(10) & xhpo2(9,55,2)

	for(n=0; n<202; n++) {
		fgets(line, 256, fp); 
	};

	//************************************************************************************
	// ihp(10) & xhp(9,37,2)

	for(n=0; n<138; n++) {
		fgets(line, 256, fp); 
	};
	
	//************************************************************************************
	// fakp(29,16,6)

	// Allocate the array A that will allow for reshaping
	A = (double*) malloc(29*16*6 * sizeof(double));

	// Read the line "fakp(29,16,6)"
	fgets(line, 256, fp);

	// Read 556 lines into the array A
	for(n=0; n<556; n++) {
		fgets(line, 256, fp); 
		sscanf(line, " %lf %lf %lf %lf %lf\n", A+5*n, A+5*n+1, A+5*n+2, A+5*n+3, A+5*n+4);
	};
	// Read the last partial line
	fgets(line, 256, fp); 
	sscanf(line, " %lf %lf %lf %lf\n", A+5*n, A+5*n+1, A+5*n+2, A+5*n+3);
		
	// Reshape A into the Coeff structure 
	for(i=0; i<6; i++) {
		for(j=0; j<16; j++) {
			for(k=0; k<29; k++) {
				path->fakp[i][j][k] = *(A+16*29*i+29*j+k);
			};
		};
	};

	// Free A
	free(A);
	//************************************************************************************
	// fakabp(2,6)

	// Allocate the array A that will allow for reshaping
	A = (double*) malloc(2*6 * sizeof(double));

	// Read the line "fakabp(2,6)"
	fgets(line, 256, fp);

	// Read 2 lines into the array A
	for(n=0; n<2; n++) {
		fgets(line, 256, fp); 
		sscanf(line, " %lf %lf %lf %lf %lf\n", A+5*n, A+5*n+1, A+5*n+2, A+5*n+3, A+5*n+4);
	};
	// Read the last partial line
	fgets(line, 256, fp); 
	sscanf(line, " %lf %lf\n", A+5*n, A+5*n+1);
		
	// Reshape A into the Coeff structure 
	for(j=0; j<6; j++) {
		for(k=0; k<2; k++) {
			path->fakabp[j][k] = *(A+2*j+k);
		};
	};

	// Free A
	free(A);
	//************************************************************************************
	// dud(5,12,5)
	
	// Allocate the array A that will allow for reshaping.
	A = (double*) malloc(5*12*5 * sizeof(double));

	// Read the line "dud(5,12,5)".
	fgets(line, 256, fp);

	// Read 60 lines into the array A.
	for(n=0; n<60; n++) {
		fgets(line, 256, fp); 
		sscanf(line, " %lf %lf %lf %lf %lf\n", A+5*n, A+5*n+1, A+5*n+2, A+5*n+3, A+5*n+4);
	};
		
	// Reshape A into the Coeff structure.
	for(i=0; i<5; i++) {
		for(j=0; j<12; j++) {
			for(k=0; k<5; k++) {
				path->dud[i][j][k] = *(A+5*12*i+5*j+k);
			};
		};
	};

	// Free A
	free(A);

	//************************************************************************************
	// fam(14,12)
	
	// Allocate the array A that will allow for reshaping.
	A = (double*) malloc(12*14 * sizeof(double));

	// Read the line "fam(14,12)".
	fgets(line, 256, fp);

	// Read 33 lines into the array A.
	for(n=0; n<33; n++) {
		fgets(line, 256, fp); 
		sscanf(line, " %lf %lf %lf %lf %lf\n", A+5*n, A+5*n+1, A+5*n+2, A+5*n+3, A+5*n+4);
	};
	// Read the last partial line.
	fgets(line, 256, fp); 
	sscanf(line, " %lf %lf %lf\n", A+5*n, A+5*n+1, A+5*n+2);
	
	// Reshape A into the Coeff structure.
	for(j=0; j<12; j++) {
		for(k=0; k<14; k++) {
			path->fam[j][k] = *(A+14*j+k);
		};
	};

	// Free A
	free(A);

	// Clean up;
	fclose(fp);

	return RTN_READFAMDUDOK;
};

//////////////////////////////////////////////////////////////////////////////
//      Copyright  International Telecommunication Union (ITU) 2018         //
//                     All rights reserved.                                 //
// No part of this publication may be reproduced, by any means whatsoever,  //
//              without written permission of ITU                           //
//////////////////////////////////////////////////////////////////////////////