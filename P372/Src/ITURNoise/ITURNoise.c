#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Local includes
#include "Common.h"
#include "ITURNoise.h"
// End Local includes

// Local Prototypes
void PrintFam(struct NoiseParams* noiseP, int month, int hour, double lng, double lat, double freq);
// End Local Prototypes

int main(int argc, char* argv[]) {

	int pntflag = SILENT;

	int month;
	int hour;
	int retval;

	double lat;
	double lng;
	double freq;
	double mmnoise;
	double out[12];

	char datafilepath[256];

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
		if ((lat < -90.0) && (lat > 90.0)) {
			printf("ITURNoise: Error: Latitude (%5.4f (degrees)) Out of Range (-90 to 90 degrees) ", lat);
			return RTN_ERRMONTH;
		}
		else {
			lat = lat * D2R;
		};

		lng = atof(argv[5]);
		if ((lat < -180.0) && (lat > 180.0)) {
			printf("ITURNoise: Error: Longitude (%5.4f (degrees)) Out of Range (-180 to 180 degrees) ", lat);
			return RTN_ERRMONTH;
		}
		else {
			lng = lng * D2R;
		};

		mmnoise = atof(argv[6]); // 

		sprintf(&datafilepath[0], "%s\\", argv[7]);

		if (argc >= 8) {
			pntflag = atoi(argv[8]);
		};

		retval = ITURNoise(month, hour, lat, lng, freq, mmnoise, datafilepath, &out[0], pntflag);

		if(retval == RTN_ITURNOISEOK) {
			if (pntflag == CSVOUTPUTNHEADER) {

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

			};
			if ((pntflag == CSVOUTPUT) || (pntflag == CSVOUTPUTNHEADER)) {
				printf("%d, %d, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f, %5.4f\n", month + 1, hour + 1, lat * R2D, lng * R2D, out[0], out[1], out[2], out[3], out[4], out[5], out[6], out[7], out[8], out[9], out[10], out[11]);
			};
		}
	}
	else {

		printf("ITURNoise: ERROR: Insufficient number (%d) of command line arguments, 7 required.\n", argc);
		printf("\n");
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
		printf("\t                 ITUHEADER        0 Prints the full header\n");
		printf("\t                 CSVOUTPUT		1 Prints out simple csv\n");
		printf("\t                 CSVOUTPUTNHEADER	2 Print out header with the simple csv\n");
		printf("\t                 SILENT			3 Do not print\n");
		printf("\n");
		printf("Example: ITURNoise 1 14 1.0 40.0 165.0 0 \"G:\\User\\Data\" \n");
		printf("                   Calculation made for January 14th hour (UTC)\n");
		printf("                   at 40 degrees North and 165 degrees East\n");
		printf("\n");
		return RTN_ERRCOMMANDLINEARGS;

	};
 
return retval;

};

int ITURNoise(int month, int hour, double lat, double lng, double freq, double mmnoise, char * datafilepath, double * out, int pntflag) {
	
	/*
	
		ITURNoise - 
	
	
	*/
	
	
	// P372.dll **********************************************************

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
	dllReadFamDud = (iReadFamDud)GetProcAddress((HMODULE)hLib, "ReadFamDud");
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

	// End P372.DLL Load ************************************************

	int retval;

	const char* P372ver;
	const char* P372compt;

	struct NoiseParams noiseP;

	struct tm* ntime;
	time_t tm;

	char ntimestr[64];
	
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
	sprintf(ntimestr, "%d/%d/%d - %02d:%02d:%02d",
		ntime->tm_mday, ntime->tm_mon + 1, ntime->tm_year - 100,
		ntime->tm_hour, ntime->tm_min, ntime->tm_sec);

	// Read in the atmospheric coefficients for the particular month.
	// The subroutine dllReadFamDud() is from P372.dll
	retval = dllReadFamDud(&noiseP, datafilepath, month);
	if (retval != RTN_READFAMDUDOK) {
		return retval;
	};

	noiseP.ManMadeNoise = mmnoise;

	// Call noise from the P372.dll
	retval = dllNoise(&noiseP, hour, lng, lat, freq);
	if (retval != RTN_NOISEOK) return retval; // check that the input parameters are correct

	*out      = noiseP.FaA;
	*(out+1)  = noiseP.DuA;
	*(out+2)  = noiseP.DlA;
	*(out+3)  = noiseP.FaM;
	*(out+4)  = noiseP.DuM;
	*(out+5)  = noiseP.DlM;
	*(out+6)  = noiseP.FaG;
	*(out+7)  = noiseP.DuG;
	*(out+8)  = noiseP.DlG;
	*(out+9) = noiseP.FamT;
	*(out+10) = noiseP.DuT;
	*(out+11) = noiseP.DlT;

	// Check to see if there is a pntflag
	if ((pntflag != SILENT) && (pntflag != ITUHEADER) && (pntflag != CSVOUTPUTNHEADER)) {
		pntflag = SILENT;
	};

	if ((pntflag == ITUHEADER) || (pntflag == CSVOUTPUTNHEADER)) {
		printf("******************************************************************************\n");
		printf("\t\tITU-R Study Group 3: Radiowave Propagation\n");
		printf("******************************************************************************\n");
		printf("\t\tAnalysis: %s\n", ntimestr);;
		printf("\t\tP372 Version:      %s\n", P372ver);
		printf("\t\tP372 Compile Time: %s\n", P372compt);
		printf("******************************************************************************\n");
	};

	if (pntflag == ITUHEADER) {
		PrintFam(&noiseP, month, hour, lng, lat, freq);
		printf("******************************************************************************\n");
	};
	
	return RTN_ITURNOISEOK;

};


void PrintFam(struct NoiseParams* noiseP, int month, int hour, double lng, double lat, double freq) {	
	const char* monthnames[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
	printf("\n");
	printf("\t%s : %d (UTC) at %f (deg lat) %f (deg long)\n", monthnames[month], hour, lat * R2D, lng * R2D);
	printf("\t[FaA]  Noise Component (Atmospheric): %f\n", noiseP->FaA);
	printf("\t[DuA]  Upper Decile    (Atmospheric): %f\n", noiseP->DuA);
	printf("\t[DlA]  Upper Decile    (Atmospheric): %f\n", noiseP->DlA);
	printf("\t[FaM]  Noise Component    (Man-Made): %f\n", noiseP->FaM);
	printf("\t[DuM]  Upper Decile       (Man-Made): %f\n", noiseP->DuM);
	printf("\t[DlM]  Lower Decile       (Man-Made): %f\n", noiseP->DlM);
	printf("\t[FaG]  Noise Component    (Galactic): %f\n", noiseP->FaG);
	printf("\t[DuG]  Upper Decile       (Galactic): %f\n", noiseP->DuG);
	printf("\t[DlG]  Lower Decile       (Galactic): %f\n", noiseP->DlG);
	printf("\t[FamT] Noise                 (Total): %f\n", noiseP->FamT);
	printf("\t[DuT]  Upper Decile          (Total): %f\n", noiseP->DuT);
	printf("\t[DlT]  Lower Decile          (Total): %f\n", noiseP->DlT);
	printf("\n");
};