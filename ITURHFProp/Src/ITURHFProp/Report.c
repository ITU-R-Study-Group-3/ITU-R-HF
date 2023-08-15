#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

// Local includes
#include "Common.h"
#include "P533.h"
#include "ITURHFProp.h"
// End local includes

// Local #defines
#define DBLFIELD		"% 7.2lf"
#define DBLFIELD1		"% 9.4lf"
#define DBLFIELD2		"% 9.2lf"
#define DBLFIELD3		"% 9.3lf"
#define STRFIELD		"%5s"

#define RFC4180_DBLFIELD		"%.2lf"
#define RFC4180_DBLFIELD1		"%.4lf"
#define RFC4180_DBLFIELD2		"%.2lf"
#define RFC4180_STRFIELD		"%s"

#define PRINT_RFC4180_HEADER 3
#define PRINT_RFC4180_DATA 2
#define PRINT_HEADER	1
#define PRINT_DATA		0
// End local #define

// Local prototypes
void PrintHeader(struct PathData path, struct ITURHFProp ITURHFP);
void PrintRecord(struct PathData path, struct ITURHFProp ITURHFP, int printhr);
void PrintLastRecord(struct PathData path, struct ITURHFProp ITURHFP);
char EW(double lng);
char NS(double lat);
void function_RPT_D(   struct PathData path, int option, int *col);
void function_RPT_DMAX(struct PathData path, int option, int *col);
void function_RPT_ELE( struct PathData path, int option, int *col);
void function_RPT_BMUF(struct PathData path, int option, int *col);
void function_RPT_BMUFD(struct PathData path, int option, int *col);
void function_RPT_OPMUF(struct PathData path, int option, int *col);
void function_RPT_OPMUFD(struct PathData path, int option, int *col);
void function_RPT_N0_F2(struct PathData path, int option, int *col);
void function_RPT_N0_E(struct PathData path, int option, int *col);
void function_RPT_E(struct PathData path, int option, int *col);
void function_RPT_PR(struct PathData path, int option, int *col);
void function_RPT_GRW(struct PathData path, int option, int *col);
void function_RPT_NOISESOURCES(struct PathData path, int option, int *col);
void function_RPT_NOISESOURCESD(struct PathData path, int option, int *col);
void function_RPT_NOISETOTALD(struct PathData path, int option, int *col);
void function_RPT_NOISETOTAL(struct PathData path, int option, int *col);
void function_RPT_SNR(struct PathData path, int option, int *col);
void function_RPT_SNRD(struct PathData path, int option, int *col);
void function_RPT_SNRXX(struct PathData path, int option, int *col);
void function_RPT_SIRD(struct PathData path, int option, int *col);
void function_RPT_SIR(struct PathData path, int option, int *col);
void function_RPT_RSN(struct PathData path, int option, int *col);
void function_RPT_BCR(struct PathData path, int option, int *col);
void function_RPT_OCR(struct PathData path, int option, int *col);
void function_RPT_OCRS(struct PathData path, int option, int *col);
void function_RPT_MIR(struct PathData path, int option, int *col);
void function_RPT_ANTENNA(struct PathData path, int option, int *col);
void function_RPT_DOMMODE(struct PathData path, int option, int *col);
void function_RPT_RXLOCATION(struct PathData path, int option, int *col);
void function_RPT_ESL(struct PathData path, int option, int *col);
void function_RPT_LONG(struct PathData path, int option, int *col);
// End local prototypes

// Local globals
char *months[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
char *cp[] = {"T + 1000","T + d0/2","MidPoint","R + d0/2","R + 1000"};
char *season[] = {"Winter", "Equinox", "Summer"};
char outstr[256] = "";
FILE *fp; // Temp file pointer for readability
static int Header = TRUE; // The first time you enter this routine the head will need to be printed.
// End local globals

void Report(struct PathData path, struct ITURHFProp ITURHFP) {

	/*
	 * Report() Prints a report of the format specified by the variable ITURHFP.RptFileFormat
	 *		The report that is generated is a comma separated value (CSV) file. Setting the
	 *		value of ITURHFP.RptFileFormat before this routine is called will allow for the custom
	 *		output of the CSV file. The options to set the variable ITURHFP.RptFileFormat are in
	 *		ITURHFProp.h under the heading "Report output options".
	 *
	 *			INPUT
	 *				struct PathData path
	 *				struct ITURHFProp ITURHFP
	 *
	 *			OUTPUT
	 *	 			Printed data to the file named
	 *				RPTddmmyy-hhnnss.txt - where the report file is time stamped as follows:
	 *					dd = day
	 *					mm = month
	 *					yy = year
	 *					hh = hour
	 *					nn = minute
	 *					ss = seconds
	 *
	 */

	// For readability set the local global file pointer.
	fp = ITURHFP.rptfp;

	// Determine if the the user wants the header printed
	if (ITURHFP.header == TRUE) {
		if (Header == TRUE) {

			// Only print out the header one time.
			Header = FALSE;

			// First output
			// Write the header to the report file.
			PrintHeader(path, ITURHFP);
			PrintRecord(path, ITURHFP, PRINT_HEADER);
			// If there is only one line to be printed in the output than the first record is the last record
			if ((ITURHFP.ihr == ITURHFP.ihrend - 1) &&
				(ITURHFP.ifrq == ITURHFP.ifrqend - 1) &&
				(ITURHFP.ilng == ITURHFP.ilngend - 1) &&
				(ITURHFP.imnth == ITURHFP.imnthend - 1) &&
				(ITURHFP.ilat == ITURHFP.ilatend - 1)) {
				// Last record of the report
				PrintLastRecord(path, ITURHFP);
			}
			else {
				PrintRecord(path, ITURHFP, PRINT_DATA);
			};
		}
		else if ((ITURHFP.ihr == ITURHFP.ihrend - 1) &&
			(ITURHFP.ifrq == ITURHFP.ifrqend - 1) &&
			(ITURHFP.ilng == ITURHFP.ilngend - 1) &&
			(ITURHFP.imnth == ITURHFP.imnthend - 1) &&
			(ITURHFP.ilat == ITURHFP.ilatend - 1)) {
			// Last record of the report
			PrintLastRecord(path, ITURHFP);
		}
		else { // Middle of the report
			PrintRecord(path, ITURHFP, PRINT_DATA);
		};
	}
	else if (ITURHFP.header == FALSE) {
		if (ITURHFP.csvRFC4180 == TRUE) {
			if (Header == TRUE) {
				PrintRecord(path, ITURHFP, PRINT_RFC4180_HEADER);
				Header = FALSE;
			}
			PrintRecord(path, ITURHFP, PRINT_RFC4180_DATA);
		} else {
			PrintRecord(path, ITURHFP, PRINT_DATA);
		}
	};

	return;

};

void PrintRecord(struct PathData path, struct ITURHFProp ITURHFP, int option) {

	int col;

	col = 3;

	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column 01: Month\n");
			fprintf(fp, "Column 02: Hour\n");
			fprintf(fp, "Column 03: Frequency (MHz)\n");
			break;
		case PRINT_RFC4180_HEADER:
		  fprintf(fp, "month,hour,frequency");
			break;
		case PRINT_DATA:
			// Each record will require the month, hour, and frequency
			// Month
			fprintf(fp, "%02d", path.month+1);
			fprintf(fp,",");
			// Hour
			fprintf(fp, " %02d", (path.hour==0?24:path.hour)/*+1*/);
			fprintf(fp,",");
			// Frequency
			fprintf(fp, DBLFIELD3, path.frequency);
			break;
		case PRINT_RFC4180_DATA:
			// Each record will require the month, hour, and frequency
			// Month
			fprintf(fp, "%d,%d,", path.month+1, (path.hour==0?24:path.hour)/*+1*/);
			fprintf(fp, RFC4180_DBLFIELD, path.frequency);
			break;
	};

	if((ITURHFP.RptFileFormat & RPT_RXLOCATION) == RPT_RXLOCATION) {
		function_RPT_RXLOCATION(path, option, &col);
	};
	if((ITURHFP.RptFileFormat & RPT_D) == RPT_D) {
		function_RPT_D(path, option, &col);
	};
	if((ITURHFP.RptFileFormat & RPT_DMAX) == RPT_DMAX) {
		function_RPT_DMAX(path, option, &col);
	};
	if((ITURHFP.RptFileFormat & RPT_ELE) == RPT_ELE) {
		function_RPT_ELE(path, option, &col);
	};
	if((ITURHFP.RptFileFormat & RPT_BMUF) == RPT_BMUF) {
		function_RPT_BMUF(path, option, &col);
	};
	if((ITURHFP.RptFileFormat & RPT_BMUFD) == RPT_BMUFD) {
		function_RPT_BMUFD(path, option, &col);
	};
	if((ITURHFP.RptFileFormat & RPT_OPMUF) == RPT_OPMUF) {
		function_RPT_OPMUF(path, option, &col);
	};
	if((ITURHFP.RptFileFormat & RPT_OPMUFD) == RPT_OPMUFD) {
		function_RPT_OPMUFD(path, option, &col);
	};
	if((ITURHFP.RptFileFormat & RPT_N0_F2) == RPT_N0_F2) {
		function_RPT_N0_F2(path, option, &col);
	};
	if((ITURHFP.RptFileFormat & RPT_N0_E) == RPT_N0_E) {
		function_RPT_N0_E(path, option, &col);
	};
	if((ITURHFP.RptFileFormat & RPT_E) == RPT_E) {
		function_RPT_E(path, option, &col);
	};
	if((ITURHFP.RptFileFormat & RPT_PR) == RPT_PR) {
		function_RPT_PR(path, option, &col);
	};
	if((ITURHFP.RptFileFormat & RPT_GRW) == RPT_GRW) {
		function_RPT_GRW(path, option, &col);
	};
	if((ITURHFP.RptFileFormat & RPT_NOISESOURCES) == RPT_NOISESOURCES) {
		function_RPT_NOISESOURCES(path, option, &col);
	};
	if((ITURHFP.RptFileFormat & RPT_NOISESOURCESD) == RPT_NOISESOURCESD) {
		function_RPT_NOISESOURCESD(path, option, &col);
	};
	if((ITURHFP.RptFileFormat & RPT_NOISETOTALD) == RPT_NOISETOTALD) {
		function_RPT_NOISETOTALD(path, option, &col);
	};
	if((ITURHFP.RptFileFormat & RPT_NOISETOTAL) == RPT_NOISETOTAL) {
		function_RPT_NOISETOTAL(path, option, &col);
	};
	if((ITURHFP.RptFileFormat & RPT_SNR) == RPT_SNR) {
		function_RPT_SNR(path, option, &col);
	};
	if((ITURHFP.RptFileFormat & RPT_SNRD) == RPT_SNRD) {
		function_RPT_SNRD(path, option, &col);
	};
	if((ITURHFP.RptFileFormat & RPT_SNRXX) == RPT_SNRXX) {
		function_RPT_SNRXX(path, option, &col);
	};
	if((ITURHFP.RptFileFormat & RPT_SIR) == RPT_SIR) {
		function_RPT_SIR(path, option, &col);
	};
	if((ITURHFP.RptFileFormat & RPT_SIRD) == RPT_SIRD) {
		function_RPT_SIRD(path, option, &col);
	};
	if((ITURHFP.RptFileFormat & RPT_RSN) == RPT_RSN) {
		function_RPT_RSN(path, option, &col);
	};
	if((ITURHFP.RptFileFormat & RPT_BCR) == RPT_BCR) {
		function_RPT_BCR(path, option, &col);
	};
	if((ITURHFP.RptFileFormat & RPT_OCR) == RPT_OCR) {
		function_RPT_OCR(path, option, &col);
	};
	if((ITURHFP.RptFileFormat & RPT_OCRS) ==RPT_OCRS ) {
		function_RPT_OCRS(path, option, &col);
	};
	if((ITURHFP.RptFileFormat & RPT_MIR) == RPT_MIR) {
		function_RPT_MIR(path, option, &col);
	};
	if((ITURHFP.RptFileFormat & RPT_DOMMODE) == RPT_DOMMODE) {
		function_RPT_DOMMODE(path, option, &col);
	};
	if((ITURHFP.RptFileFormat & RPT_ESL) == RPT_ESL) {
		function_RPT_ESL(path, option, &col);
	};
	if((ITURHFP.RptFileFormat & RPT_LONG) == RPT_LONG) {
		function_RPT_LONG(path, option, &col);
	};

	// If the data format header is being printed, put the tail on.
	if(option == PRINT_HEADER) {
		fprintf(fp, "\n");
	    fprintf(fp, "************************** End Data Format ********************************\n");
		fprintf(fp, "\n");
		fprintf(fp, "************************ Calculated Parameters ****************************\n");
		fprintf(fp, "\n");
	}
	else {
		// End of output record line
		fprintf(fp, "\n");
	};

	return;
};

void PrintHeader(struct PathData path, struct ITURHFProp ITURHFP) {

	PrintITUHeader(ITURHFP.rptfp, asctime(ITURHFP.time), ITURHFP.P533ver, ITURHFP.P533compt, path.P372ver, path.P372compt);

	fprintf(fp, "***************************** P533 Input Parameters ****************************\n");
	fprintf(fp, "\n");
	fprintf(fp, "\t%s\n", path.name);
	fprintf(fp, "\tYear          : %d\n", path.year);
	fprintf(fp, "\tMonth         : %s\n", months[path.month]);
	fprintf(fp, "\tHour          : %d (hour UTC)\n", (path.hour==0?24:path.hour);// + 1);
	fprintf(fp, "\tSSN (R12)     : %d\n", path.SSN);
	fprintf(fp, "\tDistance      : %lf (km)\n", path.distance);
	fprintf(fp, "\tdmax          : %lf (km)\n", path.dmax);
	fprintf(fp, "\tTx power      : %lf\n", path.txpower);
	fprintf(fp, "\tTx Location     %s\n", path.txname);
	fprintf(fp, "\tTx latitude   : %10.6lf %c\n", fabs(path.L_tx.lat*R2D), NS(path.L_tx.lat));
	fprintf(fp, "\tTx longitude  : %10.6lf %c\n", fabs(path.L_tx.lng*R2D), EW(path.L_tx.lng));
	fprintf(fp, "\tRx Location     %s\n", path.rxname);
	fprintf(fp, "\tRx latitude   : %10.6lf %c\n", fabs(path.L_rx.lat*R2D), NS(path.L_rx.lat));
	fprintf(fp, "\tRx longitude  : %10.6lf %c\n", fabs(path.L_rx.lng*R2D), EW(path.L_rx.lng));
	fprintf(fp, "\tlocal time Rx : %d (hour UTC)\n", (int)fmod(((path.hour==0?24:path.hour) /*+ 1*/ + (int)(path.L_rx.lng/(15.0*D2R)))+24,24.0));
	fprintf(fp, "\tlocal time Tx : %d (hour UTC)\n", (int)fmod(((path.hour==0?24:path.hour) /*+ 1*/ + (int)(path.L_tx.lng/(15.0*D2R)))+24,24.0));
	fprintf(fp, "\tFrequency     : %lf\n", path.frequency);
	fprintf(fp, "\tBandwidth     : %lf\n", path.BW);

	if(path.Modulation == ANALOG) {
		strcpy(outstr, "ANALOG");
		fprintf(fp, "\tModulation : %s\n", outstr);
	}
	else {
		strcpy(outstr, "DIGITAL");
		fprintf(fp, "\tModulation : %s\n", outstr);
	};

	fprintf(fp, "\tRequired signal-to-noise ratio : %lf\n", path.SNRr);
	fprintf(fp, "\tRequired %% of month signal-to-noise ratio : % d\n", path.SNRXXp);
	fprintf(fp, "\tRequired signal-to-interference ratio : %lf\n", path.SIRr);

	if(path.noiseP.ManMadeNoise == CITY) {
		strcpy(outstr, "CITY");
		fprintf(fp, "\tMan-made noise : %s\n", outstr);
	}
	else if(path.noiseP.ManMadeNoise == RESIDENTIAL) {
		strcpy(outstr, "RESIDENTIAL");
		fprintf(fp, "\tMan-made noise : %s\n", outstr);
	}
	else if(path.noiseP.ManMadeNoise == RURAL) {
		strcpy(outstr, "RURAL");
		fprintf(fp, "\tMan-made noise : %s\n", outstr);
	}
	else if(path.noiseP.ManMadeNoise == QUIETRURAL) {
		strcpy(outstr, "QUIETRURAL");
		fprintf(fp, "\tMan-made noise : %s\n", outstr);
	}
	else if(path.noiseP.ManMadeNoise == NOISY) {
		strcpy(outstr, "NOISY");
		fprintf(fp, "\tMan-made noise : %s\n", outstr);
	}
	else if(path.noiseP.ManMadeNoise == QUIET) {
		strcpy(outstr, "QUIET");
		fprintf(fp, "\tMan-made noise : %s\n", outstr);
	}
	else {
		fprintf(fp, "\tMan-made noise : %lf (dB)\n", path.noiseP.ManMadeNoise);
	}

	if(path.Modulation == DIGITAL) {
		fprintf(fp, "\tFrequency dispersion for simple BCR (F0) : %lf\n", path.F0);			// Frequency dispersion at a level -10 dB relative to the peak signal amplitude
		fprintf(fp, "\tTime spread for simple BCR (T0) : %lf\n", path.T0);
		fprintf(fp, "\tRequired Amplitude ratio (A) : %lf\n", path.A);
		fprintf(fp, "\tTime window (usec) : %lf\n", path.TW);
		fprintf(fp, "\tFrequency window (Hz) : %lf\n", path.FW);
	};

	if(ITURHFP.AntennaOrientation == TX2RX) {
		fprintf(fp, "\tAntenna configuration : Transmitter main beam to receiver main beam\n");
	}
	else if(ITURHFP.AntennaOrientation == MANUAL) {
		fprintf(fp, "\tAntenna configuration : User determined\n");
	}
	else {
		fprintf(fp, "\tAntenna configuration : UNKNOWN\n");
	};

	if (path.SorL == SHORTPATH) {
		strcpy(outstr, "SHORTPATH");
	}
	else if (path.SorL == LONGPATH) {
		strcpy(outstr, "LONGPATH");
	}
	else {
		strcpy(outstr, "ERROR");
	};
	fprintf(fp, "\tPath Direction : %s\n", outstr);

	fprintf(fp, "\tTransmit antenna               %.40s\n", path.A_tx.Name);
	fprintf(fp, "\tTransmit antenna bearing     : %lf\n", ITURHFP.TXBearing*R2D);
	fprintf(fp, "\tTransmit antenna gain offset : %lf\n", ITURHFP.TXGOS);
	fprintf(fp, "\tReceive antenna                %.40s\n", path.A_rx.Name);
	fprintf(fp, "\tReceive antenna bearing      : %lf\n", ITURHFP.RXBearing*R2D);
	fprintf(fp, "\tReceive antenna gain offset  : %lf\n", ITURHFP.RXGOS);

	fprintf(fp, "\n");
	fprintf(fp, "************************ End P533 Input Parameters *****************************\n");
	fprintf(fp, "\n");
	fprintf(fp, "************************** ITURHFP Input Parameters *****************************\n");
	fprintf(fp, "\n");
	fprintf(fp, "\tUpper left (North West) latitude   : %10.6lf %c\n", fabs(ITURHFP.L_UL.lat*R2D), NS(ITURHFP.L_UL.lat*R2D));
	fprintf(fp, "\tUpper left (North West) longitude  : %10.6lf %c\n", fabs(ITURHFP.L_UL.lng*R2D), EW(ITURHFP.L_UL.lng*R2D));
	fprintf(fp, "\tLower right (South East) latitude  : %10.6lf %c\n", fabs(ITURHFP.L_LR.lat*R2D), NS(ITURHFP.L_LR.lat*R2D));
	fprintf(fp, "\tLower right (South East) longitude : %10.6lf %c\n", fabs(ITURHFP.L_LR.lng*R2D), EW(ITURHFP.L_LR.lng*R2D));
	fprintf(fp, "\tNumber of frequencies : %d\n", ITURHFP.ifrqend);
	fprintf(fp, "\tNumber of hours       : %d\n", ITURHFP.ihrend);
	fprintf(fp, "\tNumber of months      : %d\n", ITURHFP.imnthend);
	fprintf(fp, "\tLatitude increment    : %lf (deg)\n", ITURHFP.latinc*R2D);
	fprintf(fp, "\tLongitude increment   : %lf (deg)\n", ITURHFP.lnginc*R2D);
	fprintf(fp, "\n");
	fprintf(fp, "************************** ITURHFP Input Parameters *****************************\n");
	fprintf(fp, "\n");
	fprintf(fp, "******************************** Data Format ***********************************\n");
	fprintf(fp, "\n");

	return;

};

void PrintLastRecord(struct PathData path, struct ITURHFProp ITURHFP) {

	PrintRecord(path, ITURHFP, PRINT_DATA);

	fprintf(fp, "\n");
	fprintf(fp, "**************************End Calculated Parameters ***********************\n\n");

	return;

};

void function_RPT_D(struct PathData path, int option, int *col) {
	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column %02d: D - Path distance (km)\n", ++*col);
			break;
		case PRINT_RFC4180_HEADER:
			fprintf(fp, ",distance");
			++*col;
			break;
		case PRINT_DATA:
			fprintf(fp,",");
			fprintf(fp, DBLFIELD2, path.distance);
			break;
		case PRINT_RFC4180_DATA:
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD2, path.distance);
			break;
	};
	return;
};

void function_RPT_DMAX(struct PathData path, int option, int *col) {
	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column %02d: dmax - Path maximum hop distance (km)\n", ++*col);
			fprintf(fp, "Column %02d: ptick - Slant Path distance (km)\n", ++*col);
			break;
		case PRINT_RFC4180_HEADER:
			fprintf(fp, ",dmax,ptick");
			*col = *col + 2;
			break;
		case PRINT_DATA:
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.dmax);
			fprintf(fp,",");
			fprintf(fp, DBLFIELD2, path.ptick);
			break;
		case PRINT_RFC4180_DATA:
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.dmax);
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD2, path.ptick);
			break;
	};
	return;
};

void function_RPT_ELE(struct PathData path, int option, int *col) {
	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column %02d: ele - Path minimum Rx elevation angle (deg)\n", ++*col);
			break;
		case PRINT_RFC4180_HEADER:
			fprintf(fp, ",ele");
			++*col;
			break;
		case PRINT_DATA:
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.ele*R2D);
			break;
		case PRINT_RFC4180_DATA:
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.ele*R2D);
			break;
	};
	return;
};

void function_RPT_BMUF(struct PathData path, int option, int *col) {
	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column %02d: BMUF - Path basic MUF (MHz)\n", ++*col);
			break;
		case PRINT_RFC4180_HEADER:
			fprintf(fp, ",BMUF");
			++*col;
			break;
		case PRINT_DATA:
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.BMUF);
			break;
		case PRINT_RFC4180_DATA:
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.BMUF);
			break;
	};
	return;
};

void function_RPT_BMUFD(struct PathData path, int option, int *col) {
	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column %02d: MUF50 - 50%% Path basic MUF (MHz)\n", ++*col);
			fprintf(fp, "Column %02d: MUF90 - 90%% Path basic MUF (MHz)\n", ++*col);
			fprintf(fp, "Column %02d: MUF10 - 10%% Path basic MUF (MHz)\n", ++*col);
			break;
		case PRINT_RFC4180_HEADER:
			fprintf(fp, ",MUF50,MUF90,MUF10");
			*col = *col + 3;
			break;
		case PRINT_DATA:
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.MUF50);
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.MUF90);
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.MUF10);
			break;
		case PRINT_RFC4180_DATA:
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.MUF50);
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.MUF90);
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.MUF10);
			break;
	};

	return;
};

void function_RPT_OPMUF(struct PathData path, int option, int *col) {
	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column %02d: OPMUF - Operation MUF (MHz)\n", ++*col);
			break;
		case PRINT_RFC4180_HEADER:
			fprintf(fp, ",OPMUF");
			++*col;
			break;
		case PRINT_DATA:
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.OPMUF);
			break;
		case PRINT_RFC4180_DATA:
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.OPMUF);
			break;
	};

	return;

};

void function_RPT_OPMUFD(struct PathData path, int option, int *col) {

	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column %02d: OPMUF90 - 90%% Operation MUF (MHz)\n", ++*col);
			fprintf(fp, "Column %02d: OPMUF10 - 10%% Operation MUF (MHz)\n", ++*col);
			break;
		case PRINT_RFC4180_HEADER:
			fprintf(fp, ",OPMUF90,OPMUF10");
			*col = *col + 2;
			break;
		case PRINT_DATA:
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.OPMUF90);
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.OPMUF10);
			break;
		case PRINT_RFC4180_DATA:
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.OPMUF90);
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.OPMUF10);
			break;
	};
	return;
};

void function_RPT_N0_F2(struct PathData path, int option, int *col) {
	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column %02d: Lowest order mode for the F2 layer\n", ++*col);
			break;
		case PRINT_RFC4180_HEADER:
			fprintf(fp, ",n0_F2");
			++*col;
			break;
		case PRINT_DATA:
			fprintf(fp,",");
			if(path.n0_F2 != NOLOWESTMODE) {
				sprintf(outstr, "  %1dF2 ", path.n0_F2+1);
			}
			else {
				sprintf(outstr, " NONE ");
			};
			fprintf(fp, STRFIELD, outstr);
			break;
		case PRINT_RFC4180_DATA:
			fprintf(fp,",");
			if(path.n0_F2 != NOLOWESTMODE) {
				sprintf(outstr, "%dF2", path.n0_F2+1);
			}
			else {
				sprintf(outstr, "NONE");
			};
			fprintf(fp, RFC4180_STRFIELD, outstr);
			break;
	};
	return;
};

void function_RPT_N0_E(struct PathData path, int option, int *col) {
	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column %02d: Lowest order mode for the E layer\n", ++*col);
			break;
		case PRINT_RFC4180_HEADER:
			fprintf(fp, ",n0_E");
			++*col;
			break;
		case PRINT_DATA:
			fprintf(fp,",");
			if(path.n0_E != NOLOWESTMODE) {
				sprintf(outstr, "   %1dE ", path.n0_E+1);
			}
			else {
				sprintf(outstr, " NONE ");
			};
			fprintf(fp, STRFIELD, outstr);
			break;
		case PRINT_RFC4180_DATA:
			fprintf(fp,",");
			if(path.n0_E != NOLOWESTMODE) {
				sprintf(outstr, "%dE", path.n0_E+1);
			}
			else {
				sprintf(outstr, "NONE");
			};
			fprintf(fp, RFC4180_STRFIELD, outstr);
			break;
	};
	return;
};

void function_RPT_E(struct PathData path, int option, int *col) {
	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column %02d: E - Path Field Strength (dB(1uV/m))\n", ++*col);
			break;
		case PRINT_RFC4180_HEADER:
			fprintf(fp, ",Ep");
			++*col;
			break;
		case PRINT_DATA:
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.Ep);
			break;
		case PRINT_RFC4180_DATA:
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.Ep);
			break;
	};
	return;
};

void function_RPT_GRW(struct PathData path, int option, int *col) {
	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column %02d: Grw - Receive Antenna Gain (dbi)\n", ++*col);
			break;
		case PRINT_RFC4180_HEADER:
			fprintf(fp, ",Grw");
			++*col;
			break;
		case PRINT_DATA:
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.Grw);
			break;
		case PRINT_RFC4180_DATA:
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.Grw);
			break;
	};
	return;
};

void function_RPT_PR(struct PathData path, int option, int *col) {
	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column %02d: Pr - Median receiver power (dB)\n", ++*col);
			break;
		case PRINT_RFC4180_HEADER:
			fprintf(fp, ",PR");
			++*col;
			break;
		case PRINT_DATA:
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.Pr);
			break;
		case PRINT_RFC4180_DATA:
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.Pr);
			break;
	};
	return;
};

void function_RPT_NOISESOURCES(struct PathData path, int option, int *col) {
	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column %02d: FaA - Atmospheric noise (dB)\n", ++*col);
			fprintf(fp, "Column %02d: FaM - Man-made noise (dB)\n", ++*col);
			fprintf(fp, "Column %02d: FaG - Galactic noise (dB)\n", ++*col);
			break;
		case PRINT_RFC4180_HEADER:
			fprintf(fp, ",FaA,FaM,FaG");
			*col = *col + 3;
			break;
		case PRINT_DATA:
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.noiseP.FaA);
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.noiseP.FaM);
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.noiseP.FaG);
			break;
		case PRINT_RFC4180_DATA:
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.noiseP.FaA);
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.noiseP.FaM);
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.noiseP.FaG);
			break;
	};
	return;
};

void function_RPT_NOISESOURCESD(struct PathData path, int option, int *col) {
	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column %02d: DuA - Upper decile deviation of atmospheric noise (dB)\n", ++*col);
			fprintf(fp, "Column %02d: DlA - Lower decile deviation of atmospheric noise (dB)\n", ++*col);
			fprintf(fp, "Column %02d: DuM - Upper decile deviation of man-made noise (dB)\n", ++*col);
			fprintf(fp, "Column %02d: DlM - Lower decile deviation of man-made noise (dB)\n", ++*col);
			fprintf(fp, "Column %02d: DuG - Upper decile deviation of atmospheric noise (dB)\n", ++*col);
			fprintf(fp, "Column %02d: DlG - Lower decile deviation of atmospheric noise (dB)\n", ++*col);
			break;
		case PRINT_RFC4180_HEADER:
			fprintf(fp, ",DuA,DlA,DuM,DlM,DuG,DlG");
			*col = *col + 6;
			break;
		case PRINT_DATA:
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.noiseP.DuA);
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.noiseP.DlA);
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.noiseP.DuM);
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.noiseP.DlM);
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.noiseP.DuG);
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.noiseP.DlG);
			break;
		case PRINT_RFC4180_DATA:
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.noiseP.DuA);
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.noiseP.DlA);
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.noiseP.DuM);
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.noiseP.DlM);
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.noiseP.DuG);
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.noiseP.DlG);
			break;
	};
	return;
};

void function_RPT_NOISETOTALD(struct PathData path, int option, int *col) {
	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column %02d: DuT - Upper decile deviation of total noise (dB)\n", ++*col);
			fprintf(fp, "Column %02d: DlT - Lower decile deviation of total noise (dB)\n", ++*col);
			break;
		case PRINT_RFC4180_HEADER:
			fprintf(fp, ",DuT,DlT");
			*col = *col + 2;
			break;
		case PRINT_DATA:
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.noiseP.DuT);
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.noiseP.DlT);
			break;
		case PRINT_RFC4180_DATA:
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.noiseP.DuT);
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.noiseP.DlT);
			break;
	};
	return;
};

void function_RPT_NOISETOTAL(struct PathData path, int option, int *col) {
	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column %02d: FamT - Total noise (dB)\n", ++*col);
			break;
		case PRINT_RFC4180_HEADER:
			fprintf(fp, ",FamT");
			++*col;
			break;
		case PRINT_DATA:
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.noiseP.FamT);
			break;
		case PRINT_RFC4180_DATA:
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.noiseP.FamT);
			break;
	};
	return;
};

void function_RPT_SNR(struct PathData path, int option, int *col) {
	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column %02d: SNR - Median signal-to-noise ratio (dB)\n", ++*col);
			break;
		case PRINT_RFC4180_HEADER:
			fprintf(fp, ",SNR");
			++*col;
			break;
		case PRINT_DATA:
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.SNR);
			break;
		case PRINT_RFC4180_DATA:
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.SNR);
			break;
	};
	return;
};

void function_RPT_SNRD(struct PathData path, int option, int *col) {
	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column %02d: DuSN - Upper decile deviation of signal-to-noise ratio (dB)\n", ++*col);
			fprintf(fp, "Column %02d: DlSN - Lower decile deviation of signal-to-noise ratio (dB)\n", ++*col);
			break;
		case PRINT_RFC4180_HEADER:
			fprintf(fp, ",DuSN,DlSN");
			*col = *col + 2;
			break;
		case PRINT_DATA:
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.DuSN);
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.DlSN);
			break;
		case PRINT_RFC4180_DATA:
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.DuSN);
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.DlSN);
			break;
	};
	return;
};

void function_RPT_SNRXX(struct PathData path, int option, int *col) {
	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column %02d: SNRXXp - Signal-to-noise ratio at %0d%% of month\n", ++*col, path.SNRXXp);
			break;
		case PRINT_RFC4180_HEADER:
			fprintf(fp, ",SNRXXp");
			++*col;
			break;
		case PRINT_DATA:
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.SNRXX);
			break;
		case PRINT_RFC4180_DATA:
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.SNRXX);
			break;
	};
	return;
};

void function_RPT_SIR(struct PathData path, int option, int *col) {
	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column %02d: SIR - Signal-to-interference ratio (dB)\n", ++*col);
			break;
		case PRINT_RFC4180_HEADER:
			fprintf(fp, ",SIR");
			++*col;
			break;
		case PRINT_DATA:
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.SIR);
			break;
		case PRINT_RFC4180_DATA:
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.SIR);
			break;
	};
	return;
};

void function_RPT_SIRD(struct PathData path, int option, int *col) {
	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column %02d: DuSI - Upper decile deviation of signal-to-interference ratio (dB)\n", ++*col);
			fprintf(fp, "Column %02d: DlSI - Lower decile deviation of signal-to-interference ratio (dB)\n", ++*col);
			break;
		case PRINT_RFC4180_HEADER:
			fprintf(fp, ",DuSI,DlSI");
			*col = *col + 2;
			break;
		case PRINT_DATA:
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.DuSI);
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.DlSI);
			break;
		case PRINT_RFC4180_DATA:
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.DuSI);
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.DlSI);
			break;
	};
	return;
};

void function_RPT_RSN(struct PathData path, int option, int *col) {
	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column %02d: RSN - Probability that the required SNR is achieved (%%)\n", ++*col);
			fprintf(fp, "Column %02d: RT - Probability that the required time spread T0 is not exceeded (%%)\n", ++*col);
			fprintf(fp, "Column %02d: RF - Probability that the required frequency spread f0 is not exceeded (%%)\n", ++*col);
			break;
		case PRINT_RFC4180_HEADER:
			fprintf(fp, ",RSN,RT,RF");
			*col = *col + 3;
			break;
		case PRINT_DATA:
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.RSN);
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.RT);
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.RF);
			break;
		case PRINT_RFC4180_DATA:
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.RSN);
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.RT);
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.RF);
			break;
	};
	return;
};

void function_RPT_BCR(struct PathData path, int option, int *col) {
	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column %02d: BCR - Basic circuit reliability (%%)\n", ++*col);
			break;
		case PRINT_RFC4180_HEADER:
		  fprintf(fp, ",BCR");
			++*col;
			break;
		case PRINT_DATA:
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.BCR);
			break;
		case PRINT_RFC4180_DATA:
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.BCR);
			break;
	};
	return;
};

void function_RPT_OCR(struct PathData path, int option, int *col) {
	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column %02d: OCR - Overall circuit reliability not considering scattering (%%)\n", ++*col);
			break;
		case PRINT_RFC4180_HEADER:
			fprintf(fp, ",OCR");
			++*col;
			break;
		case PRINT_DATA:
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.OCR);
			break;
		case PRINT_RFC4180_DATA:
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.OCR);
			break;
	};
	return;
};

void function_RPT_OCRS(struct PathData path, int option, int *col) {

	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column %02d: OCRs - Overall circuit reliability considering scattering (%%)\n", ++*col);
			fprintf(fp, "Column %02d: Probocc - Probability of scattering (%%)\n", ++*col);
			break;
		case PRINT_RFC4180_HEADER:
			fprintf(fp, ",OCRs,probocc");
			*col = *col + 2;
			break;
		case PRINT_DATA:
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.OCRs);
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.probocc);
			break;
		case PRINT_RFC4180_DATA:
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.OCRs);
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.probocc);
			break;
	};
	return;
};

void function_RPT_MIR(struct PathData path, int option, int *col) {
	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column %02d: MIR - Multimode Interference (%%)\n", ++*col);
			break;
		case PRINT_RFC4180_HEADER:
			fprintf(fp, ",MIR");
			++*col;
			break;
		case PRINT_DATA:
			fprintf(fp,",");
			fprintf(fp, DBLFIELD, path.MIR);
			break;
		case PRINT_RFC4180_DATA:
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD, path.MIR);
			break;
	};
	return;
};

void function_RPT_RXLOCATION(struct PathData path, int option, int *col) {
	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column %02d: Receiver latitude (deg)\n", ++*col);
			fprintf(fp, "Column %02d: Receiver longitude (deg)\n", ++*col);
			break;
		case PRINT_RFC4180_HEADER:
			fprintf(fp, ",rxlat,rxlng");
			*col = *col+2;
		  break;
		case PRINT_DATA:
			fprintf(fp,",");
			fprintf(fp, DBLFIELD1, path.L_rx.lat*R2D);
			fprintf(fp,",");
			fprintf(fp, DBLFIELD1, path.L_rx.lng*R2D);
			break;
		case PRINT_RFC4180_DATA:
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD1, path.L_rx.lat*R2D);
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD1, path.L_rx.lng*R2D);
			break;
	};
	return;
};

void function_RPT_ESL(struct PathData path, int option, int *col) {
	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column %02d: Short Path (<=7000 km) Field Strength (dB(1uV/m))\n", ++*col);
			fprintf(fp, "Column %02d: Long Path (>9000km) Field Strength (dB(1uV/m))\n", ++*col);
			break;
		case PRINT_RFC4180_HEADER:
			fprintf(fp, ",Es,El");
			*col = *col + 2;
			break;
		case PRINT_DATA:
			fprintf(fp,",");
			fprintf(fp, DBLFIELD1, path.Es);
			fprintf(fp,",");
			fprintf(fp, DBLFIELD1, path.El);
			break;
		case PRINT_RFC4180_DATA:
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD1, path.Es);
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD1, path.El);
			break;
	};
	return;
};

void function_RPT_LONG(struct PathData path, int option, int *col) {
	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column %02d: Free-space Field Strength 3 MW e.i.r.p. (dB(1uV/m)\n", ++*col);
			fprintf(fp, "Column %02d: Increased Long Distance Field Strength due to Focusing (dB)\n", ++*col);
			fprintf(fp, "Column %02d: \"Not otherwise included loss\" (dB)\n", ++*col);
			fprintf(fp, "Column %02d: Upper Reference Frequency (MHz)\n", ++*col);
			fprintf(fp, "Column %02d: Lower Reference Frequency (MHz)\n", ++*col);
			fprintf(fp, "Column %02d: Correction Factor at T + d0/2\n", ++*col);
			fprintf(fp, "Column %02d: Correction Factor at R - d0/2\n", ++*col);
			fprintf(fp, "Column %02d: Max Antenna Gain G_tl (0 - 8 deg)\n", ++*col);
			fprintf(fp, "Column %02d: Max Antenna Gain G_rw (0 - 8 deg)\n", ++*col);
			fprintf(fp, "Column %02d: Mean gyrofrequency\n", ++*col);
			fprintf(fp, "Column %02d: Scale factor f(f ,fL, fM, fH)\n", ++*col);
			break;
		case PRINT_RFC4180_HEADER:
			fprintf(fp, ",E0,Gap,Ly,fM,fL,K0,K1,Gtl,Grw,fH,Fscale");
			*col = *col + 11;
			break;
		case PRINT_DATA:
			fprintf(fp,",");
			fprintf(fp, DBLFIELD1, path.E0);
			fprintf(fp,",");
			fprintf(fp, DBLFIELD1, path.Gap);
			fprintf(fp,",");
			fprintf(fp, DBLFIELD1, path.Ly);
			fprintf(fp,",");
			fprintf(fp, DBLFIELD1, path.fM);
			fprintf(fp,",");
			fprintf(fp, DBLFIELD1, path.fL);
			fprintf(fp,",");
			fprintf(fp, DBLFIELD1, path.K[0]);
			fprintf(fp,",");
			fprintf(fp, DBLFIELD1, path.K[1]);
			fprintf(fp,",");
			fprintf(fp, DBLFIELD1, path.Gtl);
			fprintf(fp,",");
			fprintf(fp, DBLFIELD1, path.Grw);
			fprintf(fp, ",");
			fprintf(fp, DBLFIELD1, path.fH);
			fprintf(fp,",");
			fprintf(fp, DBLFIELD1, path.F);
			break;
		case PRINT_RFC4180_DATA:
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD1, path.E0);
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD1, path.Gap);
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD1, path.Ly);
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD1, path.fM);
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD1, path.fL);
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD1, path.K[0]);
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD1, path.K[1]);
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD1, path.Gtl);
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD1, path.Grw);
			fprintf(fp, ",");
			fprintf(fp, RFC4180_DBLFIELD1, path.fH);
			fprintf(fp,",");
			fprintf(fp, RFC4180_DBLFIELD1, path.F);
			break;
	};
	return;
};

void function_RPT_DOMMODE(struct PathData path, int option, int *col) {

	switch(option) {
		case PRINT_HEADER:
			fprintf(fp, "Column %02d: Dominant mode\n", ++*col);
			fprintf(fp, "Column %02d: Dominant mode - Elevation angle (deg)\n", ++*col);
			fprintf(fp, "Column %02d: Dominant mode - Delay (mS)\n", ++*col);
			fprintf(fp, "Column %02d: Dominant mode - Loss (dB)\n", ++*col);
			fprintf(fp, "Column %02d: Dominant mode - Propagation probability (%%)\n", ++*col);
			fprintf(fp, "Column %02d: Dominant mode - Virtual height (km)\n", ++*col);
			fprintf(fp, "Column %02d: Dominant mode - Median received power (dB)\n", ++*col);
			fprintf(fp, "Column %02d: Dominant mode - Receiver Antenna Gain (dBi)\n", ++*col);
			fprintf(fp, "Column %02d: Dominant mode - Field Strength (dB(1uV/m))\n", ++*col);
			fprintf(fp, "Column %02d: Dominant mode - Basic MUF (MHz)\n", ++*col);
			break;
		case PRINT_RFC4180_HEADER:
			fprintf(fp, ",DMidx,DMele,DMtau,DMLb,DMFprob,DMhr,DMPrw,DMGrw,DMEw,DMBMUF");
			*col = *col + 10;
			break;
		case PRINT_DATA:
			if(path.DMidx < MAXEMDS) { // E mode dominant
				sprintf(outstr,  "   %1dE ", path.DMidx+1);
			}
			else if((path.DMidx >= MAXEMDS) && (path.DMidx <= MAXMDS)) { // F2 mode dominant
				sprintf(outstr, "  %1dF2 ", path.DMidx-2);
			}
			else { // There is no dominant mode
				sprintf(outstr, " NONE ");
			};

			if(path.DMidx <= MAXMDS) {
				fprintf(fp,",");
				fprintf(fp, STRFIELD, outstr);
				fprintf(fp,",");
				fprintf(fp, DBLFIELD, path.DMptr->ele*R2D);
				fprintf(fp,",");
				fprintf(fp, DBLFIELD, path.DMptr->tau);
				fprintf(fp,",");
				fprintf(fp, DBLFIELD, path.DMptr->Lb);
				fprintf(fp,",");
				fprintf(fp, DBLFIELD, path.DMptr->Fprob);
				fprintf(fp,",");
				fprintf(fp, DBLFIELD, path.DMptr->hr);
				fprintf(fp,",");
				fprintf(fp, DBLFIELD, path.DMptr->Prw);
				fprintf(fp,",");
				fprintf(fp, DBLFIELD, path.DMptr->Grw);
				fprintf(fp,",");
				fprintf(fp, DBLFIELD, path.DMptr->Ew);
				fprintf(fp,",");
				fprintf(fp, DBLFIELD, path.DMptr->BMUF);
			}
			else {
				fprintf(fp,",");
				fprintf(fp, STRFIELD, outstr);
				fprintf(fp,",");
				fprintf(fp, DBLFIELD, 0.0);
				fprintf(fp,",");
				fprintf(fp, DBLFIELD, 0.0);
				fprintf(fp,",");
				fprintf(fp, DBLFIELD, 0.0);
				fprintf(fp,",");
				fprintf(fp, DBLFIELD, 0.0);
				fprintf(fp,",");
				fprintf(fp, DBLFIELD, 0.0);
				fprintf(fp,",");
				fprintf(fp, DBLFIELD, 0.0);
				fprintf(fp,",");
				fprintf(fp, DBLFIELD, 0.0);
				fprintf(fp,",");
				fprintf(fp, DBLFIELD, 0.0);
				fprintf(fp,",");
				fprintf(fp, DBLFIELD, 0.0);
			};
			break;
		case PRINT_RFC4180_DATA:
			if(path.DMidx < MAXEMDS) { // E mode dominant
				sprintf(outstr,  "%dE", path.DMidx+1);
			}
			else if((path.DMidx >= MAXEMDS) && (path.DMidx <= MAXMDS)) { // F2 mode dominant
				sprintf(outstr, "%dF2", path.DMidx-2);
			}
			else { // There is no dominant mode
				sprintf(outstr, "NONE");
			};

			if(path.DMidx <= MAXMDS) {
				fprintf(fp,",");
				fprintf(fp, RFC4180_STRFIELD, outstr);
				fprintf(fp,",");
				fprintf(fp, RFC4180_DBLFIELD, path.DMptr->ele*R2D);
				fprintf(fp,",");
				fprintf(fp, RFC4180_DBLFIELD, path.DMptr->tau);
				fprintf(fp,",");
				fprintf(fp, RFC4180_DBLFIELD, path.DMptr->Lb);
				fprintf(fp,",");
				fprintf(fp, RFC4180_DBLFIELD, path.DMptr->Fprob);
				fprintf(fp,",");
				fprintf(fp, RFC4180_DBLFIELD, path.DMptr->hr);
				fprintf(fp,",");
				fprintf(fp, RFC4180_DBLFIELD, path.DMptr->Prw);
				fprintf(fp,",");
				fprintf(fp, RFC4180_DBLFIELD, path.DMptr->Grw);
				fprintf(fp,",");
				fprintf(fp, RFC4180_DBLFIELD, path.DMptr->Ew);
				fprintf(fp,",");
				fprintf(fp, RFC4180_DBLFIELD, path.DMptr->BMUF);
			}
			else {
				fprintf(fp,",");
				fprintf(fp, RFC4180_STRFIELD, outstr);
				fprintf(fp,",");
				fprintf(fp, RFC4180_DBLFIELD, 0.0);
				fprintf(fp,",");
				fprintf(fp, RFC4180_DBLFIELD, 0.0);
				fprintf(fp,",");
				fprintf(fp, RFC4180_DBLFIELD, 0.0);
				fprintf(fp,",");
				fprintf(fp, RFC4180_DBLFIELD, 0.0);
				fprintf(fp,",");
				fprintf(fp, RFC4180_DBLFIELD, 0.0);
				fprintf(fp,",");
				fprintf(fp, RFC4180_DBLFIELD, 0.0);
				fprintf(fp,",");
				fprintf(fp, RFC4180_DBLFIELD, 0.0);
				fprintf(fp,",");
				fprintf(fp, RFC4180_DBLFIELD, 0.0);
				fprintf(fp,",");
				fprintf(fp, RFC4180_DBLFIELD, 0.0);
			};
			break;
	};

	return;
};

char EW(double lng) {

	if(lng < 0.0) return 'W';
	else if(lng > 0.0) return 'E';
	else return '=';

};

char NS(double lat) {

	if(lat < 0.0) return 'S';
	else if(lat > 0.0) return 'N';
	else return '=';

};

