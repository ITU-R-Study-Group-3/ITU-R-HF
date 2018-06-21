#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Local include
#include "ReadCoeff.h"
// End local include 

#define PI 3.14159265358979323846264338327950288419716939937510582
#define R2D 180.0/PI
#define D2R PI/180.0

// Local Prototypes
void WriteHeader(FILE *fp);
void WriteData(FILE *fp, double FA, double lng, double lat);
// End Local Prototypes
 
void main() {

	double lat, lng, q;
	double ZZ[29];
	double R;
	double Fam1MHz;
	double px, pz, cz;
	double u[2];
	double frequency;
	double FA;


	int lm, ln;
	int ilat, ilng;
	int j, k;
	int tmblklng, tmblklat;
	
	char OutFile[64];

	struct IonoCoeff Coeff;

	long What2Read;
	
	FILE *fp;

	// Open output file
	strcpy(OutFile, "FAKP.out");
	fp = fopen(OutFile, "w");
	if (fp == NULL) {
		printf("FAKP: ERROR Can't oen output file - %s\n", OutFile);
		printf("Press Enter...");
		getchar();
		return;
	};

	WriteHeader(fp);

	// Clear the coeff structure
	NullCoeffArrays(&Coeff);
		
	What2Read = GETFAKP | GETFAKABP | GETDUD |
							GETFAM;
		
	ReadCoeff(&Coeff, 4, What2Read);

	/////////////////////////////////////////
	// Do something with this wonderful data.
	/////////////////////////////////////////

	// frequency 
	frequency = 1.0; // MHz

	// Time Block
	tmblklng = 0; 

	lm = 29;
	ln = 15;

	for (ilat = -90; ilat <= 90; ilat++) { // latitude
		lat = ilat*D2R;
		for (ilng = 0; ilng <= 360; ilng++) { // longitude
			// The longitude used here is the geographic east longitude (0 to 2*PI radians)
			// Initialize the temp, q, as half the geographic east longitude
			lng = (ilng)*D2R;
			if (lng < 0.0) {
				q = (lng + 2.0*PI) / 2.0;
			}
			else {
				q = lng / 2.0;
			};

			// Calculate the longitude series
			for (j = 0; j < lm; j++) {
				ZZ[j] = 0.0; // Initialize ZZ[j]
				R = 0.0;
				for (k = 0; k < ln; k++) {
					R = R + sin((k + 1)*q)*Coeff.fakp[tmblklng][k][j];
				};
				ZZ[j] = R + Coeff.fakp[tmblklng][15][j];
			};

			// Calculate the latitude series
			// Reuse the temp, q, as the latitude plus 90 degrees
			q = (lat + PI / 2.0);

			R = 0.0;
			for (j = 0; j < lm; j++) {
				R = R + sin((j + 1)*q)*ZZ[j];
			};
			// Final Fourier series calculation (Note the linear nomalization using fakabp values)
			Fam1MHz = R + Coeff.fakabp[tmblklng][0] + Coeff.fakabp[tmblklng][1] * q;

			// Determine if the reciever latitude is positive or negative 
			if (lat < 0) {
				tmblklat = tmblklng + 6; // TIMEBLOCKINDX=TIMEBLOCKINDX+6
			}
			else {
				tmblklat = tmblklng; // TIMEBLOCKINDX=TIMEBLOCKINDX
			};
			// for K = 0 then U1 = -0.75
			// for K = 1 then U1 = U
			u[0] = -0.75;
			u[1] = (8.0*pow(2.0, log10(frequency)) - 11.0) / 4.0; // U = (8. * 2.**X - 11.)/4. where X = ALOG10(FREQ)
			// Please See Page 5
			// NBS Tech Note 318 Lucas and Harper 
			// "A Numerical Representation of CCIR Report 322 High Frequeny (3-30 Mc/s) Atmospheric Radio Noise Data"
			for (k = 0; k < 2; k++) {
				pz = u[k] * Coeff.fam[tmblklat][0] + Coeff.fam[tmblklat][1]; // PZ = U1*FAM(1,TIMEBLOCKINDX) + FAM(2,TIMEBLOCKINDX)
				px = u[k] * Coeff.fam[tmblklat][7] + Coeff.fam[tmblklat][8]; // PX = U1*FAM(8,TIMEBLOCKINDX) + FAM(9,TIMEBLOCKINDX)

				for (j = 2; j < 7; j++) {
					pz = u[k] * pz + Coeff.fam[tmblklat][j];			// PZ = U1*PZ + FAM(I,TIMEBLOCKINDX)
					px = u[k] * px + Coeff.fam[tmblklat][j + 7];		// PX = U1*PX + FAM(I+7,TIMEBLOCKINDX)
				}; // j=2,6

				if (k == 0) {
					cz = Fam1MHz*(2.0 - pz) - px;
					// U1 = U
				};
			}; // k=0,1

			// Frequency variation of atmospheric noise 
			FA = cz*pz + px;

			WriteData(fp, FA, lng, lat);

			printf("\r%d %d", ilat, ilng);

		};
	};
	////////////////////////////////////////

	FreeCoeffArrays(&Coeff);

	fclose(fp);

};

void WriteHeader(FILE *fp) {
	fprintf(fp, "latitude, longitude (0 - 360 E), Noise\n");
};

void WriteData(FILE *fp, double FA, double lng, double lat) {
	fprintf(fp, "%f, %f, %f\n", lat, lng, FA);

};
