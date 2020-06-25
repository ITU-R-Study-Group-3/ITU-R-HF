#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

// Local includes
#include "Common.h"
#include "Noise.h"
#include "ITURNoise.h"
// End Local includes

#define PRINTALL 100
#define PRINTCSV 101

// Local Prototypes
void PrintCSVLine(int month, int hour, double lat, double lng, double* out);
void PrintCSVHeader(const char* P372ver, const char* P372compt);
void PrintUsage();
void WriteCSVLine(FILE* fp, int month, int hour, double freq, double lat, double lng, struct FamStats* FamS);
void PrintCSVLine(int month, int hour, double lat, double lng, double* out);
int RunAtmosNoiseMonths(char* datafilepath);
// End Local Prototypes

int main(int argc, char* argv[]) {
/*

	main() - This program takes command line arguements and runs the P372 calculation engine
             
		INPUT
			Argument 1:  month (1 to 12)\n");
			Argument 2:  hour (1 to 24 (UTC))\n");
			Argument 3:  latitude (degrees)\n");
			Argument 4:  longitude (degrees)\n");
			Argument 5:  man-made noise 0-5 or value of man-made noise (dB)\n");
			                 CITY         0.0\n");
			                 RESIDENTIAL  1.0\n");
			                 RURAL		2.0\n");
			                 QUIETRURAL	3.0\n");
			                 NOISY		4.0\n");
			                 QUIET		5.0\n");
			Argument 6:  data file path in double quotes without trailing back slash\n");
			Argument 7:  print flag 0-3\n");
			                 PRINTHEADER	1 Prints the full header\n");
			                 PRINTCSV		3 Prints out simple csv\n");
			                 PRINTALL		2 Print out header with the simple csv\n");
			                 NOPRINT		0 Do not print\n");
		OUTPUT
			The output is printed to stdout depending on the print flag

		Example: ITURNoise 1 14 1.0 40.0 165.0 0 \"G:\\User\\Data\" \n");
		                   Calculation made for January 14th hour (UTC)\n");
		                   at 40 degrees North and 165 degrees East\n");

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

	Behm/2020

*/

	int pntflag = PRINTALL;
	int mnpntflag = PRINTHEADER;

	int month;
	int hour;
	int retval;

	double lat;
	double lng;
	double freq;
	double mmnoise;
	double out[12];

	char datafilepath[256];

	const char* P372ver;
	const char* P372compt;

	if (argc == 2) {

		// Then the user wants to run all the tables necessary to create the
		// Atmospheric noise Figures in Recommendation P.372-14 
		// Assume that the only argument given is the path to the data files
		sprintf(&datafilepath[0], "%s\\", argv[1]);

		RunAtmosNoiseMonths(&datafilepath[0]);

		return RTN_ITURNOISEOK;

	}; 

	// Are there enough command line arguments to proceed?
	if (argc > 6) {
		month = atoi(argv[1]) - 1;
		if ((month < 0) && (month > 11)) {
			printf("ITURNoise: Error: Month (%d) Out of Range (1 to 12) ", month + 1);
			return RTN_ERRMONTH;
		};

		hour = atoi(argv[2]) - 1;
		if ((hour < 0) && (hour > 23)) {
			printf("ITURNoise: Error: Hour (%d (UTC)) Out of Range (1 to 24 UTC) ", hour + 1);
			return RTN_ERRMONTH;
		};

		freq = atof(argv[3]);
		if ((freq < 0.01) && (freq > 30)) {
			printf("ITURNoise: Error: Frequency (%5.4f (MHz)) Out of Range (0.01 to 30 MHz) ", freq);
			return RTN_ERRMONTH;
		};

		lat = atof(argv[4]);
		if ((lat < -90.0) || (lat > 90.0)) {
			printf("ITURNoise: Error: Latitude (%5.4f (degrees)) Out of Range (-90 to 90 degrees) ", lat);
			return RTN_ERRMONTH;
		}
		else {
			lat = lat * D2R;
		};

		lng = atof(argv[5]);
		if ((lng < -180.0) || (lng > 180.0)) {
			printf("ITURNoise: Error: Longitude (%5.4f (degrees)) Out of Range (-180 to 180 degrees) ", lng);
			return RTN_ERRMONTH;
		}
		else {
			lng = lng * D2R;
		};

		mmnoise = atof(argv[6]); // 

		sprintf(&datafilepath[0], "%s\\", argv[7]);

		if (argc >= 8) {
			pntflag = atoi(argv[8]);
			if (pntflag == 1) {
				pntflag = PRINTHEADER;
			}
			else if (pntflag == 2) {
				pntflag = PRINTCSV;
			}
			else if (pntflag == 4) {
				pntflag = PRINTBLOCK;
			}
			else if (pntflag == 0) {
				pntflag = NOPRINT;
			}
			else if (pntflag == 3) {
				pntflag = PRINTCSVALL;
			}
		};

		if (pntflag == PRINTALL) {
			mnpntflag = PRINTHEADER;
		};

	// Load the Noise routines in P372.dll ******************************
#ifdef _WIN32
		int mod[512];
		// Get the handle to the P372 DLL.
		hLib = LoadLibrary("P372.dll");
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
		dllMakeNoise = (iMakeNoise)GetProcAddress((HMODULE)hLib, "_MakeNoise@52");
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

		if(retval == RTN_MAKENOISEOK) {
			if (pntflag == PRINTCSVALL) {
				PrintCSVHeader(P372ver, P372compt);
			};
			if (pntflag == PRINTCSV) {
				PrintCSVLine(month, hour, lat, lng, &out[0]);
			};
		}
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
					P.372-14 Figures of atmospheric noise
	
	*/


	// Load the Noise routines in P372.dll ******************************
#ifdef _WIN32
	// Get the handle to the P372 DLL.
	hLib = LoadLibrary("P372.dll");
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

	FILE* fp;

	int retval;
	int fn = 41; // Number of elements in the f_log array below

	const char* P372ver;
	const char* P372compt;

	struct NoiseParams noiseP;
	struct FamStats FamS;

	double freq;
	double rlat, rlng;

	// Frequency array that will be used for b) and c) Figure data generation 
	double f_log[41] = { 0.01, 0.015, 0.02, 0.025, 0.03, 0.35, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09,
		0.1, 0.15, 0.02, 0.25, 0.3, 0.35, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9,
		1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0,
		10.0, 15.0, 20.0, 25.0, 30.0};

	struct tm* ntime;
	time_t tm;

	char ntimestr[64];

	char outputfilename[256];

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
			
		for (int h = 1; h <= 23; h+=4) { // hour local time
	
			sprintf(outputfilename, ".\\A_%0dm%0dh-%s.csv", m+1, h, ntimestr);
			fp = fopen(outputfilename, "w");
			if (fp == NULL) {
				printf("ITURNoise: Error: Can't open output file %s (%s)\n", outputfilename, strerror(errno));
				return RTN_ERRCANTOPENFILE;
			};
			
			// The file is open proceed
			// User feedback
			printf("Writing file %s\n", outputfilename);

			// Write the column headings to the file
			fprintf(fp, "month,hour,freq,latitude,longitude,FaA,DuA,DlA,sigmaFaA,sigmaDuA,sigmaDlA\n");
			
			for (int ilat = -90; ilat <= 90; ilat++) { //
				rlat = ilat * D2R;
				for (int ilng = -180; ilng <= 180; ilng++) { //
					rlng = ilng * D2R;
					
					// Call the AtmosphericNoise_LT() from the P372.dll
					// Which calculates the atmospheric noise and returns the full statistics. 
					dllAtmosphericNoise_LT(&noiseP, &FamS, h, rlng, rlat, freq);
										
					// Write the data out to the file
					WriteCSVLine(fp, m, h, freq, rlat, rlng, &FamS);

				}; //  End Longitude loop
			}; // End Latitude loop

			fclose(fp);

		}; // End hour for
	}; // End month for

	/********************* End Generate a) Figure Data *******************/

	
	// User feedback
	printf("ITURNoise: Data for a) Figures Complete\n");
	printf("\nBegin Data Generation for b) and c) Figures\n");

	/*********************************************************************
						   Generate b) Figure Data
		           Variation of radio noise with Frequency
				                     and
						   Generate c) Figure Data
					Data Noise Variability and Character
	**********************************************************************/

	// We need one location for this calculation
	//             Boulder, Colorado
	rlat = 40.015744 * D2R;
	rlng = -105.27932 * D2R;

	for (int m = 0; m < 12; m += 3) {

		// Read in the atmospheric coefficients for the particular month.
		// The subroutine dllReadFamDud() is from P372.dll
		retval = dllReadFamDud(&noiseP, datafilepath, m);
		if (retval != RTN_READFAMDUDOK) {
			return retval;
		};

		for (int h = 1; h <= 23; h += 4) { // hour local time

			sprintf(outputfilename, ".\\BnC_%0dm%0dh-%s.csv", m + 1, h, ntimestr);
			fp = fopen(outputfilename, "w");
			if (fp == NULL) {
				printf("ITURNoise: Error: Can't open output file %s (%s)\n", outputfilename, strerror(errno));
				return RTN_ERRCANTOPENFILE;
			};

			// The file is open proceed
			// User feedback
			printf("Writing file %s\n", outputfilename);

			// Write the column headings to the file
			fprintf(fp, "month,hour,freq,latitude,longitude,FaA,DuA,DlA,sigmaFaA,sigmaDuA,sigmaDlA\n");

			for (int f = 0; f < fn; f++) { //

				// Call the AtmosphericNoise_LT() from the P372.dll
				// Which calculates the atmospheric noise and returns the full statistics. 
				dllAtmosphericNoise_LT(&noiseP, &FamS, h, rlng, rlat, f_log[f]);

				// Write the data out to the file
				WriteCSVLine(fp, m, h, f_log[f], rlat, rlng, &FamS);

			}; // End frequency loop

			fclose(fp);

		}; // End hour for
	}; // End month for

	/****************** End Generate b) and c) Figure Data ***************/

	// User feedback
	printf("ITURNoise: Data for b) and c) Figures Complete\n");
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
	printf("USEAGE ITURNoise [month] [hour] [latitude] [longitude] [man-made noise] [data file path] [print flag]\n");
	printf("\tArgument 1:  month (1 to 12)\n");
	printf("\tArgument 2:  hour (1 to 24 (UTC))\n");
	printf("\tArgument 3:  latitude (degrees)\n");
	printf("\tArgument 4:  longitude (degrees)\n");
	printf("\tArgument 5:  man-made noise 0-5 or value of man-made noise (dB)\n");
	printf("\t                 CITY         0.0\n");
	printf("\t                 RESIDENTIAL  1.0\n");
	printf("\t                 RURAL		2.0\n");
	printf("\t                 QUIETRURAL	3.0\n");
	printf("\t                 NOISY		4.0\n");
	printf("\t                 QUIET		5.0\n");
	printf("\tArgument 6:  data file path in double quotes without trailing back slash\n");
	printf("\tArgument 7:  print flag 0-3\n");
	printf("\t                 PRINTHEADER 0 Prints the full header\n");
	printf("\t                 PRINTCSV	   1 Prints out simple csv\n");
	printf("\t                 PRINTALL	   2 Print out header with the simple csv\n");
	printf("\t                 NOPRINT	   3 Do not print\n");
	printf("\n");
	printf("Example: ITURNoise 1 14 1.0 40.0 165.0 0 \"G:\\User\\Data\" \n");
	printf("                   Calculation made for January 14th hour (UTC)\n");
	printf("                   at 40 degrees North and 165 degrees East\n");
	printf("\n");

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

	printf("Column 1: Month\n");
	printf("Column 2: Hour (UTC)\n");
	printf("Column 3: Latitude (deg)\n");
	printf("Column 4: Longitude (deg)\n");
	printf("Column 5: [FaA]  Noise Component (Atmospheric)\n");
	printf("Column 6: [DuA]  Upper Decile    (Atmospheric)\n");
	printf("Column 7: [DlA]  Upper Decile    (Atmospheric)\n");
	printf("Column 8: [FaM]  Noise Component    (Man-Made)\n");
	printf("Column 9: [DuM]  Upper Decile       (Man-Made)\n");
	printf("Column 10:[DlM]  Lower Decile       (Man-Made) \n");
	printf("Column 11:[FaG]  Noise Component    (Galactic) \n");
	printf("Column 12:[DuG]  Upper Decile       (Galactic) \n");
	printf("Column 13:[DlG]  Lower Decile       (Galactic) \n");
	printf("Column 2: [FamT] Noise                 (Total)\n");
	printf("Column 2: [DuT]  Upper Decile          (Total)\n");
	printf("Column 2: [DlT]  Lower Decile          (Total)\n");
	printf("******************************************************************************\n");

	return;

};

void PrintCSVLine(int month, int hour, double lat, double lng, double* out) {
	/*
	PrintUsage - Prints a CSV line to a file

		INPUT
			FILE *fp		Pointer to a file
			int month		Month index (0 to 11)
			int hour		Hour inde (0 to 23)
			double lat		Latitude (radians)
			double lng		Longitude (radians)
			double * out    Pointer to a 12 element array that contains the P372 calculation

		OUTPUT
			none

	*/

	printf("%d, %d, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f\n", month + 1, hour, lat * R2D, lng * R2D, out[0], out[1], out[2], out[3], out[4], out[5], out[6], out[7], out[8], out[9], out[10], out[11]);

	return;

};

void WriteCSVLine(FILE* fp, int month, int hour, double freq, double lat, double lng, struct FamStats *FamS) {
	/*
	PrintUsage - Prints a CSV line to a file

		INPUT
			FILE *fp		Pointer to a file
			int month		Month index (0 to 11)
			int hour		Hour inde (0 to 23)
			double lat		Latitude (radians)
			double lng		Longitude (radians)
			double * out    Pointer to a 12 element array that contains the P372 calculation

		OUTPUT
			none

	*/

	fprintf(fp, "%d, %d, %3.2f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f\n", month + 1, hour + 1, freq, lat * R2D, lng * R2D, FamS->FA, FamS->Du, FamS->Dl, FamS->SigmaFam, FamS->SigmaDu, FamS->SigmaDu);

	return;

};