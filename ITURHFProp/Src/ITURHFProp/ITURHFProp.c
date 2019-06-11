#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Local includes
#include "Common.h"
#include "P533.h"
#include "ITURHFProp.h"
// End local includes

// Local prototypes
void TestPath(struct PathData *path, int n);
void AntennaType(struct PathData *path, int n);
void PathConfig(struct PathData *path, int n);
void help(void);
// End local prototypes

// Local globals
char mod[512];

#ifdef _WIN32
	HINSTANCE hLib;
	cP533Info dllP533Version;
	cP533Info dllP533CompileTime;
	iP533 dllP533;
	iPathMemory dllAllocatePathMemory;
	iPathMemory dllFreePathMemory;
	dBearing dllBearing;
	iInputDump dllInputDump;
#elif __linux__ || __APPLE__
	#include <dlfcn.h>
	void * hLib;
	char * (*dllP533Version)();
	char * (*dllP533CompileTime)();
	int (*dllP533)(struct PathData *);
	int (*dllAllocatePathMemory)(struct PathData *);
	int (*dllFreePathMemory)(struct PathData *);
	double (*dllBearing)(struct Location,struct Location);
	int (*dllInputDump)(struct PathData *);
#endif

// End local globals

int main(int argc, char *argv[]) {
	/*
	 * ITURHFProp Demonstration Program - main() - This is a dummy program whose sole purpose to pass user-simulated data to the program ITURHFProp for testing.
	 *		It is assumed that this program will be replaced with a program which gets user input via a GUI or
	 *		that the P533 DLL will be used directly. This routine acts as an example of how the P533() model is used and what has to
	 *		be loaded in order to successfully conduct an analysis.
	 *		This routine is where changes to structure ITURHFProp ITURHFP are made and then passed to ITURHFProp().
	 *
	 *			INPUT
	 *				User data for an analysis using ITURHFProp() and P533() in the input file specified on command line.
	 *
	 *			OUTPUT
	 *				Typically an output Report or a path data dump file.
	 *
	 */

	int retval;	// Return value

	struct PathData path;		// The structure that describes the path to calculate in P533().
	struct ITURHFProp ITURHFP;	// The structure that describes the number and types of runs of P533() in the user defined analysis.

	char InFilePath[256];
	char OutFileName[32];
	char OutFilePath[256];

	time_t tm;

	// Initialization
	ITURHFP.silent = FALSE;
	ITURHFP.header = TRUE;
	ITURHFP.csvRFC4180 = FALSE;
	InFilePath[0] = EMPTY;
	OutFileName[0] = EMPTY;
	OutFilePath[0] = EMPTY;
	// End Initialization

	//******************************************************************************************
	// Load P533 DLL ***************************************************************************
	//******************************************************************************************

#ifdef _WIN32
	// Get the handle to the P533 DLL.
	hLib=LoadLibrary("P533.dll");
	if(hLib==NULL) {
		printf("Main: Error %d p533.dll Not Found\n", RTN_ERRP533DLL);
		return RTN_ERRP533DLL;
	};

	// Get the handle to the DLL library, hLib.
	GetModuleFileName((HMODULE)hLib, (LPTSTR)mod, 50);
	// Get the P533Version() process from the DLL.
	dllP533Version = (cP533Info)GetProcAddress((HMODULE)hLib, "P533Version");
	// Get the P533CompileTime() process from the DLL.
	dllP533CompileTime = (cP533Info)GetProcAddress((HMODULE)hLib, "P533CompileTime");
	// Get the function P533() from the DLL.
	dllP533 = (iP533)GetProcAddress((HMODULE)hLib, "P533");
	// Get the function AllocatePathMemory() from the DLL.
	dllAllocatePathMemory = (iPathMemory)GetProcAddress((HMODULE)hLib, "AllocatePathMemory");
	// Get the function FreePathMemory() from the DLL.
	dllFreePathMemory = (iPathMemory)GetProcAddress((HMODULE)hLib, "FreePathMemory");
	// Get the function Bearing() from the DLL.
	dllBearing = (dBearing)GetProcAddress((HMODULE)hLib, "Bearing");
	// Get the function InputDump() from the DLL.
	dllInputDump = (iInputDump)GetProcAddress((HMODULE)hLib, "InputDump");
	// Get functions that have been moved into DLL as utility for PMW integration
	dllReadType11Func = (iReadType11Func)GetProcAddress((HMODULE)hLib,"ReadType11");
	dllReadType13Func = (iReadType13Func)GetProcAddress((HMODULE)hLib,"ReadType13");
	dllReadType14Func = (iReadType14Func)GetProcAddress((HMODULE)hLib,"ReadType14");
	dllIsotropicPatternFunc = (vIsotropicPatternFunc)GetProcAddress((HMODULE)hLib,"IsotropicPattern");
//	dllReadFamDudFunc = (ReadFamDudFunc)GetProcAddress((HMODULE)hLib,"ReadFamDud");
	dllReadIonParametersBinFunc = (iReadIonParametersBinFunc)GetProcAddress((HMODULE)hLib,"ReadIonParametersBin");
	dllReadIonParametersTxtFunc = (iReadIonParametersTxtFunc)GetProcAddress((HMODULE)hLib,"ReadIonParametersTxt");
	dllReadP1239Func = (iReadP1239Func)GetProcAddress((HMODULE)hLib,"ReadP1239");

#elif __linux__ || __APPLE__
	void * hLib;
	hLib = dlopen("libp533.so", RTLD_NOW);
	if (!hLib) {
		printf("Couldn't load libp533.so, exiting.\n");
		exit(1);
	}
	dllP533Version = dlsym(hLib,"P533Version");
	dllP533CompileTime = dlsym(hLib,"P533CompileTime");
	dllP533 = dlsym(hLib,"P533");
	dllAllocatePathMemory = dlsym(hLib,"AllocatePathMemory");
	dllFreePathMemory = dlsym(hLib,"FreePathMemory");
	dllInputDump = dlsym(hLib, "InputDump");
	dllBearing = dlsym(hLib,"Bearing");
	dllReadType11Func = dlsym(hLib,"ReadType11");
	dllReadType13Func = dlsym(hLib,"ReadType13");
	dllReadType14Func = dlsym(hLib,"ReadType14");
	dllIsotropicPatternFunc = dlsym(hLib,"IsotropicPattern");
	dllReadIonParametersBinFunc = dlsym(hLib,"ReadIonParametersBin");
	dllReadIonParametersTxtFunc = dlsym(hLib,"ReadIonParametersTxt");
	dllReadP1239Func = dlsym(hLib,"ReadP1239");
	//printf("%s\n",dllP533Version());
#endif

	//********************************************************************************************
	// End Load P533 DLL *************************************************************************
	//********************************************************************************************

	// Determine the P533() version of the DLL/SO.
	ITURHFP.P533ver = dllP533Version();
	// Determine the P533() compile time
	ITURHFP.P533compt = dllP533CompileTime();

	//********************************************************************************************
	// Parse Command Line ************************************************************************
	//********************************************************************************************

	// Determine the desired user options on the command line
	while ((argc > 1) && (argv[1][0] == '-')) {
		switch (argv[1][1]) {
			case 'c': //CSV OUTPUT
				ITURHFP.csvRFC4180 = TRUE;
				ITURHFP.header = FALSE;
				break;
			case 's': // Silent Mode
				ITURHFP.silent = TRUE;
				break;
			case 't': // Remove the header Mode
			  ITURHFP.header = FALSE;
				break;
			case 'v': // Version - Display version and Exit
				printf("ITURHFProp Version: %s\n", ITURHFPropVER);
				printf("P533 Version: %s\n", ITURHFP.P533ver);
				return RTN_MAINOK;
				break;
			case 'h': // Help - Display Help and Exit
				help();
				return RTN_MAINOK;
				break;
			default:
				printf("Main: Error Invalid Option: %s\n", argv[1]);
				help();
				return RTN_MAINOK;
		};

		++argv;
		--argc;
	};

	// Find the input and output files if they are on the command line
	if(argc > 1) { // argc is 2 or 3 there is an input file
		sprintf(InFilePath, "%s", argv[1]);
	};

	if(argc > 2) { // An explicite output file name has been requested
		sprintf(OutFilePath, "%s", argv[2]);
	};

	if(argc <= 1) { // There was not input file indicated on command line
		printf("Main: Error %d No Input File\n", RTN_ERRCOMMANDLINEARG);
		return RTN_ERRCOMMANDLINEARG;
	};

	//********************************************************************************************
	// End Parse Command Line ********************************************************************
	//********************************************************************************************

	//********************************************************************************************
	// Initialize the structure PathData *path for P533() and the structure ITURHFProp ITURHFP for ITURHFProp().
	// The structure PathData *path contains all the input and output variables for the P533() calculation.
	// The structure ITURHFProp ITURHFP contains all of the input for the user analysis.
	// Essentially P533() executes the propagation model while ITURHFProp() determines how many times and what
	// type of analysis is desired by the user. In this way the ITUFHProp() manages the execution of P533().
	// ITURHFProp() manages the output reports that are generated by the number of desired runs in the analysis.
	// ITURHFProp() does all the I/O necessary to populate the arrays in the structure PathData *path.
	//********************************************************************************************

	// Create the foF2, M3kF2, foF2var and antenna arrays for the path structure.
	retval = dllAllocatePathMemory(&path);
	if(retval != RTN_ALLOCATEOK) {
		printf("Main: Error %d from dllAllocatePathMemory\n", retval);
		return retval;
	};

	// Read the analysis configuration data from the file given as argv[1].
	retval = ReadInputConfiguration(argv[1], &ITURHFP, &path);
	if(retval != RTN_RICOK) {
		printf("Main: Error %d from ReadInputConfiguration\n", retval);
		return retval;
	};

	// Now that the input has been loaded the location of the transmitter and receiver are known
	// so the bearing of the antennas can be determined if necessary.
	if(ITURHFP.AntennaOrientation == TX2RX) {
		ITURHFP.TXBearing = dllBearing(path.L_tx, path.L_rx); // Point the transmitter at the receiver.
		ITURHFP.RXBearing = dllBearing(path.L_rx, path.L_tx); // Point the receiver at the transmitter.
	};

	retval = ValidateITURHFP(ITURHFP);
	if(retval != RTN_VALIDATEITURHFPOK) {
		printf("Main: Error %d from ValidateITURHFP\n", retval);
		return retval;
	};

	//Note the month and time are in c index format but human-readable so they start at 0.

	// Get the time to time stamp the output files.
	tm = time(NULL);
	ITURHFP.time = localtime(&tm);

	// Create an output file if none is designated on the command line.
	if(OutFilePath[0] == EMPTY) {
		if(ITURHFP.RptFileFormat == RPT_DUMPPATH) { // All Path Data is desired
			// The path data dump, PDD, file name is time stamped.
			sprintf(OutFileName, "PDD%02d%02d%02d-%02d%02d%02d.txt",
				ITURHFP.time->tm_mday, ITURHFP.time->tm_mon+1, ITURHFP.time->tm_year-100,
				ITURHFP.time->tm_hour, ITURHFP.time->tm_min, ITURHFP.time->tm_sec);
			strcpy(OutFilePath, ITURHFP.RptFilePath);
			strcat(OutFilePath, OutFileName);
			// Store the output file
			strcpy(ITURHFP.RptFilePath, OutFilePath);
		}
		else { // Customized report is desired
			// Open the report file and initialize the file pointer in ITURHFP
			sprintf(OutFileName, "RPT%02d%02d%02d-%02d%02d%02d.txt",
				ITURHFP.time->tm_mday, ITURHFP.time->tm_mon+1, ITURHFP.time->tm_year-100,
				ITURHFP.time->tm_hour, ITURHFP.time->tm_min, ITURHFP.time->tm_sec);
			strcpy(OutFilePath, ITURHFP.RptFilePath);
			strcat(OutFilePath, OutFileName);
			// Store the output file
			strcpy(ITURHFP.RptFilePath, OutFilePath);
		};
	}
	else {
		// Store the output file. The user has designated the output file name on the command line
		strcpy(ITURHFP.RptFilePath, OutFilePath);
	};

	// Open the output file and initialize the file pointer in ITURHFP
	ITURHFP.rptfp = fopen(ITURHFP.RptFilePath, "w");
	if(ITURHFP.rptfp == NULL) {
		printf("Main: Error %d Can't open output file %s\n", RTN_ERROPENOUTPUTFILE, ITURHFP.RptFilePath);
		return RTN_ERROPENOUTPUTFILE;
	};

	// User feedback
	if (ITURHFP.silent != TRUE)
	{
		PrintITUHeader(stdout, asctime(ITURHFP.time), ITURHFP.P533ver, ITURHFP.P533compt, "", "");
		printf("Main: Writing to file %s \n", OutFilePath);
	};

	// Run ITURHFProp now that the input file has been validated.
	retval = ITURHFProp(&path, &ITURHFP);
	if(retval != RTN_ITURHFPropOK) {
		printf(" path hour %d\n", path.hour);
		printf("Main: Error %d from ITURHFProp\n", retval);
		return retval;
	};



	// Clean up
	// Close the output file
	if(ITURHFP.rptfp != NULL) fclose(ITURHFP.rptfp); // Close report file

	// Free all the memory
	retval = dllFreePathMemory(&path);
	if(retval != RTN_PATHFREED) {
		printf("Main: Error %d from dllFreePathMemory\n", retval);
		return retval;
	};

	return RTN_MAINOK;
};


int ITURHFProp(struct PathData *path, struct ITURHFProp *ITURHFP) {

	/*
	   ITURHFProp() - Calls P533() multiple times and manages the input and output. This typically means that
	  		data is selectively set in the structure PathData path before calling P533() and then data
	  		is then removed from the structure PathData path once the calculation is complete. This routine has
	  		five major loops to control P533():
	  				i)   Longitude
	  				ii)  Latitude
	  				iii) Frequency
	  				iv)  Hours
	  				v)   Months

	  			INPUT
	  				struct PathData *path
	  				struct ITURHFProp *ITURHFP

	  			OUTPUT
	  				Output files PDD or RPT that are time stamped with the data required.

				EXTERNAL DLL
					This program requires P533.dll and P372.dll

	 */

	int i;
	int retval; // Return value
	int count;

	// Initialize the output record counter.
	count = 1;

	// Determine the maximum hour
	ITURHFP->ihrend = 0;
	for(i=0; i<NMBOFHOURS; i++) {
		if((0 <= ITURHFP->hrs[i]) && (ITURHFP->hrs[i] < 24)) ITURHFP->ihrend += 1;
	};

	// Determine the maximum frequency
	ITURHFP->ifrqend = 0;
	for(i=0; i<NMBOFFREQS; i++) {
		if((1.6 <= ITURHFP->frqs[i]) && (ITURHFP->frqs[i] <= 30.0)) ITURHFP->ifrqend += 1;
	};

	// Determine the maximum month
	ITURHFP->imnthend = 0;
	for(i=0; i<NMBOFMONTHS; i++) {
		if((0 <= ITURHFP->months[i]) && (ITURHFP->months[i] <= 12.0)) ITURHFP->imnthend += 1;
	};

	// Determine the area. If the values are all the same then do only one point. This is point-to-point mode.
	ITURHFP->ilatend = 0;
	ITURHFP->ilngend = 0;
	ITURHFP->ilatend = abs((int)(INTTWEEK+(ITURHFP->L_UL.lat - ITURHFP->L_LR.lat)/ITURHFP->latinc)); // Push the double a little to avoid (int) casting errors.
	++ITURHFP->ilatend;
	ITURHFP->ilngend = abs((int)(INTTWEEK+(ITURHFP->L_LR.lng - ITURHFP->L_LL.lng)/ITURHFP->lnginc)); // Push the double a little to avoid (int) casting errors.
	++ITURHFP->ilngend;

	// Read in the MUF decile values for the entire year.
	retval = dllReadP1239Func(path, ITURHFP->DataFilePath);
	if(retval != RTN_READP1239OK) {
		return retval;
	};

	// Read the antenna data.
	retval = ReadAntennaPatterns(path, *ITURHFP);
	if(retval != RTN_READANTENNAPATTERNSOK) {
		return retval;
	};

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
	dllReadFamDud = (iReadFamDud)GetProcAddress((HMODULE)hLib, "ReadFamDud");
#elif __linux__ || __APPLE__
	void * hLib;
	hLib = dlopen("libp372.so", RTLD_NOW);
	if (!hLib) {
		printf("Couldn't load libp372.so, exiting.\n");
		exit(1);
	}
	dllReadFamDud = dlsym(hLib, "ReadFamDud");
#endif
	// End P372.DLL Load ************************************************

	// ********************** Month Loop **********************************************************
	for(ITURHFP->imnth=0; ITURHFP->imnth<ITURHFP->imnthend; ITURHFP->imnth++) { // months
		// Save the month of interest to the path structure for processing.
		path->month = ITURHFP->months[ITURHFP->imnth];


		// Read in the ionospheric parameters for the particular month for the call to P533.
		retval = dllReadIonParametersBinFunc(path->month, path->foF2, path->M3kF2, ITURHFP->DataFilePath, ITURHFP->silent);
		if(retval != RTN_READIONPARAOK) {
			return retval;
		};

		// Read in the atmospheric coefficients for the particular month.
		// The subroutine dllReadFamDud() is from P372.dll
		retval = dllReadFamDud(&path->noiseP, ITURHFP->DataFilePath, path->month);
		if(retval != RTN_READFAMDUDOK) {
			return retval;
		};

		// Before you enter the remaining loops for the analysis give the user feedback.
		if(ITURHFP->silent != TRUE) {
			printf("\nCounting P533() Runs:\n");
		};

		// ******************* Hours loop ******************************************************
		for(ITURHFP->ihr=0; ITURHFP->ihr<ITURHFP->ihrend; ITURHFP->ihr++) { // hours
			path->hour = ITURHFP->hrs[ITURHFP->ihr];

			// **************** Frequency loop *************************************************
			for(ITURHFP->ifrq=0; ITURHFP->ifrq<ITURHFP->ifrqend; ITURHFP->ifrq++) { // freqs
				path->frequency = ITURHFP->frqs[ITURHFP->ifrq];

				// ********************* Latitude loop *****************************************
				for(ITURHFP->ilat=0; ITURHFP->ilat<ITURHFP->ilatend; ITURHFP->ilat++) {
					// Increment the latitude
					path->L_rx.lat = ITURHFP->L_LL.lat + ITURHFP->ilat*ITURHFP->latinc;

					// ***************** Longitude loop ****************************************
					for(ITURHFP->ilng=0; ITURHFP->ilng<ITURHFP->ilngend; ITURHFP->ilng++) {
						// Increment the longitude
						path->L_rx.lng = ITURHFP->L_LL.lng + ITURHFP->ilng*ITURHFP->lnginc;

						// User feedback
						if(ITURHFP->silent != TRUE) {
							printf("\r%d", count++);
						};

						// Run the model
						retval = dllP533(path); // Run P533()
						if(retval != RTN_P533OK) {
							return retval;
						};

						// Write the output
						if((ITURHFP->RptFileFormat & RPT_DUMPPATH) == RPT_DUMPPATH) {
							DumpPathData(*path, *ITURHFP);
						}
						else {
							Report(*path, *ITURHFP);
						};
					}; // ************* End Longitude loop ***************************************

				}; // ***************** End Latitude loop ****************************************

			}; // ********************* End Frequency loop ***************************************

		}; // ************************* End Hours loop *******************************************

		// Reset the counter for the next month.
		count = 1;

		if(ITURHFP->silent != TRUE) {
			printf("\n\n");
		};


	}; // ***************************** End Months loop ******************************************

	return RTN_ITURHFPropOK;
};

void help(void)
{
	printf("\n");
	printf("ITUFHProp [Options] [Input File] [Output File]\n");
	printf("\tOptions\n");
	printf("\t\t-c CSV: Create an RFC4180 compliant CSV output file.\n");
	printf("\t\t-h Help: Displays help\n");
	printf("\t\t-s Silent Mode: Supresses display output except for error\n");
	printf("\t\t   messages\n");
	printf("\t\t-t Table: Print only the CSV table to the output file.\n");
	printf("\t\t-v Version: Displays the version of ITURHFProp and P533\n");
	printf("\n");
	printf("\tInput File\n");
	printf("\t\tFull path name of the input file. If no path is given the\n");
	printf("\t\tcurrent directory is assumed.\n");
	printf("\n");
	printf("\tOutput File\n");
	printf("\t\tFull path name of the output file. Note: Existing files will\n");
	printf("\t\tbe overwritten. If no output file is\n");
	printf("\t\tindicated a default output file, either a report\n");
	printf("\t\tor a path dump file will be created in\n");
	printf("\t\tthe .\\Report file directory\n");
	printf("\n");
};

//////////////////////////////////////////////////////////////////////////////
//      Copyright  International Telecommunication Union (ITU) 2019         //
//                     All rights reserved.                                 //
// No part of this publication may be reproduced, by any means whatsoever,  //
//              without written permission of ITU                           //
//////////////////////////////////////////////////////////////////////////////
