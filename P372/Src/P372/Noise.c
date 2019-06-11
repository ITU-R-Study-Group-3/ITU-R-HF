#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

// Local includes
#include "common.h"
#include "Noise.h"

// Local prototypes
void AtmosphericNoise(struct NoiseParams *noiseP, int hour, double lng,
											double lat, double frequency);
void GalacticNoise(struct NoiseParams *noiseP, double frequency);
void ManMadeNoise(struct NoiseParams *noiseP, double frequency);
void GetFamParameters(struct NoiseParams *noiseP, struct FamStats *FS,											double lng, double lat, double frequency);
// End Local prototypes

int Noise(struct NoiseParams *noiseP, int hour, double lng, double lat, double frequency) {
	/*

	 	Noise() Determines atmospheric, man-made and galactic noise and the associated decile values
	 		The routine AtmosphericNoise() is the most involved calculation, the other two
	 		routines GalacticNoise() and ManMadeNoise() were written for consistancy. The Noise()
	 		routine also determines the combined noise.

	 		INPUT
	 			struct NoiseParams *noiseP
				double manMadeNoise
				int hour
				double lng
				double lat
				double frequency

	 		OUTPUT
	 			noiseP->DlT - Upper decile total noise
	 			noiseP->FamT - Total noise
	 			noiseP->DuT - Lower decile total noise

	 			via AtmosphericNoise()
	 			noiseP->FaA - Atmospheric noise
	 			noiseP->DuA - Upper decile deviation of atmospheric noise
	 			noiseP->DlA - Lower decile deviation of atmospheric noise

	 			via GalacticNoise()
	 			noiseP->FaG - Galactic noise
	 			noiseP->DuG - Upper decile deviation of galactic noise
	 			noiseP->DlG - Lower decile deviation of galactic noise

	 			via ManMadeNoise()
	 			noiseP->FaM - Man-made noise
	 			noiseP->DuM - Upper decile deviation of man-made noise
	 			noiseP->DlM - Lower decile deviation of man-made noise

			SUBROUTINES
				AtmosphericNoise()
				GalacticNoise()
				ManMadeNoise()

	 */

	double sigmaA; // Standard deviation of the atmospheric noise
	double sigmaG; // Standard deviation of the galactic noise
	double sigmaM; // Standard deviation of the man-made noise
	double c; // Constant for the calculation of the combined noise
	double alphaT;
	double betaT;
	double gammaT;
	double sigmaT;
	double FamTu, FamTl;

	// ******************************************************************* //
	// **************** Noise Calulation Override ************************ //
	// ******************************************************************* //
	if (noiseP->ManMadeNoise < 0.0) {
		// If the manMadeNoise is negative then the user wants to override the
		// noise calculation. There are no decile values or noise components
		// Althought they should already be initialized, set the all of the statistical
		// noise components to 0.0 to make it clear that you are canceling the noise
		// calculation
		// Then set the total noise to the desired values and return

		noiseP->DuA = 0.0;
		noiseP->DuG = 0.0;
		noiseP->DuM = 0.0;

		noiseP->DlA = 0.0;
		noiseP->DlG = 0.0;
		noiseP->DlM = 0.0;

		noiseP->FaA = 0.0;
		noiseP->FaG = 0.0;
		noiseP->FaM = noiseP->ManMadeNoise;

		noiseP->DuT = 0.0;
		noiseP->DlT = 0.0;

		noiseP->FamT = -noiseP->ManMadeNoise;

		return RTN_NOISEMANMADEOK;

	};
	// *********************************************************************** //
	// **************** End Noise Calulation Override ************************ //
	// *********************************************************************** //

	AtmosphericNoise(noiseP, hour, lng, lat, frequency);

	GalacticNoise(noiseP, frequency);

	ManMadeNoise(noiseP, frequency);

	// Determine the combined noise according to
	// ITU-R P.372-10 Section 8 "The Combination of Noises from Several Sources"
	// Find the upper decile sigmaT
	sigmaA = noiseP->DuA/1.282;
	sigmaG = 1.56;
	sigmaM = noiseP->DuM/1.282;

	c = 10.0/log(10.0);

	alphaT = exp((noiseP->FaA/c)+(pow(sigmaA, 2)/(2.0*pow(c, 2)))) +
			 exp((noiseP->FaG/c)+(pow(sigmaG, 2)/(2.0*pow(c, 2)))) +
			 exp((noiseP->FaM/c)+(pow(sigmaM, 2)/(2.0*pow(c, 2))));

	betaT =  pow(exp((noiseP->FaA/c)+(pow(sigmaA, 2)/(2.0*pow(c, 2)))),2)*(exp(pow(sigmaA/c, 2)) - 1.0) +
		     pow(exp((noiseP->FaG/c)+(pow(sigmaG, 2)/(2.0*pow(c, 2)))),2)*(exp(pow(sigmaG/c, 2)) - 1.0) +
			 pow(exp((noiseP->FaM/c)+(pow(sigmaM, 2)/(2.0*pow(c, 2)))),2)*(exp(pow(sigmaM/c, 2)) - 1.0);

	gammaT = exp(noiseP->FaA/c) + exp(noiseP->FaG/c) + exp(noiseP->FaM/c);

	if((noiseP->DuA > 12.0) || (noiseP->DuG > 12.0) || (noiseP->DuM > 12.0)) {
		sigmaT = c*sqrt(2.0*log(alphaT/gammaT));

	}
	else {
		sigmaT = c*sqrt(log(1.0+(betaT/pow(alphaT, 2))));
	};

	FamTu =  c*(log(alphaT) - (pow(sigmaT, 2)/(2.0*pow(c, 2))));

	noiseP->DuT = 1.282*sigmaT;

	// Find the lower decile sigmaT
	sigmaA = noiseP->DlA/1.282;
	sigmaG = 1.56;
	sigmaM = noiseP->DlM/1.282;

	c = 10.0/log(10.0);

	alphaT = exp((noiseP->FaA/c)+(pow(sigmaA, 2)/(2.0*pow(c, 2)))) +
			 exp((noiseP->FaG/c)+(pow(sigmaG, 2)/(2.0*pow(c, 2)))) +
			 exp((noiseP->FaM/c)+(pow(sigmaM, 2)/(2.0*pow(c, 2))));

	betaT = pow(exp((noiseP->FaA/c)+(pow(sigmaA, 2)/(2.0*pow(c, 2)))),2)*(exp(pow(sigmaA/c, 2)) - 1.0) +
			pow(exp((noiseP->FaG/c)+(pow(sigmaG, 2)/(2.0*pow(c, 2)))),2)*(exp(pow(sigmaG/c, 2)) - 1.0) +
			pow(exp((noiseP->FaM/c)+(pow(sigmaM, 2)/(2.0*pow(c, 2)))),2)*(exp(pow(sigmaM/c, 2)) - 1.0);

	gammaT = exp(noiseP->FaA/c) + exp(noiseP->FaG/c) + exp(noiseP->FaM/c);

	if((noiseP->DlA > 12.0) || (noiseP->DlG > 12.0) || (noiseP->DlM > 12.0)) {
		sigmaT = c*sqrt(2.0*log(alphaT/gammaT));
	}
	else {
		sigmaT = c*sqrt(log(1.0+(betaT/pow(alphaT, 2))));
	};

	FamTl =  c*(log(alphaT) - (pow(sigmaT, 2)/(2.0*pow(c, 2))));

	noiseP->DlT = 1.282*sigmaT;

	noiseP->FamT = min(FamTu, FamTl); // Worse-case noise

	return RTN_NOISEOK;

};

void AtmosphericNoise(struct NoiseParams *noiseP, int hour, double lng,
											double lat, double frequency) {

	/*

	  AtmosphericNoise() The method implemented here is based on REC533() code and not an ITU recommendation. Although
	 		it does provide the numbers that are required. Specifically the statistics of Fam discribed in ITU-R P.372-10
	 		section 4 Figs. 15c to 38c.

	 		The calculation of the atmospheric noise is based on using one or two four
	 		hour time blocks to determine the noise. The following code sets the two counters
	 		FS_now.tmblk and FS_adj.tmblk to the correct 4 hour time block, See P.372-9 for the six 4hr
	 		time blocks graph sets
	 		The time blocks are determined by first choosing the timeblock that contains LMT of the reciever, lrxmt.
	 		The time block, FS_now.tmblk, is set to the index of the timeblock that contains lrxmt. The "adjacent" time block,
	 		FS_adj.tmblk is then determined in the following way.
	 		If lrsmt occurs in the first 2 hours of the FS_now.tmblk'th time block then FS_adj.tmblk is set to the timeblock previous
	 		to FS_now.tmblk. If lrxmt occurs in the 3rd hour
	 		of the FS_now.tmblk'th time block then FS_adj.tmblk is set to FS_now.tmblk, i.e., FS_now.tmblk = FS_adj.tmblk.
	 		If lrxmt occurs in the 4th hour of the time block then FS_adj.tmblk is set to the next timeblock.
	 			tmblk = time block of interest
	 			tmblk     Reciever LMT hours
       			  0            0000-0400
       			  1            0400-0800
	 			  2            0800-1200
       			  3            1200-1600
       			  4            1600-2000
       			  5            2000-2400

	 		INPUT
	 			struct NoiseParams *noiseP
				int hour
				double lng
				double lat
				double frequency

	 		OUTPUT
	 			noiseP->FaA - Atmospheric noise
	 			noiseP->DuA - Upper decile deviation of atmospheric noise
	 			noiseP->DlA - Lower decile deviation of atmospheric noise

			SUBROUTINES
				GetFamParameters()

	 	This routine is based on portions of the REC533() routines: GENFAM(), GENOIS1(), ANOIS1() and NOISY().

	 */

	double lrxmt; // Local reciever mean time
	double slp; // Interpolation factor
	double fa; // Linear noise power above kTB in 1 MHz

	struct FamStats FS_now; //
	struct FamStats FS_adj; //

	// Find the local mean time at the reciever
	// Note: lrxmt is 1 to 24 and not 0 to 24
	// This is an artifact from ANOIS1()
	lrxmt = (float)hour + 1.0 + lng /(15.0*D2R);
	if(lrxmt < 0.0) lrxmt += 24.0; // roll over the time
	if(lrxmt > 24.0) lrxmt -= 24.0; //

	// The atmospheric noise is determined by i) finding the atmospheric noise at the current time
	// block at the reciever local mean time, ii) finding the noise for the "adjacent" time block and
	// then iii) iterating between the two noise values.

	// Determine the timeblock, tmblk, and "adjacent" time block, FS_adj.tmblk
	if(lrxmt < 20.0) {
		FS_now.tmblk = (int)(lrxmt/4.0 + 1.0); // Set the timeblock to the correct 4 hour block
	}
	else { // lrxmt >= 20.0
		FS_now.tmblk = 6;
	};

	// Determine "adjacent" time block, FS_adj.tmblk
	FS_adj.tmblk = (4*FS_now.tmblk - 2)/2;

	if(lrxmt < FS_adj.tmblk) {
		FS_adj.tmblk = FS_now.tmblk - 1;
	}
	else if(lrxmt == FS_adj.tmblk) {
		FS_adj.tmblk = FS_now.tmblk;
	}
	else if(lrxmt > FS_adj.tmblk) {
		FS_adj.tmblk = FS_now.tmblk + 1;
	};

	if(FS_adj.tmblk <= 0) {
		FS_adj.tmblk = 6; // Error condition
	}
	else if(FS_adj.tmblk > 6) {
		FS_adj.tmblk = 1; // If FS_adj.tmblk is greater than 6 roll it back to 1
	};

	// The time block calculation above is from REC533 ANOIS1.FOR where
	//		KJ = FS_now.tmblk
	//		JK = FS_adj.tmblk
	// Since the time blocks will be used as indexes into C arrays they must be decremented
	FS_adj.tmblk = FS_adj.tmblk - 1;
	FS_now.tmblk = FS_now.tmblk - 1;

	GetFamParameters(noiseP, &FS_now, lng, lat, frequency);
	GetFamParameters(noiseP, &FS_adj, lng, lat, frequency);

	// Interpolate is based on the local reciever mean time, lrxmt, and the 4 hour timeblock
	slp = fmod(lrxmt, 4.0)/4.0;

 	fa = pow(10.0, (FS_now.FA / 10.0)) + (pow(10.0, (FS_adj.FA / 10.0)) - pow(10.0, (FS_now.FA / 10.0)))*slp;
	noiseP->FaA = 10.0 * log10(fa);

	fa = pow(10.0, (FS_now.Du / 10.0)) + (pow(10.0, (FS_adj.Du / 10.0)) - pow(10.0, (FS_now.Du / 10.0)))*slp;
	noiseP->DuA = 10.0 * log10(fa);

	fa = pow(10.0, (FS_now.Dl/10.0)) + (pow(10.0, (FS_adj.Dl / 10.0)) - pow(10.0, (FS_now.Dl / 10.0)))*slp;
	noiseP->DlA = 10.0 * log10(fa);

	return;

};

void GetFamParameters(struct NoiseParams *noiseP, struct FamStats *FS,
											double lng, double lat, double frequency) {
	/*

	  GetFamParameters() - Finds the atmospheric noise parameters is given in Figs. 15c to 38c in
	 		P.372-10

	 		INPUT
	 			struct NoiseParams *noiseP
				struct FamStats *FS
				double lng
				double lat
				double frequency

	 		OUTPUT
	 			struct FamStats *FS - The FamStats structure contains the noise parameters

			SUBROUTINES
				None

			This routine is based on portions of the REC533() routines: GENFAM(), GENOIS1(), ANOIS1() and NOISY().

	 */

	double v[5];
	double u[2];
	double cz;
	double pz;
	double px;
	double x;
	double y;
	double Fam1MHz;	// Atmospheric noise Fam (dB above kT0b at 1 MHz)
	double R;
	double ZZ[30];	// This assumes lm = 29
	double q;		// Temp Latitude or Longitude

	int i, j, k;
	int lm, ln;

	// First find the atmospheric noise Fam (dB above kT0b at 1 MHz).
	// Set the limits of the Fourier series
	lm = 29;
	ln = 15;


	// The longitude used here is the geographic east longitude (0 to 2*PI radians)
	// Initialize the temp, q, as half the geographic east longitude
	if (lng < 0.0) {
		q = (lng + 2.0*PI) / 2.0;
	}
	else {
		q = lng / 2.0;
	};

	// Calculate the longitude series
	for(j=0; j<lm; j++) {
		ZZ[j] = 0.0; // Initialize ZZ[j]
		R = 0.0;
		for(k=0; k<ln; k++) {
			R = R + sin((k+1)*q)*noiseP->fakp[FS->tmblk][k][j];
		};
		ZZ[j] = R + noiseP->fakp[FS->tmblk][15][j];
	};

	// Calculate the latitude series
	// Reuse the temp, q, as the latitude plus 90 degrees
	q = (lat + PI/2.0);

	R = 0.0;
	for(j=0; j<lm; j++) {
		R = R + sin((j+1)*q)*ZZ[j];
	};
	// Final Fourier series calculation (Note the linear nomalization using fakabp values)
	Fam1MHz = R + noiseP->fakabp[FS->tmblk][0] + noiseP->fakabp[FS->tmblk][1]*q;

	// Determine if the reciever latitude is positive or negative
	if(lat < 0) {
		i = FS->tmblk + 6; // TIMEBLOCKINDX=TIMEBLOCKINDX+6
	}
	else {
		i = FS->tmblk; // TIMEBLOCKINDX=TIMEBLOCKINDX
	};

	// for K = 0 then U1 = -0.75
	// for K = 1 then U1 = U
	u[0] = -0.75;
	u[1] = (8.0*pow(2.0, log10(frequency)) - 11.0)/4.0; // U = (8. * 2.**X - 11.)/4. where X = ALOG10(FREQ)
	// Please See Page 5
	// NBS Tech Note 318 Lucas and Harper
	// "A Numerical Representation of CCIR Report 322 High Frequeny (3-30 Mc/s) Atmospheric Radio Noise Data"
	for(k=0; k<2; k++) {
		pz = u[k]*noiseP->fam[i][0] + noiseP->fam[i][1]; // PZ = U1*FAM(1,TIMEBLOCKINDX) + FAM(2,TIMEBLOCKINDX)
		px = u[k]*noiseP->fam[i][7] + noiseP->fam[i][8]; // PX = U1*FAM(8,TIMEBLOCKINDX) + FAM(9,TIMEBLOCKINDX)

		for(j=2; j<7; j++) {
			pz = u[k]*pz + noiseP->fam[i][j];			// PZ = U1*PZ + FAM(I,TIMEBLOCKINDX)
			px = u[k]*px + noiseP->fam[i][j+7];		// PX = U1*PX + FAM(I+7,TIMEBLOCKINDX)
		}; // j=2,6

		if(k == 0) {
			cz = Fam1MHz*(2.0 - pz) - px;
			// U1 = U
		};
	}; // k=0,1

	// Frequency variation of atmospheric noise
	FS->FA = cz*pz + px;

	//	Limit frequency to 20 MHz for Du, Dl, SigmaDu, SigmaDl
	//	because curves in ITU-R P.372 only go to 20 MHz
	x = log10(frequency);
	if(frequency > 20.0) {
		x = log10(20.0);
	};

	for(j=0; j<5; j++) { // DO I=1,5
		// Limit frequency to 10 MHz for SigmaFam
		// because curves in ITU-R P.372 only go to 10 MHz
		if((j == 4) && (frequency > 10.0)) { // IF((I .EQ. 5) .AND. (FREQ .GT. 10.0)) THEN
			x = 1.0;
		};

		y = noiseP->dud[j][i][0]; // Y = DUD(1,TIMEBLOCKINDX,I)

		for(k=1; k<5; k++) {
			y = y*x + noiseP->dud[j][i][k]; // Y = Y*X + DUD(J,TIMEBLOCKINDX,I)
		}; // k=1,4

		v[j] = y; // V(I) = Y
	}; // j=0,4

	//Store the return values
	FS->Du = v[0];		// Du 		 = V(1)
	FS->Dl = v[1];		// Dl 		 = V(2)
	FS->SigmaDu = v[2]; // Sigma_Du  = V(3)
	FS->SigmaDl = v[3]; // Sigma_Dl	 = V(4)
	FS->SigmaFam = v[4];// Sigma_Fam = V(5)

	return;
};

void ManMadeNoise(struct NoiseParams *noiseP, double frequency) {
	/*

	  ManMadeNoise() - Determines the man-made noise in accordance
	 		with Section 5 "Man-made noise" P.372-10

	 		INPUT
	 			struct NoiseParams *noiseP
				double manMadeNoise
				double frequency

	 		OUTPUT
	 			noiseP->FaM - Man-made noise
	 			noiseP->DuM - Upper decile deviation of man-made noise
	 			noiseP->DlM - Lower decile deviation of man-made noise

			SUBROUTINES
				None

	 */

	double c, d; // Intermediate values for the calculation of FaM

	if(noiseP->ManMadeNoise == CITY) {
		c = 76.8;
		d = 27.7;
		// Use the CITY category in Table 2 P.372-10 for the deciles
		noiseP->DuM = 11.0;
		noiseP->DlM =  6.7;
	}
	else if(noiseP->ManMadeNoise == RESIDENTIAL) {
		c = 72.5;
		d = 27.7;
		// Use the RESIDENTIAL category in Table 2 P.372-10 for the deciles
		noiseP->DuM = 10.6;
		noiseP->DlM =  5.3;
	}
	else if(noiseP->ManMadeNoise == RURAL) {
		c = 67.2;
		d = 27.7;
		// Use the RURAL category in Table 2 P.372-10 for the deciles
		noiseP->DuM = 9.2;
		noiseP->DlM = 4.6;
	}
	else if(noiseP->ManMadeNoise == QUIETRURAL) {
		c = 53.6;
		d = 28.6;
		// Use the RURAL category in Table 2 P.372-10 for the deciles
		noiseP->DuM = 9.2;
		noiseP->DlM = 4.6;
	}
	// The noise categories QUIET and NOISY are not in ITU-R P.372-10
	else if(noiseP->ManMadeNoise == QUIET) {
		c = 65.2;
		d = 29.1;
		// Use the RURAL category in Table 2 P.372-10 for the deciles
		noiseP->DuM = 9.2;
		noiseP->DlM = 4.6;
	}
	else if(noiseP->ManMadeNoise == NOISY) {
		c = 83.2;
		d = 37.5;
		// Use the CITY category in Table 2 P.372-10 for the deciles
		noiseP->DuM = 11.0;
		noiseP->DlM =  6.7;
	}
	else { // If manMadeNoise is any other number then it must be user input
		c = -noiseP->ManMadeNoise + 204.0;
		d = 0.0;
		// Use the CITY category in Table 2 P.372-10 for the deciles
		noiseP->DlM = 11.0;
		noiseP->DuM = 6.7;
	};

	// Calculate the man made noise, FaM
	noiseP->FaM = c - d*log10(frequency);

};

void GalacticNoise(struct NoiseParams *noiseP, double frequency) {

	/*

	 	GalacticNoise() - Determines the galactic noise in accordance with
	 		Section 5 "Man-made noise" Table 1 ade noise" P.372-10

	 		INPUT
	 			struct NoiseParams *noiseP
				double frequency

	 		OUTPUT
	 			noiseP->FaG - Galactic noise
	 			noiseP->DuG - Upper decile deviation of galactic noise
	 			noiseP->DlG - Lower decile deviation of galactic noise

			SUBROUTINES
				None

	 */

	double c, d; // Intermediate values for the calculation of FaG

	// Calculate the galactic noise, FaG
	c = 52.0;
	d = 23.0;
	noiseP->FaG = c - d*log10(frequency);

	// Determine the decile values are set to 2 dB (3/1.282)
	noiseP->DuG = 2.0;
	noiseP->DlG = 2.0;

};

int ReadFamDud(struct NoiseParams *noiseP, const char *DataFilePath, int month) {

	/*
	 * ReadFamDud() Reads the harmonized coefficient files from Peter Suessman. The files have been renamed from Suessman's
	 *		implementation to COEFFXX.TXT from itucofXX.txt, where XX is a two-digit number for the month. The names were
	 *		changed so the binary names and the text file names correspond. For this implementation of ITU-R P.533-12 the
	 *		only values that are used from these coefficient files are the arrays fakp[][], fakabp[][], dud[][][] and fam[][]
	 *		for the calculation of the atmospheric noise.
	 *
	 *		INPUT
	 *			struct NoiseParams *noiseP
	 *			int month
	 *
	 *		OUTPUT
	 *			noiseP->fam
	 *			noiseP->dud
	 *			noiseP->fakp
	 *			noiseP->fakabp
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
	sprintf(CoeffFile, "COEFF%02dW.txt", month+1);
	strcat(InFilePath, CoeffFile);

	fp = fopen(InFilePath, "r");
	if(fp == NULL) {
		printf("ReadFamDud: ERROR Can't find input file - %s\n", InFilePath);

		return RTN_ERROPENCOEFFFILE;
	};

	// Read the first header line.
	fgets(line, 256, fp);

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
				noiseP->fakp[i][j][k] = *(A+16*29*i+29*j+k);
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
			noiseP->fakabp[j][k] = *(A+2*j+k);
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
				noiseP->dud[i][j][k] = *(A+5*12*i+5*j+k);
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
			noiseP->fam[j][k] = *(A+14*j+k);
		};
	};

	// Free A
	free(A);

	// Clean up;
	fclose(fp);

	return RTN_READFAMDUDOK;
};

char const * P372Version() {

	/*

	P372Version() - Returns the version of the P533 DLL

	INPUT
	None

	OUTPUT
	returns a pointer to the version character string

	SUBROUTINES
	None

	*/

	return P372VER;

};

char const * P372CompileTime() {

	/*

	P533CompileTime() - Returns the compile time of the P533 DLL

	INPUT
	None

	OUTPUT
	returns a pointer to the version character string

	SUBROUTINES
	None

	*/

	return P372CT;

};


//////////////////////////////////////////////////////////////////////////////
//      Copyright  International Telecommunication Union (ITU) 2019         //
//                     All rights reserved.                                 //
// No part of this publication may be reproduced, by any means whatsoever,  //
//              without written permission of ITU                           //
//////////////////////////////////////////////////////////////////////////////
