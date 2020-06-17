#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Local includes
#include "Common.h"
#include "ITURNoise.h"
// End Local includes

// Local Prototypes
int ITURNoise(int month, int hour, double lat, double lng, double freq, double mmnoise, char* datafilepath, double* out);
void PrintFam(struct NoiseParams* noiseP, int month, int hour, double lng, double lat, double freq);
// End Local Prototypes

int main(int argc, char* argv[]) {

	boolean pntflag = FALSE;

	int month;
	int hour;
	int retval;

	double lat;
	double lng;
	double freq;
	double mmnoise;
	double out[12];

	char* datafilepath;


	if (argc > 7) {
		month = atoi(argv[0]) - 1;
		if ((month < 0) && (month > 11)) {
			printf("ITURNoise: Error: Month (%d) Out of Range (1 to 12) ", month + 1);
			return RTN_ERRMONTH;
		};

		hour = atoi(argv[1]) - 1;
		if ((hour < 0) && (hour > 23)) {
			printf("ITURNoise: Error: Hour (%d (UTC)) Out of Range (1 to 24 UTC) ", hour + 1);
			return RTN_ERRMONTH;
		};

		freq = atof(argv[2]);
		if ((freq < 0.01) && (freq > 30)) {
			printf("ITURNoise: Error: Frequency (%5.4f (MHz)) Out of Range (0.01 to 30 MHz) ", freq);
			return RTN_ERRMONTH;
		};

		lat = atof(argv[3]);
		if ((lat < -90.0) && (lat > 90.0)) {
			printf("ITURNoise: Error: Latitude (%5.4f (degrees)) Out of Range (-90 to 90 degrees) ", lat);
			return RTN_ERRMONTH;
		}
		else {
			lat = lat * D2R;
		};

		lng = atof(argv[4]);
		if ((lat < -180.0) && (lat > 180.0)) {
			printf("ITURNoise: Error: Longitude (%5.4f (degrees)) Out of Range (-180 to 180 degrees) ", lat);
			return RTN_ERRMONTH;
		}
		else {
			lng = lng * D2R;
		};

		mmnoise = atof(argv[5]); // 

		datafilepath = argv[6];

		retval = ITURNoise(month, hour, lat, lng, freq, mmnoise, datafilepath, out, pntflag);

	}
	else {

		printf("ITURNoise: ERROR: Insufficient number (%d) of command line arguments. \n", argc);
		return RTN_ERRCOMMANDLINEARGS;

	};
 
return retval;

};

int ITURNoise(int month, int hour, double lat, double lng, double freq, double mmnoise, char * datafilepath, double * out, boolean pntflag) {
	
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
	}

	// Initialize Noise from the P372.dll
	dllInitializeNoise(&noiseP);
	// End Initialize Noise
	
	// Before moving on load the version and compile time of the P372.DLL
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

	// Call noise from the P372.dll
	retval = dllNoise(&noiseP, hour, lng, lat, freq);
	if (retval != RTN_NOISEOK) return retval; // check that the input parameters are correct

	if (pntflag == TRUE) {

		printf("******************************************************************************/n");
		printf("                  ITU-R Study Group 3: Radiowave Propagation\n");
		printf("******************************************************************************/n");
		printf("\tAnalysis: %s\n", ntimestr);;
		printf("\tP372 Version:      %s\n", P372ver);
		printf("\tP372 Compile Time: %s\n", P372compt);
		printf("******************************************************************************/n");
		PrintFam(&noiseP, month, hour, lng, lat, freq);
		printf("******************************************************************************/n");

	};
	
	return RTN_ITURNOISEOK;

};


void PrintFam(struct NoiseParams* noiseP, int month, int hour, double lng, double lat, double freq) {	
	const char* monthnames[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
	printf("\n");
	printf("ITURNoise: %s : %d (UTC) at %f (deg lat) %f (deg long)\n", monthnames[month], hour, lat * R2D, lng * R2D);
	printf("ITURNoise: [FaA]  Noise Component (Atmospheric): %f\n", noiseP->FaA);
	printf("ITURNoise: [DuA]  Upper Decile    (Atmospheric): %f\n", noiseP->DuA);
	printf("ITURNoise: [DlA]  Upper Decile    (Atmospheric): %f\n", noiseP->DlA);
	printf("ITURNoise: [FaM]  Noise Component    (Man-Made): %f\n", noiseP->FaM);
	printf("ITURNoise: [DuM]  Upper Decile       (Man-Made): %f\n", noiseP->DuM);
	printf("ITURNoise: [DlM]  Lower Decile       (Man-Made): %f\n", noiseP->DlM);
	printf("ITURNoise: [FaG]  Noise Component    (Galactic): %f\n", noiseP->FaG);
	printf("ITURNoise: [DuG]  Upper Decile       (Galactic): %f\n", noiseP->DuG);
	printf("ITURNoise: [DlG]  Lower Decile       (Galactic): %f\n", noiseP->DlG);
	printf("ITURNoise: [FamT] Noise                 (Total): %f\n", noiseP->FamT);
	printf("ITURNoise: [DuT]  Upper Decile          (Total): %f\n", noiseP->DuT);
	printf("ITURNoise: [DlT]  Lower Decile          (Total): %f\n", noiseP->DlT);
	printf("\n");
};