#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Local includes
#include "Common.h"
#include "P533.h"
#include "ITURHFProp.h"
// End local includes

// Local prototypes
void substrbtwnchar(char instr[256], char searchchar, char * outstr);
unsigned long OutputOption(char optstr[256]);
void InitializeInput(struct ITURHFProp *ITURHFP, struct PathData *path);
// End local prototypes

int ReadInputConfiguration(char InFilePath[256], struct ITURHFProp *ITURHFP, struct PathData *path) {

	// This program reads data from a file and calls ITURHFProp().
	// All inputs are in degrees.

	int i,k;
	int retval;

	char line[256];
	char instr[256];
	char optstr[256];

	FILE *fp;

	// Open the file that is specified on the command line.
	fp = fopen(InFilePath, "r");
	if(fp == NULL) {
		printf("ReadingInputConfiguration: ERROR Can't find input file - %s\n", InFilePath);
		printf("Press Enter...");
		getchar();
		return RTN_ERRNOINPUTFILE;
	};

	// Initailize the input values in the structure PathData path and
	// structure ITURHFProp ITURHFP to default values.
	InitializeInput(ITURHFP, path);

	// k is the lines read counter
	k = 1;

	// Read the first line of the file.
	fgets(line, 256, fp);
	while (!feof(fp)) {

		// Check if the line is a comment or blank.
		if (((line[0] == '/') && (line[1] == '/'))
			||
			((line[0] == ' ') && (line[1] == ' '))) {
			// Ignore line
		}
		else { // Valid line
			// Check for the reserved words
			if (strncmp("PathName", line, 8) == 0) { // char 8 is a space
				// The name is between two quotes-find them.
				substrbtwnchar(line, '\"', path->name);
			};
			if (strncmp("TXGOS", line, 5) == 0) {
				sscanf(line, "%*s %lf", &ITURHFP->TXGOS);
			};
			if (strncmp("RXGOS", line, 5) == 0) {
				sscanf(line, "%*s %lf", &ITURHFP->RXGOS);
			};
			if (strncmp("AntennaOrientation", line, 18) == 0) {
				// The name is between two quotes-find them.
				substrbtwnchar(line, '\"', instr);
				if (strcmp(instr, "TX2RX") == 0) {
					ITURHFP->AntennaOrientation = TX2RX;
				}
				else if ((strcmp(instr, "ARBITRARY") == 0) || (strcmp(instr, "MANUAL") == 0)) {
					ITURHFP->AntennaOrientation = MANUAL;
				};
			};
			if (strncmp("TXBearing", line, 5) == 0) {
				sscanf(line, "%*s %lf", &ITURHFP->TXBearing);
				ITURHFP->TXBearing = ITURHFP->TXBearing*D2R;
			};
			if (strncmp("RXBearing", line, 5) == 0) {
				sscanf(line, "%*s %lf", &ITURHFP->RXBearing);
				ITURHFP->RXBearing = ITURHFP->RXBearing*D2R;
			};
			if (strncmp("PathTXName", line, 10) == 0) {
				// The name is between two quotes-find them.
				substrbtwnchar(line, '\"', path->txname);
			};
			if (strncmp("Path.L_tx.lat", line, 13) == 0) {
				sscanf(line, "%*s %lf", &path->L_tx.lat);
				path->L_tx.lat = path->L_tx.lat*D2R;
			};
			if (strncmp("Path.L_tx.lng", line, 13) == 0) {
				sscanf(line, "%*s %lf", &path->L_tx.lng);
				path->L_tx.lng = path->L_tx.lng*D2R;
			};
			if (strncmp("TXAntFilePath", line, 13) == 0) {
				// The name is between two quotes-find them.
				substrbtwnchar(line, '\"', ITURHFP->TXAntFilePath);
			};
			if (strncmp("PathRXName", line, 10) == 0) {
				// The name is between two quotes-find them.
				substrbtwnchar(line, '\"', path->rxname);
			};
			if (strncmp("Path.L_rx.lat", line, 13) == 0) {
				sscanf(line, "%*s %lf", &path->L_rx.lat);
				path->L_rx.lat = path->L_rx.lat*D2R;
			};
			if (strncmp("Path.L_rx.lng", line, 13) == 0) {
				sscanf(line, "%*s %lf", &path->L_rx.lng);
				path->L_rx.lng = path->L_rx.lng*D2R;
			};
			if (strncmp("RXAntFilePath", line, 13) == 0) {
				// The name is between two quotes-find them.
				substrbtwnchar(line, '\"', ITURHFP->RXAntFilePath);
			};
			if (strncmp("Path.year", line, 9) == 0) {
				sscanf(line, "%*s %d", &path->year);
			};
			if (strncmp("Path.month", line, 10) == 0) {
				sscanf(line, "%*s %[^/\n]", &instr);
				// If this contains no commas, then it is a single month.
				if (strchr(instr, ',') == NULL) {
					sscanf(line, "%*s %d", &ITURHFP->months[0]);
					ITURHFP->months[0] -= 1;
				}
				else {
					i = 0;
					retval = 2;
					while ((strlen(instr) != 0) && (instr[0] != '/') && (retval == 2)) {
						retval = sscanf(instr, "%d, %[0-9 ,]", &ITURHFP->months[i], &instr);
						ITURHFP->months[i++] -= 1;
					};
				};
			};
			if (strncmp("Path.hour", line, 9) == 0) {
				sscanf(line, "%*s %[^/\n]", &instr);
				// If this contains no commas, then it is a single month.
				if (strchr(instr, ',') == NULL) {
					sscanf(line, "%*s %d", &ITURHFP->hrs[0]);
					ITURHFP->hrs[0] -= 1;
				}
				else {
					i = 0;
					retval = 2;
					while ((strlen(instr) != 0) && (instr[0] != '/') && (retval == 2)) {
						retval = sscanf(instr, "%d, %[0-9 ,]", &ITURHFP->hrs[i], &instr);
						ITURHFP->hrs[i++] -= 1;
					};
				};
			};
			if (strncmp("Path.SSN", line, 8) == 0) {
				sscanf(line, "%*s %d", &path->SSN);
			};
			if (strncmp("Path.frequency", line, 14) == 0) {
				sscanf(line, "%*s %[^/\n]", &instr);
				// If this contains no commas, then it is a single month.
				if (strchr(instr, ',') == NULL) {
					sscanf(line, "%*s %lf", &ITURHFP->frqs[0]);
				}
				else {
					i = 0;
					retval = 2;
					while ((strlen(instr) != 0) && (instr[0] != '/') && (retval == 2)) {
						retval = sscanf(instr, "%lf, %[0-9 ,.]", &ITURHFP->frqs[i++], &instr);
					};
				};
			};
			if (strncmp("Path.txpower", line, 12) == 0) {
				sscanf(line, "%*s %lf", &path->txpower);
			};
			if (strncmp("Path.BW", line, 7) == 0) {
				sscanf(line, "%*s %lf", &path->BW);
			};
			if (strncmp("Path.SNRr", line, 9) == 0) {
				sscanf(line, "%*s %lf", &path->SNRr);
			};
			if (strncmp("Path.SNRXXp", line, 9) == 0) {
				sscanf(line, "%*s %d", &path->SNRXXp);
			};
			if (strncmp("Path.ManMadeNoise", line, 17) == 0) {
				// The name is between two quotes-find them.
				substrbtwnchar(line, '\"', instr);
				if (strcmp(instr, "RESIDENTIAL") == 0) {
					path->noiseP.ManMadeNoise = RESIDENTIAL;
				}
				else if (strcmp(instr, "CITY") == 0) {
					path->noiseP.ManMadeNoise = CITY;
				}
				else if (strcmp(instr, "RURAL") == 0) {
					path->noiseP.ManMadeNoise = RURAL;
				}
				else if (strcmp(instr, "QUIETRURAL") == 0) {
					path->noiseP.ManMadeNoise = QUIETRURAL;
				}
				else if (strcmp(instr, "NOISY") == 0) {
					path->noiseP.ManMadeNoise = NOISY;
				}
				else if (strcmp(instr, "QUIET") == 0) {
					path->noiseP.ManMadeNoise = QUIET;
				}
				else { // This is a number
					sscanf(line, "%*s %lf", &path->noiseP.ManMadeNoise);
				};
			};
			if (strncmp("Path.Modulation", line, 15) == 0) {
				// The name is between two quotes-find them.
				substrbtwnchar(line, '\"', instr);
				if (strcmp(instr, "DIGITAL") == 0) {
					path->Modulation = DIGITAL;
				}
				else if (strcmp(instr, "ANALOG") == 0) {
					path->Modulation = ANALOG;
				};
			};
			if (strncmp("Path.SIRr", line, 9) == 0) {
				sscanf(line, "%*s %lf", &path->SIRr);
			};
			if (strncmp("Path.A", line, 6) == 0) {
				sscanf(line, "%*s %lf", &path->A);
			};
			if (strncmp("Path.TW", line, 7) == 0) {
				sscanf(line, "%*s %lf", &path->TW);
			};
			if (strncmp("Path.FW", line, 7) == 0) {
				sscanf(line, "%*s %lf", &path->FW);
			};
			if (strncmp("Path.T0", line, 7) == 0) {
				sscanf(line, "%*s %lf", &path->T0);
			};
			if (strncmp("Path.F0", line, 7) == 0) {
				sscanf(line, "%*s %lf", &path->F0);
			};
			if (strncmp("Path.SorL", line, 9) == 0) {
				// The name is between two quotes-find them.
				substrbtwnchar(line, '\"', instr);
				if (strcmp(instr, "SHORTPATH") == 0) {
					path->SorL = SHORTPATH;
				}
				else if (strcmp(instr, "LONGPATH") == 0) {
					path->SorL = LONGPATH;
				};
			};
			if (strncmp("RptFilePath", line, 11) == 0) {
				// The name is between two quotes-find them.
				substrbtwnchar(line, '\"', ITURHFP->RptFilePath);
			};
			if (strncmp("RptFileFormat", line, 13) == 0) {
				// The name is between two quotes-find them.
				substrbtwnchar(line, '\"', instr);

				// If this contains no bangs (|), then it is a single option.
				if (strchr(instr, '|') == NULL) {
					ITURHFP->RptFileFormat = OutputOption(instr);
				}
				else {
					retval = 2;
					ITURHFP->RptFileFormat = 0;
					while ((strlen(instr) != 0) && (instr[0] != '/') && (retval == 2)) {
						retval = sscanf(instr, "%s | %[a-z,A-Z _|]", &optstr, &instr);
						ITURHFP->RptFileFormat = ITURHFP->RptFileFormat | OutputOption(optstr);
					};
				};
			};
			if (strncmp("LL.lat", line, 6) == 0) {
				sscanf(line, "%*s %lf", &ITURHFP->L_LL.lat);
				ITURHFP->L_LL.lat = ITURHFP->L_LL.lat*D2R;
			};
			if (strncmp("LL.lng", line, 6) == 0) {
				sscanf(line, "%*s %lf", &ITURHFP->L_LL.lng);
				ITURHFP->L_LL.lng = ITURHFP->L_LL.lng*D2R;
			};
			if (strncmp("LR.lat", line, 6) == 0) {
				sscanf(line, "%*s %lf", &ITURHFP->L_LR.lat);
				ITURHFP->L_LR.lat = ITURHFP->L_LR.lat*D2R;
			};
			if (strncmp("LR.lng", line, 6) == 0) {
				sscanf(line, "%*s %lf", &ITURHFP->L_LR.lng);
				ITURHFP->L_LR.lng = ITURHFP->L_LR.lng*D2R;
			};
			if (strncmp("UL.lat", line, 6) == 0) {
				sscanf(line, "%*s %lf", &ITURHFP->L_UL.lat);
				ITURHFP->L_UL.lat = ITURHFP->L_UL.lat*D2R;
			};
			if (strncmp("UL.lng", line, 6) == 0) {
				sscanf(line, "%*s %lf", &ITURHFP->L_UL.lng);
				ITURHFP->L_UL.lng = ITURHFP->L_UL.lng*D2R;
			};
			if (strncmp("UR.lat", line, 6) == 0) {
				sscanf(line, "%*s %lf", &ITURHFP->L_UR.lat);
				ITURHFP->L_UR.lat = ITURHFP->L_UR.lat*D2R;
			};
			if (strncmp("UR.lng", line, 6) == 0) {
				sscanf(line, "%*s %lf", &ITURHFP->L_UR.lng);
				ITURHFP->L_UR.lng = ITURHFP->L_UR.lng*D2R;
			};
			// An alternative way to input data is by two points of the analysis rectangle
			// which is more efficient since 4 corner input is redundant
			// The south east corner of the analysis rectangle
			if (strncmp("SE.lat", line, 6) == 0) {
				sscanf(line, "%*s %lf", &ITURHFP->L_LR.lat);
				ITURHFP->L_LR.lat = ITURHFP->L_LR.lat*D2R;
			};
			if (strncmp("SE.lng", line, 6) == 0) {
				sscanf(line, "%*s %lf", &ITURHFP->L_LR.lng);
				ITURHFP->L_LR.lng = ITURHFP->L_LR.lng*D2R;
			};
			// The north west corner of the analysis rectangle
			if (strncmp("NW.lat", line, 6) == 0) {
				sscanf(line, "%*s %lf", &ITURHFP->L_UL.lat);
				ITURHFP->L_UL.lat = ITURHFP->L_UL.lat*D2R;
			};
			if (strncmp("NW.lng", line, 6) == 0) {
				sscanf(line, "%*s %lf", &ITURHFP->L_UL.lng);
				ITURHFP->L_UL.lng = ITURHFP->L_UL.lng*D2R;
			};
			// Analysis window increments
			if (strncmp("latinc", line, 6) == 0) {
				sscanf(line, "%*s %lf", &ITURHFP->latinc);
				ITURHFP->latinc = ITURHFP->latinc*D2R;
			};
			if (strncmp("lnginc", line, 6) == 0) {
				sscanf(line, "%*s %lf", &ITURHFP->lnginc);
				ITURHFP->lnginc = ITURHFP->lnginc*D2R;
			};
			//
			if (strncmp("DataFilePath", line, 12) == 0) {
				// The name is between two quotes-find them.
				substrbtwnchar(line, '\"', ITURHFP->DataFilePath);
			};
		};
		// Read the next line.
		fgets(line, 256, fp);
	};

	// There are optional ways to enter the analysis rectangle. 
	// Determine if the user choose to inpout the analysis rectangle 
	// defined by the north west and south east corner
	if ((ITURHFP->L_LR.lat != TOOBIG) && (ITURHFP->L_LR.lng != TOOBIG) &&
		(ITURHFP->L_LL.lat == TOOBIG) && (ITURHFP->L_LL.lng == TOOBIG) &&
		(ITURHFP->L_UR.lat == TOOBIG) && (ITURHFP->L_UR.lng == TOOBIG) &&
		(ITURHFP->L_UL.lat != TOOBIG) && (ITURHFP->L_UL.lng != TOOBIG)) {
		// Option 1: Is used so set the remaining corners
		ITURHFP->L_LL.lat = ITURHFP->L_LR.lat; // South
		ITURHFP->L_LL.lng = ITURHFP->L_UL.lng; // West
		ITURHFP->L_UR.lat = ITURHFP->L_UL.lat; // North
		ITURHFP->L_UR.lng = ITURHFP->L_LR.lng; // East
	};

	fclose(fp);

	return RTN_RICOK;
};


void substrbtwnchar(char instr[256], char searchchar, char * outstr) {

	char * frstchar;
	char * scndchar;

	int n;

	frstchar = strchr(instr, searchchar);
	scndchar = strrchr(instr, searchchar);
	if((&frstchar[0] != NULL) && (&scndchar[0] != NULL)) {
		n =  (int)(&scndchar[0] - &frstchar[0] - sizeof(char));
		strncpy(outstr, &frstchar[0]+sizeof(char), n);
		outstr[n] = '\0';
	}
	else {
		outstr[0] = '\0';
	};

	return;
};

unsigned long OutputOption(char optstr[256]) {

	/*
	  OutputOption() - This subroutine determines which variables the user
	    wants in a report file. Since some of the options are a subset of
		others make sure that the select option is unique.

		INPUTS
			char optstr[256] This string is in the format:
				RPT_OPTION1 | RPT_OPTION2 | RPT_OPTION# | ...
				Where OPTION1, OPTION2 and OPTION3 are defined
				in ITURHFProp.h

		OUTPUT
			returns unsigned long where each bit field represents a
				particular varable or set of variables as defined in
				ITURHFProp.h
	*/
	// RPT_A*
	if(strncmp("RPT_ALL", optstr, 7) == 0) {
		return RPT_ALL;
	};
	// RPT_B*
	if(strncmp("RPT_BCR", optstr, 7) == 0) {
		return RPT_BCR;
	};
	if((strncmp("RPT_BMUF", optstr, 8) == 0) && (strncmp("RPT_BMUFD", optstr, 9) != 0)) {
		return RPT_BMUF;
	};
	if(strncmp("RPT_BMUFD", optstr, 9) == 0) {
		return RPT_BMUFD;
	};
	// RPT_D*
	if(strncmp("RPT_DUMPPATH", optstr, 12) == 0) {
		return RPT_DUMPPATH;
	};
	if(strncmp("RPT_DOMMODE", optstr, 11) == 0) {
		return RPT_DOMMODE;
	};
	if(strncmp("RPT_DMAX", optstr, 8) == 0) {
		return RPT_DMAX;
	};
	if((strncmp("RPT_D", optstr, 5) == 0) && (strncmp("RPT_DUMPPATH", optstr, 12) != 0) && (strncmp("RPT_DMAX", optstr, 8) != 0) && (strncmp("RPT_DOMMODE", optstr, 11) != 0)) {
		return RPT_D;
	};
	// RPT_E*
	if(strncmp("RPT_ELE", optstr, 7) == 0) {
		return RPT_ELE;
	};
	if(strncmp("RPT_ESL", optstr, 7) == 0) {
		return RPT_ESL;
	};
	if((strncmp("RPT_E", optstr, 5) == 0) && (strncmp("RPT_ELE", optstr, 7) != 0)) {
		return RPT_E;
	};
	// RPT_G*
	if(strncmp("RPT_GRW", optstr, 7) == 0) {
		return RPT_GRW;
	};
	// RPT_L*
	if(strncmp("RPT_LONG", optstr, 8) == 0) {
		return RPT_LONG;
	};
	// RPT_M*
	if(strncmp("RPT_MIR", optstr, 7) == 0) {
		return RPT_MIR;
	};
	// RPT_N*
	if(strncmp("RPT_N0_F2", optstr, 9) == 0) {
		return RPT_N0_F2;
	};
	if(strncmp("RPT_N0_E", optstr, 8) == 0) {
		return RPT_N0_E;
	};
	if(strncmp("RPT_NOISESOURCESD", optstr, 17) == 0) {
		return RPT_NOISESOURCESD;
	};
	if((strncmp("RPT_NOISESOURCES", optstr, 16) == 0) && (strncmp("RPT_NOISESOURCESD", optstr, 17) != 0)) {
		return RPT_NOISESOURCES;
	};
	if(strncmp("RPT_NOISETOTALD", optstr, 15) == 0) {
		return RPT_NOISETOTALD;
	};
	if((strncmp("RPT_NOISETOTAL", optstr, 14) == 0) && (strncmp("RPT_NOISETOTALD", optstr, 15) != 0)) {
		return RPT_NOISETOTAL;
	};
	//RPT_O*
	if(strncmp("RPT_OPMUFD", optstr, 10) == 0) {
		return RPT_OPMUFD;
	};
	if((strncmp("RPT_OPMUF", optstr, 9) == 0) && (strncmp("RPT_OPMUFD", optstr, 10) != 0)) {
		return RPT_OPMUF;
	};
	if(strncmp("RPT_OCRS", optstr, 8) == 0) {
		return RPT_OCRS;
	};
	if((strncmp("RPT_OCR", optstr, 7) == 0) && (strncmp("RPT_OCRS", optstr, 8) != 0)) {
		return RPT_OCR;
	};
	// RPT_P*
	if(strncmp("RPT_PR", optstr, 6) == 0) {
		return RPT_PR;
	};
	// RPT_R*
	if(strncmp("RPT_RSN", optstr, 7) == 0) {
		return RPT_RSN;
	};
	if(strncmp("RPT_RXLOCATION", optstr, 14) == 0) {
		return RPT_RXLOCATION;
	};
	//RPT_S*
	if(strncmp("RPT_SNRXX", optstr, 9) == 0) {
		return RPT_SNRXX;
	};
	if(strncmp("RPT_SNRD", optstr, 8) == 0) {
		return RPT_SNRD;
	};
	if((strncmp("RPT_SNR", optstr, 7) == 0) && (strncmp("RPT_SNRD", optstr, 8) != 0) && (strncmp("RPT_SNRXX", optstr, 9) != 0)) {
		return RPT_SNR;
	};
	if(strncmp("RPT_SIRD", optstr, 8) == 0) {
		return RPT_SIRD;
	};
	if((strncmp("RPT_SIR", optstr, 7) == 0) && (strncmp("RPT_SIRD", optstr, 8) != 0)) {
		return RPT_SIR;
	};


	return 0;
};

void InitializeInput(struct ITURHFProp *ITURHFP, struct PathData *path) {

	/*
		InitializeInput = Initializes all of the input parameters that are read by
			ReadInputConfiguration()

			INPUT
				struct ITURHFProp *ITURHFP
				struct PathData *path

			OUTPUT
				none
	*/

	int i;

	sprintf(path->name, "Path Data");
	sprintf(path->txname, "Transmitter");
	path->L_tx.lat = 0.0;
	path->L_tx.lng = 0.0;
	sprintf(path->rxname, "Receiver");
	path->L_rx.lat = 0.0;
	path->L_rx.lng = 0.0;
	path->year = 2022;
	path->SSN = 99;
	path->txpower = 0.0;
	path->BW = 0.0;
	path->SNRr = 0.0;
	path->SNRXXp = 0;
	path->noiseP.ManMadeNoise = NOISY;
	path->Modulation = ANALOG;
	path->SIRr = 0.0;
	path->A = 0.0;
	path->TW = 0.0;
	path->FW = 0.0;
	path->T0 = 0.0;
	path->F0 = 0.0;
	path->SorL= SHORTPATH;

	ITURHFP->TXGOS = 0.0;
	ITURHFP->RXGOS = 0.0;
	ITURHFP->AntennaOrientation = TX2RX;
	ITURHFP->TXBearing = 0.0;
	ITURHFP->RXBearing = 0.0;
	sprintf(ITURHFP->TXAntFilePath, ".");
	sprintf(ITURHFP->RXAntFilePath, ".");
	for(i=0; i<NMBOFFREQS; i++) ITURHFP->frqs[i] = 99.0;
	for(i=0; i<NMBOFHOURS; i++) ITURHFP->hrs[i] = 99;
	for(i=0; i<NMBOFHOURS; i++) ITURHFP->months[i] = 99;
	sprintf(ITURHFP->RptFilePath, ".");
	ITURHFP->RptFileFormat = RPT_ALL;
	ITURHFP->L_LL.lat = TOOBIG;
	ITURHFP->L_LL.lng = TOOBIG;
	ITURHFP->L_LR.lat = TOOBIG;
	ITURHFP->L_LR.lng = TOOBIG;
	ITURHFP->L_UL.lat = TOOBIG;
	ITURHFP->L_UL.lng = TOOBIG;
	ITURHFP->L_UR.lat = TOOBIG;
	ITURHFP->L_UR.lng = TOOBIG;
	ITURHFP->latinc = 1.0;
	ITURHFP->lnginc = 1.0;
	sprintf(ITURHFP->DataFilePath, ".");

	return;

};


int ReadAntennaPatterns(struct PathData *path, struct ITURHFProp ITURHFP) {

	int retval;
    int antType;
    int lineCtr;
    FILE *fp;
	char line[256];		// Read input line
	char instr[256];	// String temp

	// User feedback
	if(ITURHFP.silent != TRUE) {
		printf("ReadAntennaPatterns: Reading transmit and receive antenna\n");
	};

	// At this point there is only one other antenna pattern file type that can be read.
	// The antenna pattern must be rotated to the correct azimuth before P533() is run.

	// Determine the type of receiver antenna file.
	if(strcmp(ITURHFP.RXAntFilePath, "ISOTROPIC") == 0) { // Isotropic Antenna
		ITURHFP.rxantfp = NULL;
		dllIsotropicPatternFunc(&path->A_rx, ITURHFP.RXGOS, ITURHFP.silent);
		// Store the name of the antenna to the path structure.
		strcpy(path->A_rx.Name, "ISOTROPIC");
	} else {
	    //If it's not an ISOTROPIC, open the file and take a look...
	    /* VOACAP type antennas identify the antenna type on the forth
	     * line; e.g;
         *   14    [ 2] Antenna Type..: 30 x (efficiency + 91 gain values) follow
         */
	    fp = fopen(ITURHFP.RXAntFilePath, "r");
		
	    if (fp == NULL) {
            if(ITURHFP.silent != TRUE) {
				printf("Error opening Rx. antenna file %.65s\n", ITURHFP.RXAntFilePath);
			};
		    return RTN_ERRCANTOPENRXANTFILE;
	    };

	    for (lineCtr = 0 ; lineCtr<4 ; ++lineCtr) {
		    fgets(line, sizeof(line), fp);		// Scroll to line 3, Antenna type
	    }

	    sscanf(line, " %d %s\n", &antType, instr);
        rewind(fp);

	    if(antType == 11) {
		    retval = dllReadType11Func(&path->A_rx, fp, ITURHFP.silent);
            fclose(fp);
		    if(retval != RTN_READANTENNAPATTERNSOK) {
				    return retval;
            }
		} else if(antType == 13) {
		    retval = dllReadType13Func(&path->A_rx, fp, ITURHFP.RXBearing, ITURHFP.silent);
            fclose(fp);
		    if (retval != RTN_READANTENNAPATTERNSOK) {
				    return retval;
		    };
	    } else if (antType == 14) {
		    retval = dllReadType14Func(&path->A_rx, fp, ITURHFP.silent);
            fclose(fp);
		    if (retval != RTN_READANTENNAPATTERNSOK) {
				    return retval;
		    };
        } else {
            printf("Unsuppported antenna type: %d\n",antType);
            return RTN_ERRCANTOPENRXANTFILE;
        } 
    }; // end of the rx antenna type

	// Determine the type of transmitter antenna file.
	if(strcmp(ITURHFP.TXAntFilePath, "ISOTROPIC") == 0) { // Isotropic Antenna
		ITURHFP.txantfp = NULL;
		dllIsotropicPatternFunc(&path->A_tx, ITURHFP.TXGOS, ITURHFP.silent);
		// Store the name of the antenna to the path structure.
		strcpy(path->A_tx.Name, "ISOTROPIC");
	} else {
        fp = fopen(ITURHFP.TXAntFilePath, "r");

	    if (fp == NULL) {
			if(ITURHFP.silent != TRUE) {
				printf("Error opening Tx. antenna file %.65s\n", ITURHFP.TXAntFilePath);
			};
		    return RTN_ERRCANTOPENTXANTFILE;
	    };
	    for (lineCtr = 0 ; lineCtr<4 ; ++lineCtr) {
		    fgets(line, sizeof(line), fp);		// Antenna type
	    }
	    sscanf(line, " %d %s\n", &antType, instr);

        rewind(fp);

	    if(antType == 11) {
		    retval = dllReadType11Func(&path->A_tx, fp, ITURHFP.silent);
            fclose(fp);
		    if(retval != RTN_READANTENNAPATTERNSOK) {
				    return retval;
            }
		} else if(antType == 13) {
		    retval = dllReadType13Func(&path->A_tx, fp, ITURHFP.TXBearing, ITURHFP.silent);
            fclose(fp);
		    if(retval != RTN_READANTENNAPATTERNSOK) {
				    return retval;
            }
		} else if (antType == 14) {
		    retval = dllReadType14Func(&path->A_tx, fp, ITURHFP.silent);
            fclose(fp);
		    if (retval != RTN_READANTENNAPATTERNSOK) {
				    return retval;
		    };
	    } else {
			printf("Unsuppported antenna type: %d\n",antType);
            return RTN_ERRCANTOPENTXANTFILE;
	    };

	    return RTN_READANTENNAPATTERNSOK;
    }
};


//////////////////////////////////////////////////////////////////////////////
//      Copyright  International Telecommunication Union (ITU) 2018         //
//                     All rights reserved.                                 //
// No part of this publication may be reproduced, by any means whatsoever,  //
//              without written permission of ITU                           //
//////////////////////////////////////////////////////////////////////////////
