#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Local includes
#include "Common.h"
#include "Noise.h"

// Local Prototypes
void PrintFam(struct NoiseParams* noiseP, int month, int hour, double lng, double lat, double freq);
// End Local Prototypes

int __stdcall MakeNoise(int month, int hour, double lat, double lng, double freq, double mmnoise, char* datafilepath, double* out, int pntflag) {

	/*

		MakeNoise - This program is a stand alone method to use the P.372-14 method

			INPUT
				int month
				int hour
				double lat
				double lng
				double freq
				double mmnoise
				char* datafilepath
				int pntflag
				
			OUTPUT
				double* out           Pointer to an array of 12 doubles 

			
	*/

	// End P372.DLL Load ************************************************

	int retval;

	const char* P372ver;
	const char* P372compt;

	struct NoiseParams noiseP;

	struct tm* ntime;
	time_t tm;

	char ntimestr[64];

	// Allocate the memory in the noise structure
	retval = AllocateNoiseMemory(&noiseP);
	if (retval != RTN_ALLOCATEP372OK) {
		return RTN_ERRALLOCATENOISE;
	};

	// Initialize Noise from the P372.dll
	InitializeNoise(&noiseP);
	// End Initialize Noise

	// Load the version and compile time of the P372.DLL
	P372ver = P372Version();
	P372compt = P372CompileTime();

	// Get the time to time stamp the output files.
	tm = time(NULL);
	ntime = localtime(&tm);
	sprintf(ntimestr, "%d/%d/%d - %02d:%02d:%02d",
		ntime->tm_mday, ntime->tm_mon + 1, ntime->tm_year - 100,
		ntime->tm_hour, ntime->tm_min, ntime->tm_sec);

	// Read in the atmospheric coefficients for the particular month.
	// The subroutine dllReadFamDud() is from P372.dll
	retval = ReadFamDud(&noiseP, datafilepath, month);
	if (retval != RTN_READFAMDUDOK) {
		return retval;
	};

	noiseP.ManMadeNoise = mmnoise;

	// Call noise from the P372.dll
	retval = Noise(&noiseP, hour, lng, lat, freq);
	if (retval != RTN_NOISEOK) return retval; // check that the input parameters are correct

	*out = noiseP.FaA;
	*(out + 1) = noiseP.DuA;
	*(out + 2) = noiseP.DlA;
	*(out + 3) = noiseP.FaM;
	*(out + 4) = noiseP.DuM;
	*(out + 5) = noiseP.DlM;
	*(out + 6) = noiseP.FaG;
	*(out + 7) = noiseP.DuG;
	*(out + 8) = noiseP.DlG;
	*(out + 9) = noiseP.FamT;
	*(out + 10) = noiseP.DuT;
	*(out + 11) = noiseP.DlT;

	// Check to see if there is a pntflag
	if ((pntflag != NOPRINT) && (pntflag != PRINTHEADER)) {
		pntflag = NOPRINT;
	};

	if (pntflag == PRINTHEADER) {
		printf("******************************************************************************\n");
		printf("\t\tITU-R Study Group 3: Radiowave Propagation\n");
		printf("******************************************************************************\n");
		printf("\t\tAnalysis: %s\n", ntimestr);;
		printf("\t\tP372 Version:      %s\n", P372ver);
		printf("\t\tP372 Compile Time: %s\n", P372compt);
		printf("******************************************************************************\n");
	};

	if (pntflag != NOPRINT) {
		PrintFam(&noiseP, month, hour, lng, lat, freq);
		printf("******************************************************************************\n");
	};

	return RTN_MAKENOISEOK;

};


void PrintFam(struct NoiseParams* noiseP, int month, int hour, double lng, double lat, double freq) {
	
	const char* monthnames[] = { "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };
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

	return;
};

