#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Local include
#include "ReadCoeff.h"
// End local include 

void ReadCoeff(struct IonoCoeff *Coeff, int month, long What2Read) {

	/*
	 * ReadCoeff() Reads the harmonized coefficient files from Peter Suessman. The files have been renamed from Suessman's
	 *		implementation to COEFFXX.TXT from itucofXX.txt, where XX is a two-digit number that represents the month. The names were 
	 *		changed so the binary names and the text file names correspond. The binary and text coefficients will be included
	 *		in this project. For this implementation of ITU-R P.533-10 the only values that are used from these coefficient files
	 *		are the arrays dud[][][] and fam[][] for the calculation of the atmospheric noise. There are many other parameters in 
	 *		this set of coefficient files which can be retrieved in a simmilar manner to the extraction of dud[][][] and fam[][].
	 *
	 *		Although this routine was designed to be used with p533(), it can be used with ReadCoeff.h for other programs.  
	 *
	 *		INPUT
	 *			struct IonoCoeff *Coeff
	 *			int month - Month index
	 *			long What2Read - Flag to determine what is to be extracted from the Coeff file
	 *
	 *		OUTPUT
	 *			struct IonoCoeff *Coeff - The Coeff structure will contain pointers to the 
	 *				the desired data from the coefficient file
	 *
	 */
	 
	int i,j,k;
	int n,m;

	// A is the array that is read into from the file and aids in reshaping the target arrays
	// in the Coeff structure
	double *A;
	
	char line[256];

	char CoeffFile[14];
	char InFilePath[270];
	
	FILE *fp;

	strcpy(InFilePath, "..\\..\\..\\data\\coeffs\\txt\\");
	sprintf(CoeffFile, "COEFF%02dW.txt", month);
	strcat(InFilePath, CoeffFile);

	fp = fopen(InFilePath, "r"); 
	if(fp == NULL) {
		printf("ReadCoeff: ERROR Can't find input file - %s\n", InFilePath);
		printf("Press Enter...");
		getchar();
		return;
	};
	
	// Clear the Coeff structure
	NullCoeffArrays(Coeff);

	// Read the first header line
	fgets(line, 256, fp);

	// Store the header name
	strcpy(Coeff->name, line);
	
	//************************************************************************************
	// if2(10) & xf2(13,76,2)
	
	if((What2Read & GETXF2) == GETXF2) {
		// Create the if2 array
		Coeff->if2 = (int*) malloc(10 * sizeof(int));
		// Create the xf2 array
		Coeff->xf2 = (double***) malloc(2 * sizeof(double**));
		for(n=0; n<2; n++) {
			Coeff->xf2[n] = (double**) malloc(76 * sizeof(double*));
			for(m=0; m<76; m++) {
				Coeff->xf2[n][m] = (double*) malloc(13 * sizeof(double));
			};
		};

		// Allocate the array A that will allow for reshaping
		A = (double*) malloc(13*76*2 * sizeof(double));

		// Read the line "if2(10)"
		fgets(line, 256, fp);

		// Read 2 lines into the if2 array
		fgets(line, 256, fp); 
		sscanf(line, " %d %d %d %d %d\n", Coeff->if2+0, Coeff->if2+1, Coeff->if2+2,
											   Coeff->if2+3, Coeff->if2+4);
		fgets(line, 256, fp); 
		sscanf(line, " %d %d %d %d %d\n", Coeff->if2+5, Coeff->if2+6, Coeff->if2+7,
											   Coeff->if2+8, Coeff->if2+9);
		// Read the line "xf2(13,76,2)"
		fgets(line, 256, fp);
	
		// Read 395 lines into the array 
		for(n=0; n<395; n++) {
			fgets(line, 256, fp); 
			sscanf(line, " %lf %lf %lf %lf %lf\n", A+5*n, A+5*n+1, A+5*n+2, A+5*n+3, A+5*n+4);
		};
		// Read the last partial line
		fgets(line, 256, fp); 
		sscanf(line, " %lf\n", A+5*n);

		// Reshape A into the Coeff structure 
		for(i=0; i<2; i++) {
			for(j=0; j<76; j++) {
				for(k=0; k<13; k++) {
					Coeff->xf2[i][j][k] = *(A+76*13*i+13*j+k);
				}
			}
		};

		// Free A
		free(A);

	}
	else { // This data is not desired
		// Move to the next block
		for(n=0; n<400; n++) {
			fgets(line, 256, fp); 
		};
	};

	//************************************************************************************
	// ifm3(10) & xfm3(9,49,2)

	if((What2Read & GETXFM3) == GETXFM3) {
		// Create the ifm3 array
		Coeff->ifm3 = (int*) malloc(10 * sizeof(int));
		// Create the xfm3 array
		Coeff->xfm3 = (double***) malloc(2 * sizeof(double**));
		for(n=0; n<2; n++) {
			Coeff->xfm3[n] = (double**) malloc(49 * sizeof(double*));
			for(m=0; m<49; m++) {
				Coeff->xfm3[n][m] = (double*) malloc(9 * sizeof(double));
			};
		};

		// Allocate the array A that will allow for reshaping
		A = (double*) malloc(9*49*2 * sizeof(double));

		// Read the line "ifm3(10)"
		fgets(line, 256, fp);

		// Read 2 lines into the ifm3 array
		fgets(line, 256, fp); 
		sscanf(line, " %d %d %d %d %d\n", Coeff->ifm3, Coeff->ifm3+1, Coeff->ifm3+2,
											   Coeff->ifm3+3, Coeff->ifm3+4);
		fgets(line, 256, fp); 
		sscanf(line, " %d %d %d %d %d\n", Coeff->ifm3+5, Coeff->ifm3+6, Coeff->ifm3+7,
											   Coeff->ifm3+8, Coeff->ifm3+9);
		// Read the line "xfm3(19,49,2)"
		fgets(line, 256, fp);

		// Read 176 lines into the array A
		for(n=0; n<176; n++) {
			fgets(line, 256, fp); 
			sscanf(line, " %lf %lf %lf %lf %lf\n", A+5*n, A+5*n+1, A+5*n+2, A+5*n+3, A+5*n+4);
		};
		// Read the last partial line
		fgets(line, 256, fp); 
		sscanf(line, " %lf %lf\n", A+5*n, A+5*n+1);

		// Reshape A into the Coeff structure 
		for(i=0; i<2; i++) {
			for(j=0; j<49; j++) {
				for(k=0; k<9; k++) {
					Coeff->xfm3[i][j][k] = *(A+49*9*i+9*j+k);
				}
			}
		};

		// Free A
		free(A);

	}
	else { // This data is not desired
		// Move to the next block
		for(n=0; n<181; n++) {
			fgets(line, 256, fp); 
		};
	};
	//************************************************************************************
	// ie(10) & xe(9,22,2)
	if((What2Read & GETXE) == GETXE) {
		// Create the ie array
		Coeff->ie = (int*) malloc(10 * sizeof(int));
		// Create the xe array
		Coeff->xe = (double***) malloc(2 * sizeof(double**));
		for(n=0; n<2; n++) {
			Coeff->xe[n] = (double**) malloc(22 * sizeof(double*));
			for(m=0; m<22; m++) {
				Coeff->xe[n][m] = (double*) malloc(9 * sizeof(double));
			};
		};

		// Allocate the array A that will allow for reshaping
		A = (double*) malloc(9*22*2 * sizeof(double));
		
		// Read the line "ie(10)"
		fgets(line, 256, fp);

		// Read 2 lines into the ie array
		fgets(line, 256, fp); 
		sscanf(line, " %d %d %d %d %d\n", Coeff->ie, Coeff->ie+1, Coeff->ie+2,
											   Coeff->ie+3, Coeff->ie+4);
		fgets(line, 256, fp); 
		sscanf(line, " %d %d %d %d %d\n", Coeff->ie+5, &Coeff->ie+6, Coeff->ie+7,
											   Coeff->ie+8, &Coeff->ie+9);
		// Read the line "xe(13,76,2)"
		fgets(line, 256, fp);

		// Read 79 lines into the array A
		for(n=0; n<79; n++) {
			fgets(line, 256, fp); 
			sscanf(line, " %lf %lf %lf %lf %lf\n", A+5*n, A+5*n+1, A+5*n+2, A+5*n+3, A+5*n+4);
		};
		// Read the last partial line
		fgets(line, 256, fp); 
		sscanf(line, " %lf\n", A+5*n);

		// Reshape A into the Coeff structure 
		for(i=0; i<2; i++) {
			for(j=0; j<22; j++) {
				for(k=0; k<9; k++) {
					Coeff->xe[i][j][k] = *(A+22*9*i+9*j+k);
				}
			}
		};

		// Free A
		free(A);

	}
	else { // This data is not desired
		// Move to the next block
		for(n=0; n<84; n++) {
			fgets(line, 256, fp); 
		};
	};

	//************************************************************************************
	// iesu(10) & xesu(5,55,2)
	if((What2Read & GETXESU) == GETXESU) {
		// Create the iesu array
		Coeff->iesu = (int*) malloc(10 * sizeof(int));
		// Create the xesu array
		Coeff->xesu = (double***) malloc(2 * sizeof(double**));
		for(n=0; n<2; n++) {
			Coeff->xesu[n] = (double**) malloc(55 * sizeof(double*));
			for(m=0; m<55; m++) {
				Coeff->xesu[n][m] = (double*) malloc(5 * sizeof(double));
			};
		};

		// Allocate the array A that will allow for reshaping
		A = (double*) malloc(5*55*2 * sizeof(double));

		// Read the line "iesu(10)"
		fgets(line, 256, fp);

		// Read 2 lines into the iesu array
		fgets(line, 256, fp); 
		sscanf(line, " %d %d %d %d %d\n", Coeff->iesu, Coeff->iesu+1, Coeff->iesu+2,
											   Coeff->iesu+3, Coeff->iesu+4);
		fgets(line, 256, fp); 
		sscanf(line, " %d %d %d %d %d\n", Coeff->iesu+5, &Coeff->iesu+6, Coeff->iesu+7,
											   Coeff->iesu+8, &Coeff->iesu+9);
		// Read the line "xesu(13,76,2)"
		fgets(line, 256, fp);
	
		// Read 79 lines into the array A
		for(n=0; n<110; n++) {
			fgets(line, 256, fp); 
			sscanf(line, " %lf %lf %lf %lf %lf\n", A+5*n, A+5*n+1, A+5*n+2, A+5*n+3, A+5*n+4);
		};

		// Reshape A into the Coeff structure 
		for(i=0; i<2; i++) {
			for(j=0; j<55; j++) {
				for(k=0; k<5; k++) {
					Coeff->xesu[i][j][k] = *(A+55*5*i+5*j+k);
				}
			}
		};

		// Free A
		free(A);

	}
	else { // This data is not desired
		// Move to the next block
		for(n=0; n<114; n++) {
			fgets(line, 256, fp); 
		};
	};
	//************************************************************************************
	// ies(10) & xes(7,61,2)
	if((What2Read & GETXES) == GETXES) {
		// Create the ies array
		Coeff->ies = (int*) malloc(10 * sizeof(int));
		// Create the xes array
		Coeff->xes = (double***) malloc(2 * sizeof(double**));
		for(n=0; n<2; n++) {
			Coeff->xes[n] = (double**) malloc(61 * sizeof(double*));
			for(m=0; m<61; m++) {
				Coeff->xes[n][m] = (double*) malloc(7 * sizeof(double));
			};
		};

		// Allocate the array A that will allow for reshaping
		A = (double*) malloc(7*61*2 * sizeof(double));

		// Read the line "ies(10)"
		fgets(line, 256, fp);

		// Read 2 lines into the ies array
		fgets(line, 256, fp); 
		sscanf(line, " %d %d %d %d %d\n", Coeff->ies, Coeff->ies+1, Coeff->ies+2,
											   Coeff->ies+3, Coeff->ies+4);
		fgets(line, 256, fp); 
		sscanf(line, " %d %d %d %d %d\n", Coeff->ies+5, &Coeff->ies+6, Coeff->ies+7,
											   Coeff->ies+8, &Coeff->ies+9);
		// Read the line "xes(7,61,2)"
		fgets(line, 256, fp);

		// Read 79 lines into the array A
		for(n=0; n<170; n++) {
			fgets(line, 256, fp); 
			sscanf(line, " %lf %lf %lf %lf %lf\n", A+5*n, A+5*n+1, A+5*n+2, A+5*n+3, A+5*n+4);
		};
		// Read the last partial line
		fgets(line, 256, fp); 
		sscanf(line, " %lf %lf %lf %lf\n", A+5*n, A+5*n+1, A+5*n+2, A+5*n+3);

		// Reshape A in to the Coeff structure 
		for(i=0; i<2; i++) {
			for(j=0; j<61; j++) {
				for(k=0; k<7; k++) {
					Coeff->xes[i][j][k] = *(A+61*7*i+7*j+k);
				}
			}
		};

		// Free A
		free(A);
	}
	else { // This data is not desired
		// Move to the next block
		for(n=0; n<175; n++) {
			fgets(line, 256, fp); 
		};
	};
	//************************************************************************************
	// iels(10) & xels(5,55,2)
	if((What2Read & GETXESL) == GETXESL) {
		// Create the iels array
		Coeff->iesl = (int*) malloc(10 * sizeof(int));
		// Create the xels array
		Coeff->xesl = (double***) malloc(2 * sizeof(double**));
		for(n=0; n<2; n++) {
			Coeff->xesl[n] = (double**) malloc(55 * sizeof(double*));
			for(m=0; m<55; m++) {
				Coeff->xesl[n][m] = (double*) malloc(5 * sizeof(double));
			};
		};

		// Allocate the array A that will allow for reshaping
		A = (double*) malloc(5*55*2 * sizeof(double));

		// Read the line "iels(10)"
		fgets(line, 256, fp);

		// Read 2 lines into the iels array
		fgets(line, 256, fp); 
		sscanf(line, " %d %d %d %d %d\n", Coeff->iesl, Coeff->iesl+1, Coeff->iesl+2,
											   Coeff->iesl+3, Coeff->iesl+4);
		fgets(line, 256, fp); 
		sscanf(line, " %d %d %d %d %d\n", Coeff->iesl+5, &Coeff->iesl+6, Coeff->iesl+7,
											   Coeff->iesl+8, &Coeff->iesl+9);
		// Read the line "xels(13,76,2)"
		fgets(line, 256, fp);

		// Read 79 lines into the array A
		for(n=0; n<110; n++) {
			fgets(line, 256, fp); 
			sscanf(line, " %lf %lf %lf %lf %lf\n", A+5*n, A+5*n+1, A+5*n+2, A+5*n+3, A+5*n+4);
		};

		// Reshape A into the Coeff structure 
		for(i=0; i<2; i++) {
			for(j=0; j<55; j++) {
				for(k=0; k<5; k++) {
					Coeff->xesl[i][j][k] = *(A+55*5*i+5*j+k);
				}
			}
		};

		// Free A
		free(A);

	}
	else { // This data is not desired
		// Move to the next block
		for(n=0; n<114; n++) {
			fgets(line, 256, fp); 
		};
	};
	//************************************************************************************
	// ihpo1(10) & xhpo1(13,29,2)
	if((What2Read & GETXHPO1) == GETXHPO1) {
		// Create the ihpo1 array
		Coeff->ihpo1 = (int*) malloc(10 * sizeof(int));
		// Create the xhpo1 array
		Coeff->xhpo1 = (double***) malloc(2 * sizeof(double**));
		for(n=0; n<2; n++) {
			Coeff->xhpo1[n] = (double**) malloc(29 * sizeof(double*));
			for(m=0; m<29; m++) {
				Coeff->xhpo1[n][m] = (double*) malloc(13 * sizeof(double));
			};
		};

		// Allocate the array A that will allow for reshaping
		A = (double*) malloc(13*29*2 * sizeof(double));

		// Read the line "ihpo1(10)"
		fgets(line, 256, fp);

		// Read 2 lines into the ihpo1 array
		fgets(line, 256, fp); 
		sscanf(line, " %d %d %d %d %d\n", Coeff->ihpo1, Coeff->ihpo1+1, Coeff->ihpo1+2,
											   Coeff->ihpo1+3, Coeff->ihpo1+4);
		fgets(line, 256, fp); 
		sscanf(line, " %d %d %d %d %d\n", Coeff->ihpo1+5, &Coeff->ihpo1+6, Coeff->ihpo1+7,
											   Coeff->ihpo1+8, &Coeff->ihpo1+9);
		// Read the line "xhpo1(13,29,2)"
		fgets(line, 256, fp);
	
		// Read 79 lines into the array A
		for(n=0; n<150; n++) {
			fgets(line, 256, fp); 
			sscanf(line, " %lf %lf %lf %lf %lf\n", A+5*n, A+5*n+1, A+5*n+2, A+5*n+3, A+5*n+4);
		};
		// Read the last partial line
		fgets(line, 256, fp); 
		sscanf(line, " %lf %lf %lf %lf\n", A+5*n, A+5*n+1, A+5*n+2, A+5*n+3);

		// Reshape A in to the Coeff structure 
		for(i=0; i<2; i++) {
			for(j=0; j<29; j++) {
				for(k=0; k<13; k++) {
					Coeff->xhpo1[i][j][k] = *(A+29*13*i+13*j+k);
				}
			}
		};

		// Free A
		free(A);

	}
	else { // This data is not desired
		// Move to the next block
		for(n=0; n<155; n++) {
			fgets(line, 256, fp); 
		};
	};
	//************************************************************************************
	// ihpo2(10) & xhpo2(9,55,2)
	if((What2Read & GETXHPO2) == GETXHPO2) {
		// Create the ihpo2 array
		Coeff->ihpo2 = (int*) malloc(10 * sizeof(int));
		// Create the xhpo2 array
		Coeff->xhpo2 = (double***) malloc(2 * sizeof(double**));
		for(n=0; n<2; n++) {
			Coeff->xhpo2[n] = (double**) malloc(55 * sizeof(double*));
			for(m=0; m<55; m++) {
				Coeff->xhpo2[n][m] = (double*) malloc(9 * sizeof(double));
			};
		};

		// Allocate the array A that will allow for reshaping
		A = (double*) malloc(9*55*2 * sizeof(double));

		// Read the line "ihpo2(10)"
		fgets(line, 256, fp);

		// Read 2 lines into the ihpo2 array
		fgets(line, 256, fp); 
		sscanf(line, " %d %d %d %d %d\n", Coeff->ihpo2, Coeff->ihpo2+1, Coeff->ihpo2+2,
											   Coeff->ihpo2+3, Coeff->ihpo2+4);
		fgets(line, 256, fp); 
		sscanf(line, " %d %d %d %d %d\n", Coeff->ihpo2+5, &Coeff->ihpo2+6, Coeff->ihpo2+7,
											   Coeff->ihpo2+8, &Coeff->ihpo2+9);
		// Read the line "xhpo2(9,55,2)"
		fgets(line, 256, fp);
	
		// Read 79 lines into the array A
		for(n=0; n<198; n++) {
			fgets(line, 256, fp); 
			sscanf(line, " %lf %lf %lf %lf %lf\n", A+5*n, A+5*n+1, A+5*n+2, A+5*n+3, A+5*n+4);
		};

		// Reshape A into the Coeff structure 
		for(i=0; i<2; i++) {
			for(j=0; j<55; j++) {
				for(k=0; k<9; k++) {
					Coeff->xhpo2[i][j][k] = *(A+55*9*i+9*j+k);
				}
			}
		};

		// Free A
		free(A);
	}
	else { // This data is not desired
		// Move to the next block
		for(n=0; n<202; n++) {
			fgets(line, 256, fp); 
		};
	};
	//************************************************************************************
	// ihp(10) & xhp(9,37,2)
	if((What2Read & GETXHP) == GETXHP) {
		// Create the ihp array
		Coeff->ihp = (int*) malloc(10 * sizeof(int));
		// Create the xhp array
		Coeff->xhp = (double***) malloc(2 * sizeof(double**));
		for(n=0; n<2; n++) {
			Coeff->xhp[n] = (double**) malloc(37 * sizeof(double*));
			for(m=0; m<37; m++) {
				Coeff->xhp[n][m] = (double*) malloc(9 * sizeof(double));
			};
		};

		// Allocate the array A that will allow for reshaping
		A = (double*) malloc(9*37*2 * sizeof(double));

		// Read the line "ihp(10)"
		fgets(line, 256, fp);

		// Read 2 lines into the ihp array
		fgets(line, 256, fp); 
		sscanf(line, " %d %d %d %d %d\n", Coeff->ihp, Coeff->ihp+1, Coeff->ihp+2,
											   Coeff->ihp+3, Coeff->ihp+4);
		fgets(line, 256, fp); 
		sscanf(line, " %d %d %d %d %d\n", Coeff->ihp+5, &Coeff->ihp+6, Coeff->ihp+7,
											   Coeff->ihp+8, &Coeff->ihp+9);
		// Read the line "xhp(13,76,2)"
		fgets(line, 256, fp);
	
		// Read 79 lines into the array A
		for(n=0; n<133; n++) {
			fgets(line, 256, fp); 
			sscanf(line, " %lf %lf %lf %lf %lf\n", A+5*n, A+5*n+1, A+5*n+2, A+5*n+3, A+5*n+4);
		};
		// Read the last partial line
		fgets(line, 256, fp); 
		sscanf(line, " %lf\n", A+5*n);
		
		// Reshape A into the Coeff structure 
		for(i=0; i<2; i++) {
			for(j=0; j<37; j++) {
				for(k=0; k<9; k++) {
					Coeff->xhp[i][j][k] = *(A+37*9*i+9*j+k);
				}
			}
		};

		// Free A
		free(A);

	}
	else { // This data is not desired
		// Move to the next block
		for(n=0; n<138; n++) {
			fgets(line, 256, fp); 
		};
	};
	//************************************************************************************
	// fakp(29,16,6)
	if((What2Read & GETFAKP) == GETFAKP) {
		// Create the fakp array
		Coeff->fakp = (double***) malloc(6 * sizeof(double**));
		for(n=0; n<6; n++) {
			Coeff->fakp[n] = (double**) malloc(16 * sizeof(double*));
			for(m=0; m<16; m++) {
				Coeff->fakp[n][m] = (double*) malloc(29 * sizeof(double));
			};
		};

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
					Coeff->fakp[i][j][k] = *(A+16*29*i+29*j+k);
				};
			};
		};

		// Free A
		free(A);

	}
	else {// This data is not desired
		// Move to the next block
		for(n=0; n<558; n++) {
			fgets(line, 256, fp); 
		};
	};

	//************************************************************************************
	// fakabp(2,6)
	if((What2Read & GETFAKABP) == GETFAKABP) {
		// Create the fakabp array
		Coeff->fakabp = (double**) malloc(6 * sizeof(double*));
		for(m=0; m<6; m++) {
			Coeff->fakabp[m] = (double*) malloc(2 * sizeof(double));
		};

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
				Coeff->fakabp[j][k] = *(A+2*j+k);
			};
		};

		// Free A
		free(A);

	}
	else {// This data is not desired
		// Move to the next block
		for(n=0; n<4; n++) {
			fgets(line, 256, fp); 
		};
	};
	//************************************************************************************
	// dud(5,12,5)
	if((What2Read & GETDUD) == GETDUD) {
		// Create the dud array
		Coeff->dud = (double***) malloc(5 * sizeof(double**));
		for(n=0; n<5; n++) {
			Coeff->dud[n] = (double**) malloc(12 * sizeof(double*));
			for(m=0; m<12; m++) {
				Coeff->dud[n][m] = (double*) malloc(5 * sizeof(double));
			};
		};

		// Allocate the array A that will allow for reshaping
		A = (double*) malloc(5*12*5 * sizeof(double));

		// Read the line "dud(5,12,5)"
		fgets(line, 256, fp);

		// Read 60 lines into the array A
		for(n=0; n<60; n++) {
			fgets(line, 256, fp); 
			sscanf(line, " %lf %lf %lf %lf %lf\n", A+5*n, A+5*n+1, A+5*n+2, A+5*n+3, A+5*n+4);
		};
		
		// Reshape A into the Coeff structure 
		for(i=0; i<5; i++) {
			for(j=0; j<12; j++) {
				for(k=0; k<5; k++) {
					Coeff->dud[i][j][k] = *(A+5*12*i+5*j+k);
				};
			};
		};

		// Free A
		free(A);

	}
	else {// This data is not desired
		// Move to the next block
		for(n=0; n<61; n++) {
			fgets(line, 256, fp); 
		};
	};
	//************************************************************************************
	// fam(14,12)
	if((What2Read & GETFAM) == GETFAM) {
		// Create the fam array
		Coeff->fam = (double**) malloc(12 * sizeof(double*));
		for(m=0; m<12; m++) {
			Coeff->fam[m] = (double*) malloc(14 * sizeof(double));
		};
		
		// Allocate the array A that will allow for reshaping
		A = (double*) malloc(12*14 * sizeof(double));

		// Read the line "fam(14,12)"
		fgets(line, 256, fp);

		// Read 33 lines into the array A
		for(n=0; n<33; n++) {
			fgets(line, 256, fp); 
			sscanf(line, " %lf %lf %lf %lf %lf\n", A+5*n, A+5*n+1, A+5*n+2, A+5*n+3, A+5*n+4);
		};
		// Read the last partial line
		fgets(line, 256, fp); 
		sscanf(line, " %lf %lf %lf\n", A+5*n, A+5*n+1, A+5*n+2);
		
		// Reshape A into the Coeff structure 
		for(j=0; j<12; j++) {
			for(k=0; k<14; k++) {
				Coeff->fam[j][k] = *(A+14*j+k);
			};
		};

		// Free A
		free(A);

	}
	else {// This data is not desired
		// Move to the next block
		for(n=0; n<35; n++) {
			fgets(line, 256, fp); 
		};
	};
	//************************************************************************************
	// sys1(9,16,6)
	if((What2Read & GETSYS1) == GETSYS1) {
		// Create the sys1 array
		Coeff->sys1 = (double***) malloc(6 * sizeof(double**));
		for(n=0; n<6; n++) {
			Coeff->sys1[n] = (double**) malloc(16 * sizeof(double*));
			for(m=0; m<16; m++) {
				Coeff->sys1[n][m] = (double*) malloc(9 * sizeof(double));
			};
		};

		// Allocate the array A that will allow for reshaping
		A = (double*) malloc(9*16*6 * sizeof(double));

		// Read the line "sys1(9,16,6)"
		fgets(line, 256, fp);

		// Read 172 lines into the array A
		for(n=0; n<172; n++) {
			fgets(line, 256, fp); 
			sscanf(line, " %lf %lf %lf %lf %lf\n", A+5*n, A+5*n+1, A+5*n+2, A+5*n+3, A+5*n+4);
		};
		// Read the last partial line
		fgets(line, 256, fp); 
		sscanf(line, " %lf %lf %lf %lf\n", A+5*n, A+5*n+1, A+5*n+2, A+5*n+3);
		
		// Reshape A into the Coeff structure 
		for(i=0; i<6; i++) {
			for(j=0; j<16; j++) {
				for(k=0; k<9; k++) {
					Coeff->sys1[i][j][k] = *(A+16*9*i+9*j+k);
				};
			};
		};

		// Free A
		free(A);

	}
	else {// This data is not desired
		// Move to the next block
		for(n=0; n<174; n++) {
			fgets(line, 256, fp); 
		};
	};
	//************************************************************************************
	// sys2(9,16,6)
	if((What2Read & GETSYS2) == GETSYS2) {
		// Create the sys2 array
		Coeff->sys2 = (double***) malloc(6 * sizeof(double**));
		for(n=0; n<6; n++) {
			Coeff->sys2[n] = (double**) malloc(16 * sizeof(double*));
			for(m=0; m<16; m++) {
				Coeff->sys2[n][m] = (double*) malloc(9 * sizeof(double));
			};
		};

		// Allocate the array A that will allow for reshaping
		A = (double*) malloc(9*16*6 * sizeof(double));

		// Read the line "sys2(9,16,6)"
		fgets(line, 256, fp);

		// Read 172 lines into the array A
		for(n=0; n<172; n++) {
			fgets(line, 256, fp); 
			sscanf(line, " %lf %lf %lf %lf %lf\n", A+5*n, A+5*n+1, A+5*n+2, A+5*n+3, A+5*n+4);
		};
		// Read the last partial line
		fgets(line, 256, fp); 
		sscanf(line, " %lf %lf %lf %lf\n", A+5*n, A+5*n+1, A+5*n+2, A+5*n+3);
		
		// Reshape A into the Coeff structure 
		for(i=0; i<6; i++) {
			for(j=0; j<16; j++) {
				for(k=0; k<9; k++) {
					Coeff->sys2[i][j][k] = *(A+16*9*i+9*j+k);
				};
			};
		};

		// Free A
		free(A);

	}
	else {// This data is not desired
		// Move to the next block
		for(n=0; n<174; n++) {
			fgets(line, 256, fp); 
		};
	};
	//************************************************************************************
	// perr(9,4,6)
	if((What2Read & GETPERR) == GETPERR) {
		// Create the perr array
		Coeff->perr = (double***) malloc(6 * sizeof(double**));
		for(n=0; n<6; n++) {
			Coeff->perr[n] = (double**) malloc(4 * sizeof(double*));
			for(m=0; m<4; m++) {
				Coeff->perr[n][m] = (double*) malloc(9 * sizeof(double));
			};
		};

		// Allocate the array A that will allow for reshaping
		A = (double*) malloc(9*4*6 * sizeof(double));

		// Read the line "perr(9,4,6)"
		fgets(line, 256, fp);

		// Read 43 lines into the array A
		for(n=0; n<43; n++) {
			fgets(line, 256, fp); 
			sscanf(line, " %lf %lf %lf %lf %lf\n", A+5*n, A+5*n+1, A+5*n+2, A+5*n+3, A+5*n+4);
		};
		// Read the last partial line
		fgets(line, 256, fp); 
		sscanf(line, " %lf\n", A+5*n);
		
		// Reshape A into the Coeff structure 
		for(i=0; i<6; i++) {
			for(j=0; j<4; j++) {
				for(k=0; k<9; k++) {
					Coeff->perr[i][j][k] = *(A+4*9*i+9*j+k);
				};
			};
		};

		// Free A
		free(A);

	}
	else {// This data is not desired
		// Move to the next block
		for(n=0; n<45; n++) {
			fgets(line, 256, fp); 
		};
	};
	//************************************************************************************
	// f2d(16,6,6)
	if((What2Read & GETF2D) == GETF2D) {
		// Create the f2d array
		Coeff->f2d = (double***) malloc(6 * sizeof(double**));
		for(n=0; n<6; n++) {
			Coeff->f2d[n] = (double**) malloc(6 * sizeof(double*));
			for(m=0; m<6; m++) {
				Coeff->f2d[n][m] = (double*) malloc(16 * sizeof(double));
			};
		};

		// Allocate the array A that will allow for reshaping
		A = (double*) malloc(6*16*6 * sizeof(double));

		// Read the line "f2d(16,6,6)"
		fgets(line, 256, fp);

		// Read 115 lines into the array A
		for(n=0; n<115; n++) {
			fgets(line, 256, fp); 
			sscanf(line, " %lf %lf %lf %lf %lf\n", A+5*n, A+5*n+1, A+5*n+2, A+5*n+3, A+5*n+4);
		};
		// Read the last partial line
		fgets(line, 256, fp); 
		sscanf(line, " %lf\n", A+5*n);
		
		// Reshape A into the Coeff structure 
		for(i=0; i<6; i++) {
			for(j=0; j<6; j++) {
				for(k=0; k<16; k++) {
					Coeff->f2d[i][j][k] = *(A+6*16*i+16*j+k);
				};
			};
		};

		// Free A
		free(A);

	}
	else {// This data is not desired
		// Move to the next block
		for(n=0; n<117; n++) {
			fgets(line, 256, fp); 
		};
	};
	//************************************************************************************
	// pko(8,7,6)
	if((What2Read & GETPKO) == GETPKO) {
		// Create the pko array
		Coeff->pko = (double***) malloc(6 * sizeof(double**));
		for(n=0; n<6; n++) {
			Coeff->pko[n] = (double**) malloc(7 * sizeof(double*));
			for(m=0; m<7; m++) {
				Coeff->pko[n][m] = (double*) malloc(8 * sizeof(double));
			};
		};

		// Allocate the array A that will allow for reshaping
		A = (double*) malloc(8*7*6 * sizeof(double));

		// Read the line "pko(8,7,6)"
		fgets(line, 256, fp);

		// Read 67 lines into the array A
		for(n=0; n<67; n++) {
			fgets(line, 256, fp); 
			sscanf(line, " %lf %lf %lf %lf %lf\n", A+5*n, A+5*n+1, A+5*n+2, A+5*n+3, A+5*n+4);
		};
		// Read the last partial line
		fgets(line, 256, fp); 
		sscanf(line, " %lf\n", A+5*n);
		
		// Reshape A into the Coeff structure 
		for(i=0; i<6; i++) {
			for(j=0; j<7; j++) {
				for(k=0; k<8; k++) {
					Coeff->pko[i][j][k] = *(A+7*8*i+8*j+k);
				};
			};
		};

		// Free A
		free(A);

	}
	else {// This data is not desired
		// Move to the next block
		for(n=0; n<69; n++) {
			fgets(line, 256, fp); 
		};
	};
	//************************************************************************************
	// slp(8,7,6)
	if((What2Read & GETSLP) == GETSLP) {
		// Create the slp array
		Coeff->slp = (double***) malloc(6 * sizeof(double**));
		for(n=0; n<6; n++) {
			Coeff->slp[n] = (double**) malloc(7 * sizeof(double*));
			for(m=0; m<7; m++) {
				Coeff->slp[n][m] = (double*) malloc(8 * sizeof(double));
			};
		};

		// Allocate the array A that will allow for reshaping
		A = (double*) malloc(8*7*6 * sizeof(double));

		// Read the line "slp(8,7,6)"
		fgets(line, 256, fp);

		// Read 67 lines into the array A
		for(n=0; n<67; n++) {
			fgets(line, 256, fp); 
			sscanf(line, " %lf %lf %lf %lf %lf\n", A+5*n, A+5*n+1, A+5*n+2, A+5*n+3, A+5*n+4);
		};
		// Read the last partial line
		fgets(line, 256, fp); 
		sscanf(line, " %lf\n", A+5*n);
		
		// Reshape A into the Coeff structure 
		for(i=0; i<6; i++) {
			for(j=0; j<7; j++) {
				for(k=0; k<8; k++) {
					Coeff->slp[i][j][k] = *(A+7*8*i+8*j+k);
				};
			};
		};

		// Free A
		free(A);

	}
	else {// This data is not desired
		// Move to the next block
		for(n=0; n<69; n++) {
			fgets(line, 256, fp); 
		};
	};
	//************************************************************************************
	// ccr(8,7,6)
	if((What2Read & GETCCR) == GETCCR) {
		// Create the ccr array
		Coeff->ccr = (double***) malloc(6 * sizeof(double**));
		for(n=0; n<6; n++) {
			Coeff->ccr[n] = (double**) malloc(7 * sizeof(double*));
			for(m=0; m<7; m++) {
				Coeff->ccr[n][m] = (double*) malloc(8 * sizeof(double));
			};
		};

		// Allocate the array A that will allow for reshaping
		A = (double*) malloc(8*7*6 * sizeof(double));

		// Read the line "ccr(8,7,6)"
		fgets(line, 256, fp);

		// Read 67 lines into the array A
		for(n=0; n<67; n++) {
			fgets(line, 256, fp); 
			sscanf(line, " %lf %lf %lf %lf %lf\n", A+5*n, A+5*n+1, A+5*n+2, A+5*n+3, A+5*n+4);
		};
		// Read the last partial line
		fgets(line, 256, fp); 
		sscanf(line, " %lf\n", A+5*n);
		
		// Reshape A into the Coeff structure 
		for(i=0; i<6; i++) {
			for(j=0; j<7; j++) {
				for(k=0; k<8; k++) {
					Coeff->ccr[i][j][k] = *(A+7*8*i+8*j+k);
				};
			};
		};

		// Free A
		free(A);

	}
	else {// This data is not desired
		// Move to the next block
		for(n=0; n<69; n++) {
			fgets(line, 256, fp); 
		};
	};

	/**********************************************************************************************/

	// Clean up;
	fclose(fp);

	return;
};

void	FreeCoeffArrays(struct IonoCoeff *Coeff) {

	/*
	 *	FreeCoeffArrays() - Frees the memory 
	 *
	 *		INPUT
	 *			struct IonoCoeff *Coeff
	 *
	 *		OUTPUT
	 *			struct IonoCoeff *Coeff
	 *
	 */

	 strcpy(Coeff->name, "Free");		
	 if(Coeff->if2 != NULL)		free(Coeff->if2);		
	 if(Coeff->xf2 != NULL)		free(Coeff->xf2);		
	 if(Coeff->ifm3 != NULL)		free(Coeff->ifm3);		
	 if(Coeff->xfm3 != NULL)		free(Coeff->xfm3);		
	 if(Coeff->ie != NULL)		free(Coeff->ie);			
	 if(Coeff->xe != NULL)		free(Coeff->xe);		
	 if(Coeff->iesu != NULL)		free(Coeff->iesu);		
	 if(Coeff->xesu != NULL)		free(Coeff->xesu);		
	 if(Coeff->ies != NULL)		free(Coeff->ies);		
	 if(Coeff->xes != NULL)		free(Coeff->xes);		
	 if(Coeff->iesl != NULL)		free(Coeff->iesl);		
	 if(Coeff->xesl != NULL)		free(Coeff->xesl);		
	 if(Coeff->ihpo1 != NULL)	free(Coeff->ihpo1);		
	 if(Coeff->xhpo1 != NULL)	free(Coeff->xhpo1);	
	 if(Coeff->ihpo2 != NULL)	free(Coeff->ihpo2);		
	 if(Coeff->xhpo2 != NULL)	free(Coeff->xhpo2);	
	 if(Coeff->ihp != NULL)		free(Coeff->ihp);		
	 if(Coeff->xhp != NULL)		free(Coeff->xhp);		
	 if(Coeff->fakp != NULL)		free(Coeff->fakp);		
	 if(Coeff->fakabp != NULL)	free(Coeff->fakabp);	
	 if(Coeff->dud != NULL)		free(Coeff->dud);		
	 if(Coeff->fam != NULL)		free(Coeff->fam);		
	 if(Coeff->sys1 != NULL)		free(Coeff->sys1);		
	 if(Coeff->sys2 != NULL)		free(Coeff->sys2);		
	 if(Coeff->perr != NULL)		free(Coeff->perr);		
	 if(Coeff->f2d != NULL)		free(Coeff->f2d);		
	 if(Coeff->pko != NULL)		free(Coeff->pko);		
	 if(Coeff->slp != NULL)		free(Coeff->slp);		
	 if(Coeff->ccr != NULL)		free(Coeff->ccr);		

	 return;

};
void	NullCoeffArrays(struct IonoCoeff *Coeff) {

	/*
	 *	NullCoeffArrays() - initializes the structure IonoCoeff
	 *
	 *		INPUT
	 *			struct IonoCoeff *Coeff
	 *
	 *		OUTPUT
	 *			struct IonoCoeff *Coeff
	 *
	 */

	 strcpy(Coeff->name, "NULL");		
	 Coeff->if2 = NULL;		
	 Coeff->xf2 = NULL;		
	 Coeff->ifm3 = NULL;		
	 Coeff->xfm3 = NULL;		
	 Coeff->ie = NULL;			
	 Coeff->xe = NULL;		
	 Coeff->iesu = NULL;		
	 Coeff->xesu = NULL;		
	 Coeff->ies = NULL;		
	 Coeff->xes = NULL;		
	 Coeff->iesl = NULL;		
	 Coeff->xesl = NULL;		
	 Coeff->ihpo1 = NULL;		
	 Coeff->xhpo1 = NULL;	
	 Coeff->ihpo2 = NULL;		
	 Coeff->xhpo2 = NULL;	
	 Coeff->ihp = NULL;		
	 Coeff->xhp = NULL;		
	 Coeff->fakp = NULL;		
	 Coeff->fakabp = NULL;	
	 Coeff->dud = NULL;		
	 Coeff->fam = NULL;		
	 Coeff->sys1 = NULL;		
	 Coeff->sys2 = NULL;		
	 Coeff->perr = NULL;		
	 Coeff->f2d = NULL;		
	 Coeff->pko = NULL;		
	 Coeff->slp = NULL;		
	 Coeff->ccr = NULL;		

	 return;
};