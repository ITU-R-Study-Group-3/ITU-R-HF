#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <unistd.h>

// Local includes
#include "Common.h"
#include "Noise.h"
#include "ITURNoise.h"
// End Local includes

#define PRINTALL 100
#define PRINTCSV 101

// Local Prototypes
void PrintCSVLine(int month, int hour, double freq, double rlat, double rlng, double* out);
void PrintCSVHeader(const char* P372ver, const char* P372compt);
void PrintUsage();
int RunAtmosNoiseMonths(char* datafilepath);
void FindV_d(double freq, double c[5], double d[5], double* V_d, double* sigma_V_d);
// End Local Prototypes

int main(int argc, char* argv[]) {
	/*

		main() - This program takes command line arguments and runs the P372 calculation engine

			INPUT
				Argument 1:  month (1 to 12))
				Argument 2:  hour (1 to 24 (UTC))
				Argument 3:  frequency (0.01 to 30 MHz)
				Argument 4:  latitude (degrees)
				Argument 5:  longitude (degrees)
				Argument 6:  man-made noise 0-5 or value of man-made noise (dB)
								 CITY         0.0
								 RESIDENTIAL  1.0
								 RURAL		  2.0
								 QUIETRURAL	  3.0
								 NOISY		  4.0
								 QUIET		  5.0
				Argument 7:  data file path in double quotes without trailing back slash
				Argument 8:  print flag 0-3
								 PRINTHEADER	1 Prints the full header
								 PRINTCSV		3 Prints out simple csv
								 PRINTALL		2 Print out header with the simple csv
								 NOPRINT		0 Do not print
			OUTPUT
				The output is printed to stdout depending on the print flag

			Example: ITURNoise 1 14 1.0 40.0 165.0 0 "G:\\User\\Data"
							   Calculation made for January 14th hour (UTC)
							   at 40 degrees North and 165 degrees East

			******************************************************************************
					ITU-R Study Group 3: Radiowave Propagation
			******************************************************************************
							Analysis: 19/6/20 - 13:34:31
							P372 Version:      14.1
							P372 Compile Time: Fri Jun 19 10:57:00 2020
			******************************************************************************

					JAN : 13 (UTC) at 40.000000 (deg lat) 165.000000 (deg long)
					[FaA]  Noise Component (Atmospheric): 60.732650
					[DuA]  Upper Decile    (Atmospheric): 10.600910
					[DlA]  Upper Decile    (Atmospheric): 8.277667
					[FaM]  Noise Component    (Man-Made): 76.800000
					[DuM]  Upper Decile       (Man-Made): 11.000000
					[DlM]  Lower Decile       (Man-Made): 6.700000
					[FaG]  Noise Component    (Galactic): 52.000000
					[DuG]  Upper Decile       (Galactic): 2.000000
					[DlG]  Lower Decile       (Galactic): 2.000000
					[FamT] Noise                 (Total): 76.986526
					[DuT]  Upper Decile          (Total): 10.940193
					[DlT]  Lower Decile          (Total): 6.573930

			******************************************************************************

			*********************************************************************************************
			These software methods for the prediction of the performance of HF circuits based on
			Recommendations ITU-R P.533-14 and P.372-13
			The ITURHFProp, P533 and P372 software has been developed collaboratively by participants in ITU-R
			Study Group 3. It may be used by implementers in their implementation of the Recommendation as well
			as in revisions of the specific original Recommendation and in other ITU Recommendations, free from
			any copyright assertions.

			This software is provided “as is” WITH NO WARRANTIES, EXPRESS OR IMPLIED,
			INCLUDING BUT NOT LIMITED TO, THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
			AND NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.

			The ITU shall not be held liable in any event for any damages whatsoever (including, without
			limitation, damages for loss of profits, business interruption, loss of information, or any other
			pecuniary loss) arising out of or related to use of the software.
			***************************************************************************************************

			Behm 2022

	*/

	int pntflag = MNPRINTTOSTDOUT;
	int mnpntflag = MNPRINTTOSTDOUT;

	int month;
	int hour;
	int retval;

	double lat, lng;
	double rlat, rlng;
	double freq;
	double mmnoise;
	double out[12];

	char datafilepath[256];

	const char* P372ver;
	const char* P372compt;

	struct stat sb;

	if (argc == 2) {

		// Then the user wants to run all the tables necessary to create the
		// Atmospheric noise Figures in Recommendation P.372-14 
		// Assume that the only argument given is the path to the data files
		sprintf(&datafilepath[0], "%s\\", argv[1]);

		RunAtmosNoiseMonths(&datafilepath[0]);

		return RTN_ITURNOISEOK;

	}; // argc == 2

	// Are there enough command line arguments to proceed?
	if (argc > 6) {

		month = atoi(argv[1]) - 1;
		if ((month < 0) || (month > 11)) {
			printf("ITURNoise: Error: Month (%d) Out of Range (1 to 12) ", month + 1);
			return RTN_ERRMONTH;
		};

		hour = atoi(argv[2])%24;// - 1;
		if ((hour < 0) || (hour > 23)) {
			printf("ITURNoise: Error: Hour (%d (UTC)) Out of Range (1 to 24 UTC) ", hour /*+ 1*/);
			return RTN_ERRMONTH;
		};

		freq = atof(argv[3]);
		if ((freq < 0.01) || (freq > 30)) {
			printf("ITURNoise: Error: Frequency (%5.4f (MHz)) Out of Range (0.01 to 30 MHz) ", freq);
			return RTN_ERRMONTH;
		};

		lat = atof(argv[4]);
		if ((lat < -90.0) || (lat > 90.0)) {
			printf("ITURNoise: Error: Latitude (%5.4f (degrees)) Out of Range (-90 to 90 degrees) ", lat);
			return RTN_ERRMONTH;
		}
		else {
			rlat = lat * D2R;
		};

		lng = atof(argv[5]);
		if ((lng < -180.0) || (lng > 180.0)) {
			printf("ITURNoise: Error: Longitude (%5.4f (degrees)) Out of Range (-180 to 180 degrees) ", lng);
			return RTN_ERRMONTH;
		}
		else {
			rlng = lng * D2R;
		};

		mmnoise = atof(argv[6]); // 

		sprintf(&datafilepath[0], "%s\\", argv[7]);
		if (stat(datafilepath, &sb)) { // Check to see if the directory exists
			printf("ITURNoise: Error: Data file path %s does not exist\n", datafilepath);
			return RTN_ERRBADDATAFILEPATH;
		};

		if (argc >= 9) {
			pntflag = atoi(argv[8]);
		
			switch (pntflag) {
			case 0:
				// Passthru print flag to  MakeNoise()
				mnpntflag = MNNOPRINT;
				break; 
			case 1:
				// Passthru print flag to  MakeNoise()
				mnpntflag = MNPRINTTOSTDOUT;
				break; 
			case 2: 
				// Passthru print flag to  MakeNoise()
				mnpntflag = MNPRINTTOFILE;
				break;
			case 3:
				pntflag = PRINTCSVALL;
				break;
			case 4:
				pntflag = PRINTCSV;
				break;
			default: 
				// Passthru print flag to  MakeNoise()
				mnpntflag = MNPRINTTOSTDOUT;
			}
		};

		if ((pntflag == PRINTCSV) || (pntflag == PRINTCSVALL)) {
			// Tell MakeNoise() not to print that the printing will be here
			mnpntflag = MNNOPRINT;
		};

	// Load the Noise routines in P372.dll ******************************
#ifdef _WIN32
			int mod[512];
			// Get the handle to the P372 DLL.
			hLib = LoadLibrary(TEXT("P372.dll"));
			if (hLib == NULL) {
				printf("ITURNoise: AllocatePathMemory: Error %d P372.DLL Not Found\n", RTN_ERRP372DLL);
				return RTN_ERRP372DLL;
			};
			// Get the handle to the DLL library, hLib.
			GetModuleFileName((HMODULE)hLib, (LPTSTR)mod, 50);
			// Get the P372Version() process from the DLL.
			dllP372Version = (cP372Info)GetProcAddress((HMODULE)hLib, "P372Version");
			// Get the P372CompileTime() process from the DLL.
			dllP372CompileTime = (cP372Info)GetProcAddress((HMODULE)hLib, "P372CompileTime");
			dllMakeNoise = (iMakeNoise)GetProcAddress((HMODULE)hLib, "__MakeNoise@52");
#elif __linux__ || __APPLE__
			void* hLib;
			hLib = dlopen("libp372.so", RTLD_NOW);
			if (!hLib) {
				printf("Couldn't load libp372.so, exiting.\n");
				exit(1);
			};
			dllAllocateNoiseMemory = dlsym(hLib, "AllocateNoiseMemory");
			dllP372Version = dlsym(hLib, "P372Version");
			dllP372CompileTime = dlsym(hLib, "P372CompileTime");
			dllNoise = dlsym(hLib, "Noise");
			dllAllocateNoiseMemory = dlsym(hLib, "AllocateNoiseMemory");
			dllFreeNoiseMemory = dlsym(hLib, "FreeNoiseMemory");
			dllInitializeNoise = dlsym(hLib, "InitializeNoise");
			dllReadFamDud = dlsym(hLib, "ReadFamDud");
#endif	
	
			// Load the version and compile time of the P372.DLL
			P372ver = dllP372Version();
			P372compt = dllP372CompileTime();

			// Run MakeNoise() which calculates the noise parameters for a single point 
			retval = dllMakeNoise(month, hour, lat, lng, freq, mmnoise, datafilepath, &out[0], mnpntflag);

			if (retval == RTN_MAKENOISEOK) {
				if (pntflag == PRINTCSVALL) {
					PrintCSVHeader(P372ver, P372compt);
				};
				if ((pntflag == PRINTCSV) || (pntflag == PRINTCSVALL)){
					PrintCSVLine(month, hour, freq, rlat, rlng, &out[0]);
				};
			};
	}
	else {
		printf("ITURNoise: ERROR: Insufficient number (%d) of command line arguments, 7 required.\n", argc);
		printf("\n");
		PrintUsage();
		return RTN_ERRCOMMANDLINEARGS;
	};

	FreeLibrary(hLib);
 
return retval;

};

int RunAtmosNoiseMonths(char * datafilepath) {

	/*
		RunMonths - Outputs the files necessary to parse to create Recommendation 
					P.372-14 Figures of atmospheric noise.

			INPUT
				char * datafilepath Pointer to the CCIR data files 

			OUTPUT
				12 P.372-14 Atmospheric Noice a) Figure Data Files
				12 P.372-14 Atmospheric Noice b) Figure Data Files
				12 P.372-14 Atmospheric Noice c) Figure Data Files

		CAUTION: This routine relies on the code in Noise.c and specifically the 
		subroutine AtmosphericNoise(). Any modifications to AtmosphericNoise()
		need to be accounted for here and in utility program AtmosphericNoise_LT()
		which was a direct copy of AtmosphericNoice() as of June 2020 Ver 14.1.		
	
	*/

	// Load the Noise routines in P372.dll ******************************
#ifdef _WIN32
	// Get the handle to the P372 DLL.
	hLib = LoadLibrary(TEXT("P372.dll"));
	if (hLib == NULL) {
		printf("ITURHFProp: Error %d P372.DLL Not Found\n", RTN_ERRP372DLL);
		return RTN_ERRP372DLL;
	};
	int mod[512];
	// Get the handle to the DLL library, hLib.
	GetModuleFileName((HMODULE)hLib, (LPTSTR)mod, 50);
	// Get the P372Version() process from the DLL.
	dllP372Version = (cP372Info)GetProcAddress((HMODULE)hLib, "P372Version");
	// Get the P372CompileTime() process from the DLL.
	dllP372CompileTime = (cP372Info)GetProcAddress((HMODULE)hLib, "P372CompileTime");

	dllNoise = (iNoise)GetProcAddress((HMODULE)hLib, "Noise");
	dllAllocateNoiseMemory = (iNoiseMemory)GetProcAddress((HMODULE)hLib, "AllocateNoiseMemory");
	dllFreeNoiseMemory = (iNoiseMemory)GetProcAddress((HMODULE)hLib, "FreeNoiseMemory");
	dllInitializeNoise = (vInitializeNoise)GetProcAddress((HMODULE)hLib, "InitializeNoise");
	dllAtmosphericNoise_LT = (vAtmosphericNoise_LT)GetProcAddress((HMODULE)hLib, "AtmosphericNoise_LT");
	dllAtmosphericNoise = (vAtmosphericNoise)GetProcAddress((HMODULE)hLib, "AtmosphericNoise");
	dllReadFamDud = (iReadFamDud)GetProcAddress((HMODULE)hLib, "ReadFamDud");
#elif __linux__ || __APPLE__
	void* hLib;
	hLib = dlopen("libp372.so", RTLD_NOW);
	if (!hLib) {
		printf("Couldn't load libp372.so, exiting.\n");
		exit(1);
	}
	dllReadFamDud = dlsym(hLib, "ReadFamDud");
	dllP372Version = dlsym(hLib, "P372Version");
	dllP372CompileTime = dlsym(hLib, "P372CompileTime");
	dllNoise = dlsym(hLib, "Noise");
	dllAllocateNoiseMemory = dlsym(hLib, "AllocateNoiseMemory");
	dllFreeNoiseMemory = dlsym(hLib, "FreeNoiseMemory");
	dllInitializeNoise = dlsym(hLib, "InitializeNoise");
#endif

	FILE* fp = NULL;
	FILE* fp_V_d = NULL;
	FILE* fp_sigma_V_d = NULL;

	int i;
	int dummy = 0;
	int s, tb; // s = season and tb = timeblock counter for reading V_d and sigma_V_d data 
	int retval;
	int fn = 41; // Number of elements in the f_log array below

	const char* P372ver;
	const char* P372compt;

	struct NoiseParams noiseP;
	struct FamStats FamS;

	double freq;
	double rlat, rlng;
	double V_d, sigma_V_d;

	double c[4][6][5];
	double d[4][6][5];

	char line[256];
	char strl[5][256];

	// Frequency array that will be used for b) and c) Figure data generation 
	double f_log[41] = { 0.01, 0.015, 0.02, 0.025, 0.03, 0.035, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09,
		0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9,
		1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0,
		10.0, 15.0, 20.0, 25.0, 30.0};

	// Variables specific to the code fragment from AtmosphericNoise() 
	// for the b) figure data generation
	double pz, px, cz;
	double u[2];
	double Fam[11]; // Output array for b) figure data generation
	double Fam1MHz;
	// End Variables for code fragment from AtmosphericNoise() 

	struct tm* ntime;
	time_t tm;

	char ntimestr[64];

	char outputfilename[256];
	char acsvfilepath[256];
	char bcsvfilepath[256];
	char ccsvfilepath[256];
	char V_dfilepath[256];
	char sigma_V_dfilepath[256];
	char command[256];

	struct stat st = { 0 };
	
	// Open output file directories if necessary
	// The file structure for output files is static
	sprintf(acsvfilepath, "%s", ".\\P372_figures\\a\\csv\\");
	sprintf(bcsvfilepath, "%s", ".\\P372_figures\\b\\csv\\");
	sprintf(ccsvfilepath, "%s", ".\\P372_figures\\c\\csv\\");

	// Check to see if the A csv directory exists
	if (stat(acsvfilepath, &st) == -1) {
		sprintf(command, "mkdir %s", acsvfilepath);
		system(command);
	};

	// Check to see if the B csv directory exists
	if (stat(bcsvfilepath, &st) == -1) {
		sprintf(command, "mkdir %s", bcsvfilepath);
		system(command);
	};

	// Check to see if the C csv directory exists
	if (stat(ccsvfilepath, &st) == -1) {
		sprintf(command, "mkdir %s", ccsvfilepath);
		system(command);
	};

	// End opening output file directories

	//////////////////////////////////////////////////////////////////////////////
	// Open and read the V_d.txt and sigma_V_d.txt input files
	// NOTE: The data in V_d.txt and sigma_V_d.txt are not in the CCIR coefficeint 
	// files. They have been extracted from NTIA Report 85-173 which contains 
	// the Behm verified CCIR coefficient data that is used throughout in 
	// ITURHFProp(). 
	sprintf(V_dfilepath, "%s\\%s", datafilepath, "V_d.txt");
	fp_V_d = fopen(V_dfilepath, "r");
	if (fp_V_d == NULL) {
		printf("ITURNoise: Error: Can't open input file %s (%s)\n", V_dfilepath, strerror(errno));
		return RTN_ERRV_DCANTOPENFILE;
	};

	sprintf(sigma_V_dfilepath, "%s\\%s", datafilepath, "sigma_V_d.txt");
	fp_sigma_V_d = fopen(sigma_V_dfilepath, "r");
	if (fp_sigma_V_d == NULL) {
		printf("ITURNoise: Error: Can't open input file %s (%s)\n", sigma_V_dfilepath, strerror(errno));
		return RTN_ERRSIGMA_V_DCANTOPENFILE;
	};

	tb = 0;
	s = 0;
	while (fscanf(fp_V_d, "%[^\n] ", line) != EOF) {
		
		int retval = sscanf(line, "%d %d %s %s %s %s %s",
			&dummy, &dummy, &strl[4], &strl[3], &strl[2], &strl[1], &strl[0]);

		for (i = 0; i < 5; i++) {
			c[s][tb][i] = atof(strl[i]);
		};

		tb += 1;
		if (tb == 6) {
			tb = 0;
			s += 1;
		};
	};

	tb = 0;
	s = 0;
	while (fscanf(fp_sigma_V_d, "%[^\n] ", line) != EOF) {
		
		int retval = sscanf(line, "%d %d %s %s %s %s %s",
			&dummy, &dummy, &strl[4], &strl[3], &strl[2], &strl[1], &strl[0]);

		for (i = 0; i < 5; i++) {
			d[s][tb][i] = atof(strl[i]);
		};

		tb += 1;
		if (tb == 6) {
			tb = 0;
			s += 1;
		};
	};

	// End open and reading input files
	//////////////////////////////////////////////////////////////////////////////
	
	// Allocate the memory in the noise structure
	retval = dllAllocateNoiseMemory(&noiseP);
	if (retval != RTN_ALLOCATEP372OK) {
		return RTN_ERRALLOCATENOISE;
	};

	// Initialize Noise from the P372.dll
	dllInitializeNoise(&noiseP);
	// End Initialize Noise

	// Load the version and compile time of the P372.DLL
	P372ver = dllP372Version();
	P372compt = dllP372CompileTime();

	// Get the time to time stamp the output files.
	tm = time(NULL);
	ntime = localtime(&tm);

	// User feedback
	printf("************************************************************\n");
	printf("                    ITU-R Study Group 3\n");
	printf("************************************************************\n");
	printf("         ITURNoise: P372 Figure Data Generator\n");
	sprintf(ntimestr, "%0d/%0d/%0d %02d:%02d:%02d",
		ntime->tm_mday, ntime->tm_mon + 1, ntime->tm_year - 100,
		ntime->tm_hour, ntime->tm_min, ntime->tm_sec);
	printf("         Creation: %s\n", ntimestr);
	printf("************************************************************\n");
	printf("\nBegin Data Generation for a) Figures\n");

	// Make a filename compatible string from the time stamp
	// to be used in the loops below
	sprintf(ntimestr, "%0d%0d%0dd-%02d%02d%02dt",
		ntime->tm_mday, ntime->tm_mon + 1, ntime->tm_year - 100,
		ntime->tm_hour, ntime->tm_min, ntime->tm_sec);

	// Initialize variables
	noiseP.ManMadeNoise = RURAL;
	freq = 1.0;
	
	/*********************************************************************
						Generate a) Figure Data
       Expected values of atmospheric noise, Fam (dB above kT_0B at 1MHz)
    **********************************************************************/
	
	for (int m = 0; m < 12; m+=3) {

		// Read in the atmospheric coefficients for the particular month.
		// The subroutine dllReadFamDud() is from P372.dll
		retval = dllReadFamDud(&noiseP, datafilepath, m);
		if (retval != RTN_READFAMDUDOK) {
			return retval;
		};
			
		for (int h = 0; h <= 23; h+=4) { // hour local time
	
			sprintf(outputfilename, "%sa_%0dm%0dh.csv", acsvfilepath, m+1, h);
			fp = fopen(outputfilename, "w");
			if (fp == NULL) {
				printf("ITURNoise: Error: Can't open output file %s (%s)\n", outputfilename, strerror(errno));
				return RTN_ERRCANTOPENFILE;
			};
			
			// The file is open proceed
			// User feedback
			printf("Writing file %s\n", outputfilename);

			// Write the column headings to the file
			fprintf(fp, "month,hour,freq,latitude,longitude,FaA\n");
			
			for (int ilat = -90; ilat <= 90; ilat++) { //
				rlat = ilat * D2R;
				for (int ilng = -180; ilng <= 180; ilng++) { //
					rlng = ilng * D2R;
					
					// Call the AtmosphericNoise_LT() from the P372.dll
					// Which calculates the atmospheric noise and returns the full statistics. 
					dllAtmosphericNoise_LT(&noiseP, &FamS, h, rlng, rlat, freq);
										
					// Write the data out to the file
					fprintf(fp, "%d, %d, %5.4f, %5.4f, %5.4f, %5.4f\n", 
						m + 1, h, freq, rlat * R2D, rlng * R2D, FamS.FA);

				}; //  End Longitude loop
			}; // End Latitude loop

			fclose(fp);

		}; // End hour for
	}; // End month for
	
	/********************* End Generate a) Figure Data *******************/
	
	// User feedback
	printf("ITURNoise: Data for a) Figures Complete\n");
	printf("\nBegin Data Generation for b) Figures\n");

	// For the b) and c) plots a location is required
	// Boulder, Colorado
	rlat = 40.015744 * D2R;
	rlng = -105.27932 * D2R;

	/*********************************************************************
						   Generate b) Figure Data
				   Variation of radio noise with Frequency
	**********************************************************************/

	for (int m = 0; m < 12; m += 3) {

		// Read in the atmospheric coefficients for the particular month.
		// The subroutine dllReadFamDud() is from P372.dll
		retval = dllReadFamDud(&noiseP, datafilepath, m);
		if (retval != RTN_READFAMDUDOK) {
			return retval;
		};

		for (int h = 0; h <= 23; h += 4) { // hour local time

			sprintf(outputfilename, "%sb_%0dm%0dh.csv", bcsvfilepath, m + 1, h);
			fp = fopen(outputfilename, "w");
			if (fp == NULL) {
				printf("ITURNoise: Error: Can't open output file %s (%s)\n", outputfilename, strerror(errno));
				return RTN_ERRCANTOPENFILE;
			};

			// The file is open proceed
			// User feedback
			printf("Writing file %s\n", outputfilename);

			// Write the column headings to the file
			fprintf(fp, "month,hour,freq,latitude,longitude,Fam5,Fam10,Fam20,Fam30,Fam40,Fam50,Fam60,Fam70,Fam80,Fam90,Fam100\n");

			for (int f = 0; f < fn; f++) { //
				for (int F1 = 0; F1 <= 10; F1++) {

					// Fam1MHz range 10 to 100 dB by 10 dB except the first
					if (F1 == 0) {
						Fam1MHz = 5.0;
					}
					else {
						Fam1MHz = F1 * 10.0;
					};

					// Set the time block to the current local time, the h loop
					FamS.tmblk = (int)(h / 4.0); // Set the timeblock to the correct 4 hour block

					//*** Code from AtmosphericNoise() **********************************

					// Determine if the reciever latitude is positive or negative
					if (rlat < 0) {
						i = FamS.tmblk + 6; // TIMEBLOCKINDX=TIMEBLOCKINDX+6
					}
					else {
						i = FamS.tmblk; // TIMEBLOCKINDX=TIMEBLOCKINDX
					};

					// for K = 0 then U1 = -0.75
					// for K = 1 then U1 = U
					u[0] = -0.75;
					u[1] = (8.0 * pow(2.0, log10(f_log[f])) - 11.0) / 4.0; // U = (8. * 2.**X - 11.)/4. where X = ALOG10(FREQ)
					// Please See Page 5
					// NBS Tech Note 318 Lucas and Harper
					// "A Numerical Representation of CCIR Report 322 High Frequeny (3-30 Mc/s) Atmospheric Radio Noise Data"
					for (int k = 0; k < 2; k++) {
						pz = u[k] * noiseP.fam[i][0] + noiseP.fam[i][1]; // PZ = U1*FAM(1,TIMEBLOCKINDX) + FAM(2,TIMEBLOCKINDX)
						px = u[k] * noiseP.fam[i][7] + noiseP.fam[i][8]; // PX = U1*FAM(8,TIMEBLOCKINDX) + FAM(9,TIMEBLOCKINDX)

						for (int j = 2; j < 7; j++) {
							pz = u[k] * pz + noiseP.fam[i][j];			// PZ = U1*PZ + FAM(I,TIMEBLOCKINDX)
							px = u[k] * px + noiseP.fam[i][j + 7];		// PX = U1*PX + FAM(I+7,TIMEBLOCKINDX)
						}; // j=2,6

						if (k == 0) {
							cz = Fam1MHz * (2.0 - pz) - px;
							// U1 = U
						};
					}; // k=0,1

					// Frequency variation of atmospheric noise
					Fam[F1] = cz * pz + px;
					//*** Code from AtmosphericNoise() **********************************

				}; // End Fam1MHz loop

				// Print out all Fam1MHz data for this frequency
				fprintf(fp, "%d, %d, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f\n", 
					m+1,h,f_log[f],rlat*R2D,rlng*R2D,Fam[0],Fam[1],Fam[2],Fam[3],Fam[4],Fam[5],Fam[6],Fam[7],Fam[8],Fam[9],Fam[10]);
			
			}; // End f loop
		}; // End h loop

		fclose(fp);

	}; // End m loop

	// User feedback
	printf("ITURNoise: Data for b) Figures Complete\n");
	printf("\nBegin Data Generation for c) Figures\n");

	/*********************************************************************
						   Generate c) Figure Data
					Data Noise Variability and Character
	**********************************************************************/
	
	for (int m = 0; m < 12; m += 3) {

		// Set the season index, s, for the c and d arrays
		s = m / 3;

		// Read in the atmospheric coefficients for the particular month.
		// The subroutine dllReadFamDud() is from P372.dll
		retval = dllReadFamDud(&noiseP, datafilepath, m);
		if (retval != RTN_READFAMDUDOK) {
			return retval;
		};

		for (int h = 0; h <= 23; h += 4) { // hour local time

			// Set the time block index, tb, for the c and d arrays
			tb = h / 4;

			sprintf(outputfilename, "%sc_%0dm%0dh.csv", ccsvfilepath, m + 1, h);
			fp = fopen(outputfilename, "w");
			if (fp == NULL) {
				printf("ITURNoise: Error: Can't open output file %s (%s)\n", outputfilename, strerror(errno));
				return RTN_ERRCANTOPENFILE;
			};

			// The file is open proceed
			// User feedback
			printf("Writing file %s\n", outputfilename);

			// Write the column headings to the file
			fprintf(fp, "month,hour,freq,latitude,longitude,FaA,DuA,DlA,sigmaFaA,sigmaDuA,sigmaDlA,V_d,sigma_V_d\n");

			for (int f = 0; f < fn; f++) { //

				// Call the AtmosphericNoise_LT() from the P372.dll
				// Which calculates the atmospheric noise and returns the full statistics. 
				dllAtmosphericNoise_LT(&noiseP, &FamS, h, rlng, rlat, f_log[f]);

				// Determine V_d and sigma_V_d
				FindV_d(f_log[f], c[s][tb], d[s][tb], &V_d, &sigma_V_d);

				// Write the data out to the file
				fprintf(fp, "%d, %d, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f\n", 
					m+1, h, f_log[f], rlat* R2D, rlng* R2D, FamS.FA, FamS.Du, FamS.Dl, FamS.SigmaFam, FamS.SigmaDu, FamS.SigmaDl, V_d, sigma_V_d);

			}; // End frequency loop

			fclose(fp);

		}; // End hour for
	}; // End month for
	
	/****************** End Generate c) Figure Data ***************/

	fclose(fp_V_d);
	fclose(fp_sigma_V_d);

	// User feedback
	printf("ITURNoise: Data for c) Figures Complete\n");
	printf("\n*** End ITURNoise Data Generation ***\n");

	FreeLibrary(hLib);

	return RTN_ATMOSFILESOK;
	   
	 // End P372.DLL Load ************************************************
};

void PrintUsage() {

	/*
		PrintUsage - Prints a brief summary of how to use ITURNoise()

			INPUT
				none

			OUTPUT
				none

	*/
	printf("******************************************************************************\n");
	printf("\t\tITU-R Study Group 3: Radiowave Propagation\n");
	printf("******************************************************************************\n");
	printf("\n");
	printf("USEAGE: ITURNoise [month] [hour] [frequency] [latitude] [longitude]\n");
	printf("\t\t[man-made noise] [data file path] [print flag]\n");
	printf("\n");
	printf("\tArgument 1:  month (1 to 12)\n");
	printf("\tArgument 2:  hour (1 to 24 (UTC))\n");
	printf("\tArgument 3:  frequency (0.01 to 30.0 (MHz))\n");
	printf("\tArgument 4:  latitude (degrees decimal)\n");
	printf("\tArgument 5:  longitude (degrees decimal)\n");
	printf("\tArgument 6:  man-made noise 0-5 or value of man-made noise (- dB)\n");
	printf("\t\t0.0\tCity\n");
	printf("\t\t1.0\tResidential\n");
	printf("\t\t2.0\tRural\n");
	printf("\t\t3.0\tQuiet Rural\n");
	printf("\t\t4.0\tNoisy\n");
	printf("\t\t5.0\tQuiet\n");
	printf("\tArgument 7:  data file path within double quotes without trailing back slash\n");
	printf("\tArgument 8:  print flag 0-4 [Optional]\n");
	printf("\t\t0\tNo Output\n");
	printf("\t\t1\tPrint Detailed output to stdout\n");
	printf("\t\t2\tPrint Detailed outpout to file .\\MakeNoiseOut.txt\n");
	printf("\t\t3\tPrint CSV output with Header\n");
	printf("\t\t4\tPrint CSV output without Header\n");
	printf("\n");
	printf("Example: ITURNoise 1 14 1.0 40.0 165.0 0 \"G:\\User\\Data\" 1\n");
	printf("\t\tCalculation made for January 14th hour (UTC) \n");
    printf("\t\tat 40 degrees North and 165 degrees East\n");
	printf("\t\tfor 1.0 MHz print detailed output to stdout\n");
	printf("\n");
	printf("******************************************************************************\n");

	return;

};

void PrintCSVHeader(const char* P372ver, const char* P372compt) {
	/*
	PrintCSVHeader - Prints a summary of the CSV output columns 

		INPUT
			none

		OUTPUT
			none

	*/

	struct tm* ntime;
	time_t tm;

	char ntimestr[64];

	// Get the time to time stamp the output files.
	tm = time(NULL);
	ntime = localtime(&tm);
	sprintf(ntimestr, "%d/%d/%d - %02d:%02d:%02d",
		ntime->tm_mday, ntime->tm_mon + 1, ntime->tm_year - 100,
		ntime->tm_hour, ntime->tm_min, ntime->tm_sec);

	printf("******************************************************************************\n");
	printf("\t\tITU-R Study Group 3: Radiowave Propagation\n");
	printf("******************************************************************************\n");
	printf("\t\tAnalysis: %s\n", ntimestr);;
	printf("\t\tP372 Version:      %s\n", P372ver);
	printf("\t\tP372 Compile Time: %s\n", P372compt);
	printf("******************************************************************************\n");

	printf("Column 1:  Month\n");
	printf("Column 2:  Hour (UTC)\n");
	printf("Column 3:  Frequency (MHz)\n");
	printf("Column 4:  Latitude (deg)\n");
	printf("Column 5:  Longitude (deg)\n");
	printf("Column 6:  [FaA]  Noise Component (Atmospheric)\n");
	printf("Column 7:  [DuA]  Upper Decile    (Atmospheric)\n");
	printf("Column 8:  [DlA]  Upper Decile    (Atmospheric)\n");
	printf("Column 9:  [FaM]  Noise Component    (Man-Made)\n");
	printf("Column 10: [DuM]  Upper Decile       (Man-Made)\n");
	printf("Column 11: [DlM]  Lower Decile       (Man-Made) \n");
	printf("Column 12: [FaG]  Noise Component    (Galactic) \n");
	printf("Column 13: [DuG]  Upper Decile       (Galactic) \n");
	printf("Column 14: [DlG]  Lower Decile       (Galactic) \n");
	printf("Column 15: [FamT] Noise                 (Total)\n");
	printf("Column 16: [DuT]  Upper Decile          (Total)\n");
	printf("Column 17: [DlT]  Lower Decile          (Total)\n");
	printf("******************************************************************************\n");

	return;

};

void PrintCSVLine(int month, int hour, double freq, double rlat, double rlng, double* out) {
	/*
	PrintUsage - Prints a CSV line to a file

		INPUT
			FILE *fp		Pointer to a file
			int month		Month index (0 to 11)
			int hour		Hour inde (0 to 23)
			double freq		Frequency (MHz)
			double rlat		Latitude (radians)
			double rlng		Longitude (radians)
			double * out    Pointer to a 12 element array that contains the P372 calculation

		OUTPUT
			none

	*/

	printf("%d, %d, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f\n", month + 1, hour, freq, rlat * R2D, rlng * R2D, out[0], out[1], out[2], out[3], out[4], out[5], out[6], out[7], out[8], out[9], out[10], out[11]);

	return;

};

void FindV_d(double freq, double c[5], double d[5], double *V_d, double *sigma_V_d) {

	/*
	
		FindV_d - Calculates (30) and (31) from NTIA Report 85-173
		          "Atmospheric Radio Noise: Worldwide Levels and Other Characteristics"

			INPUT
				freq	frequency (MHz) 
				c		5 coefficients for (30) for a given time block and season
				d		5 coefficients for (31) for a given time block and season

			OUTPUT 
				V_d			Voltage deviation, which is the ratio of the 
							root mean square envelope voltage to the average noise envelope voltage
				sigma_V_d	The std deviation of V_d

	*/

	double x, x2, x3, x4;

	x = log10(freq);

	x2 = x * x;
	x3 = x2 * x;
	x4 = x3 * x;

	*V_d = c[0] + c[1] * x + c[2] * x2 + c[3] * x3 + c[4] * x4;

	*sigma_V_d = d[0] + d[1] * x + d[2] * x2 + d[3] * x3 + d[4] * x4;

	return;

};

