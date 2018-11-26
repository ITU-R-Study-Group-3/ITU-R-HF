// Operating system preprocessor ****************************************************************
#ifdef _WIN32
	#include <Windows.h>

	// P533.DLL typedefs ***************************************************
	// P533 functions
	typedef const char * (__cdecl *cP533Info)();
	typedef int(__cdecl * iP533)(struct PathData * path);
	typedef int(__cdecl * iPathMemory)(struct PathData * path);
	typedef int(__cdecl * iReadType13Func)(struct Antenna *Ant, char * DataFilePath, double bearing, int silent);
	typedef void(__cdecl * vIsotropicPatternFunc)(struct Antenna *Ant, double G);
	typedef int(__cdecl * iReadFamDudFunc)(struct PathData *path, char * DataFilePath);
	typedef int(__cdecl * iReadIonParametersBinFunc)(int month, float ****foF2, float ****M3kF2, char DataFilePath[256], int silent);
	typedef int(__cdecl * iReadIonParametersTxtFunc)(struct PathData *path, char DataFilePath[256], int silent);
	typedef int(__cdecl * iReadP1239Func)(struct PathData *path, char * DataFilePath);
	typedef int(__cdecl * iInputDump)(struct PathData *path);

	// Geometry functions
	// GreatCirclePoint()
	typedef void(__cdecl * vGreatCirclePoint)(struct Location here, struct Location there, struct ControlPt *midpnt, double distance, double fraction);
	// GreatCircleDistance()
	typedef double(__cdecl * dGreatCircleDistance)(struct Location here, struct Location there);
	// Bearing()
	typedef double(__cdecl * dBearing)(struct Location here, struct Location there);
	// GeomagneticCoords()
	typedef void(__cdecl * vGeomagneticCoords)(struct Location here, struct Location *there);

#endif
// End P533.dll typedefs ************************************************

#ifdef _WIN32
	HINSTANCE hLib;
	cP533Info dllP533Version;
	cP533Info dllP533CompileTime;
	iP533 dllP533;
	iPathMemory dllAllocatePathMemory;
	iPathMemory dllFreePathMemory;
	iPathMemory dllAllocateAntennaMemory;
	dBearing dllBearing;
	iReadType11Func dllReadType11Func;
	iReadType13Func dllReadType13Func;
	iReadType14Func dllReadType14Func;
	vIsotropicPatternFunc dllIsotropicPatternFunc;
	iReadIonParametersBinFunc dllReadIonParametersBinFunc;
	iReadIonParametersTxtFunc dllReadIonParametersTxtFunc;
	iReadP1239Func dllReadP1239Func;
#elif __linux__ || __APPLE__
	#include <dlfcn.h>
	void * hLib;
	char * (*dllP533Version)();
	char * (*dllP533CompileTime)();
	int (*dllP533)(struct PathData *);
	int (*dllAllocatePathMemory)(struct PathData *);
	int (*dllFreePathMemory)(struct PathData *);
	int (*dllAllocateAntennaMemory)(struct Antenna *Ant, int freqn, int azin, int elen);
	double (*dllBearing)(struct Location,struct Location);
	int  (*dllReadType11Func)(struct Antenna *Ant, FILE *fp, int silent);
	int  (*dllReadType13Func)(struct Antenna *Ant, FILE *fp, double bearing, int silent);
	int  (*dllReadType14Func)(struct Antenna *Ant, FILE *fp, int silent);
	void (*dllIsotropicPatternFunc)(struct Antenna *Ant, double G, int silent);
	int  (*dllReadIonParametersTxtFunc)(struct PathData *path, char DataFilePath[256], int silent);
        int  (*dllReadIonParametersBinFunc)(int month, float ****foF2, float ****M3kF2, char DataFilePath[256], int silent);
	int  (*dllReadP1239Func)(struct PathData *path, const char * DataFilePath);
#endif

// End operating system preprocessor **************************************************************

// ITURHFProp **************************************************************************************

// #defines

// Version
#define ITURHFPropVER		__DATE__

// Have the preprocessor stamp the compile time
#define ITURHFPropCT		__TIMESTAMP__

// The following #defines are for the output field in structure ITURHFProp.

// Report output options **************************************************************************
#define	RPT_D				2			// 01	Path distance (km)
#define	RPT_DMAX			4			// 02	Path dmax (km) and Slant Range (km)
#define	RPT_ELE				8			// 03	Path elevation (Rad)
#define	RPT_BMUF			16			// 04	Path basic MUF (MHz)
#define RPT_BMUFD			32			// 05	BMUF deciles MUF50, MUF90 & MUF10
#define	RPT_OPMUF			64			// 06	Operational MUF (MHz)
#define	RPT_OPMUFD			128			// 07	OPMUF deciles OPMUF90, OPMUF10
#define RPT_N0_F2			256			// 08	Lowest order F2 layer mode
#define RPT_N0_E			512			// 09	Lowest order E layer mode
#define RPT_E				1024		// 10	Field Strength (dB(1uV/m)) Es, El or Ei depending on distance
#define	RPT_PR				2048		// 11	Median received power, Pr, and the receive antenna gain, Grw
#define RPT_NOISESOURCES	4096		// 12	Noise (Atmospheric) FaA, (Man-made) FaM & (Galactic) FaG (dB)
#define RPT_NOISESOURCESD	8192		// 13	DuA, DlA, DuM, DlM, DuG, & DlG
#define	RPT_NOISETOTALD		16384		// 14	DuT & DlT
#define	RPT_NOISETOTAL		32768		// 15	Total Noise, FamT (dB)
#define	RPT_SNR				65536		// 16	Signal-to-noise ratio (dB)
#define	RPT_SNRD			131072		// 17	DuSN & DlSN
#define	RPT_SNRXX			262144		// 18	Signal-to-noise ratio (dB) at the desired reliability XX %
#define	RPT_SIR				524288		// 19	Signal-to-interference ratio (dB)
#define	RPT_SIRD			1048576		// 20	SIR decile variations DuSI & DlSI
#define	RPT_RSN				2097152		// 21	Digital modulation performance parameters RSN, RT & RF
#define	RPT_BCR				4194304		// 22	Basic Circuit Relability (%)
#define	RPT_OCR				8388608		// 23	Overall Circuit Relability (%) without the consideration of scattering
#define	RPT_OCRS			16777216	// 24	Overall Circuit Relability (%) considering of scattering and Probocc (%)
#define	RPT_MIR				33554432	// 25	Multimode interference ratio (%)
#define RPT_RXLOCATION		67108864	// 26	Receiver location
#define RPT_DOMMODE			134217728	// 27	Dominant Mode
#define RPT_GRW				268435456   // 28
#define RPT_ESL				536870912   // 29	Field Strength (dB(1uV/m)) Es and El
#define RPT_LONG			1073741824	// 30	Long path parameters Gap, E0, fM, fL, Ly and K[2]

// Choose all CSV output
#define RPT_ALL				4294967294	// 2**(32) - 2

// RPT_DUMPPATHFILE - Dump the path structure everything else will be ignored
//			This option dumps all calculated path values which may
//			be helpful in debugging. The file created by the option is named
//			PDDddmmyy-hhnnss.txt - where the path data dump file is time stamped
//				dd = day
//				mm = month
//				yy = year
//				hh = hour
//				nn = minute
//				ss = seconds
#define	RPT_DUMPPATH		4294967295
// End Report output options **********************************************************************

// Return values **********************************************************************************

// Note: All return values from the driver program ITURHFProp >= 1000 and < 2000

// Return values for the following programs
//		ITURHFProp()
//		ReadIonParameters()
//		ReadAntennaPatterns()
//		ValidateITURHFP()
//      main()

//	Return ERROR number >= 1100
#define RTN_ERROPENOUTPUTFILE		1100 // ERROR: Can Not Open Output File
#define RTN_ERRP533DLL				1101 // ERROR: Can Not Find P533.DLL
#define	RTN_ERRCANTOPENRXANTFILE	1102 // ERROR: Can Not Open Recieve Antenna File
#define	RTN_ERRCANTOPENTXANTFILE	1103 // ERROR: Can Not Open Transmit Antenna File
#define RTN_ERRANTENNAORN			1104 // ERROR: Antenna Orientation
#define RTN_ERRTXBEARING			1105 // ERROR: Transmit Bearing
#define RTN_ERRRXBEARING			1106 // ERROR: Receive Bearing
#define RTN_ERRRXGOS				1107 // ERROR: Receive Gain Offset
#define RTN_ERRTXGOS				1108 // ERROR: Transmit Gain Offset
#define RTN_ERRLLLAT				1109 // ERROR: Invalid Lower Left Latitude
#define RTN_ERRLRLAT				1110 // ERROR: Invalid Lower Right Latitude
#define RTN_ERRULLAT				1111 // ERROR: Invalid Upper Left Latitude
#define RTN_ERRURLAT				1112 // ERROR: Invalid Upper Right Latitude
#define RTN_ERRLLLNG				1113 // ERROR: Invalid Lower Left Longitude
#define RTN_ERRLRLNG				1114 // ERROR: Invalid Lower Right Longitude
#define RTN_ERRULLNG				1115 // ERROR: Invalid Upper Left Longitude
#define RTN_ERRURLNG				1116 // ERROR: Invalid Upper Right Longitude
#define RTN_ERRLLAT					1117 // ERROR: Invalid Area Left Latitude
#define RTN_ERRULAT					1118 // ERROR: Invalid Area Right Latitude
#define RTN_ERRLLNG					1119 // ERROR: Invalid Area Left Longitude
#define RTN_ERRRLNG					1120 // ERROR: Invalid Area Right Longitude
#define RTN_ERRLL					1121 // ERROR: Invalid Area Lower Latitude
#define RTN_ERRUL					1122 // ERROR: Invalid Area Upper Latitude
#define RTN_ERRLR					1123 // ERROR: Invalid Area Left Longitude
#define RTN_ERRUR					1124 // ERROR: Invalid Area Right Longitude

// Returns ERROR for ITURHFProp - main()
#define RTN_ERRCOMMANDLINEARG		1200 // ERROR: Invalid Command Line
#define RTN_ERRNOINPUTFILE			1201 // ERROR: Missing Input File

// Returns OKAY numbers Returns < 1100
#define RTN_ITURHFPropOK			1001 // ITURHFProp()
#define RTN_VALIDATEITURHFPOK		1002 // ValidateITURHFP()
#define RTN_RICOK					1003 // ReadInputConfiguration()

#define RTN_MAINOK					1000 // Okay Calculation Completed

// End Return *********************************************************

#define NMBOFHOURS		24
#define NMBOFFREQS		32
#define NMBOFMONTHS		12

// End returns for ITURHFProp Demonstration Program - main()

// End returns values *****************************************************************************

// Antenna orientation ****************************************************************************
// Transmit and receive antennas point main lobes at each other
// Typically used in point-to-point analyses
// This option expresses the desire of the user to have the antennas pointed at
// each other.
#define	TX2RX			10
// Transmit and receive antenna are at bearings chosen by the user.
#define MANUAL			11
// End antenna orientation ************************************************************************

// Casting to an (int) sometimes needs a little help to push it to the next int.
#define INTTWEEK		1e-8

// Empty string character
#define	EMPTY			'0'

// End #defines ***********************************************************************************

// Structures *************************************************************************************

// ITURHFProp structure are the input parameters to run p533().
struct ITURHFProp {

	// Output Report File
	FILE *rptfp;
	char RptFilePath[256];
	long unsigned RptFileFormat;

	// Transmitter antenna input file
	FILE *txantfp;
	char TXAntFilePath[256];
	double TXBearing;		// This is where the main beam of the tx antenna is pointing.
	double TXGOS;			// The pattern gain offset

	// Receiver antenna input file
	FILE *rxantfp;
	char RXAntFilePath[256];
	double RXBearing;		// This is where the main beam of the rx antenna is pointing.
	double RXGOS;			// The pattern gain offset

	// Hours
	int hrs[NMBOFHOURS]; // Hours array
	int ihr;	 // Hours index
	int ihrend;  // Total number of hours to process

	// Frequency
	double frqs[NMBOFFREQS];
	int ifrq;
	int ifrqend;

	// Month
	int months[NMBOFMONTHS];
	int imnth;
	int imnthend;

	// Area of interest is defined by four corners.
	// It is assumed that the corners are typically:
	//		L_UL would be the north west corner of the area of interest
	//		L_UR would be the north east corner of the area of interest
	//		L_LL would be the south west corner of the area of interest
	//		L_LR would be the south east corner of the area of interest
	struct Location L_UL;
	struct Location L_UR;
	struct Location L_LR;
	struct Location L_LL;

	int ilatend;
	int ilngend;

	int ilat;
	int ilng;

	double latinc;
	double lnginc;

	int AntennaOrientation;

	struct tm *time;

	char DataFilePath[256];

	char const *P533ver;		// P533() Version number
	char const *P533compt;		// P533() Compile time

	int silent;					// Silent flag
	int header;					// Print header flag
};

// End structures *********************************************************************************

// Prototypes *************************************************************************************

// Note: The arguments passed are by reference (pointer) if the subroutine changes the argument within it.
//       Otherwise arguments are passed by value. There are a few cases that the only reason that the arguments
//		 are passed by reference was because the next level program required it.
// Any subroutines prototyped here are used external to the file that contains them. There may be local subroutines in
// each program file; consult them for more details. These subroutines were developed as the code was being written
// in the order necessary. If the order is maintained, the correspondence will be preserved between the code and
// the recommendation ITU-R P.533-12. In this regard, the order of execution of the subroutines is important because
// calculations in P.533-12 build on one another.

// ITURHFProp.c Prototype
int ITURHFProp(struct PathData *path, struct ITURHFProp *ITURHFP);

// P533.c Prototype for the P533 propagation model engine
int P533(struct PathData *path);

// DumpPathData.c Prototype
void DumpPathData(struct PathData path, struct ITURHFProp ITURHFP);
int degrees(double coord);
int minutes(double coord);
int seconds(double coord);
int hrs(double time);
int mns(double time);
void PrintITUHeader(FILE * fp, char * time, char const * P533Version, char const * P533CompT, char const * P372Version, char const * P372CompT);
void PrintITUTail(FILE * fp);

// Report.c Prototype
void Report(struct PathData path, struct ITURHFProp ITURHFP);

// ReadAntennaPatterns.c Prototype
int ReadAntennaPatterns(struct PathData *path, struct ITURHFProp ITURHFP);

// ValidateITURHFP.c Prototype
int ValidateITURHFP(struct ITURHFProp ITURHFP);

// ReadInputConfiguration.c PRototype
int ReadInputConfiguration(char InFilePath[256], struct ITURHFProp *ITURHFP, struct PathData *path);

// End prototypes *********************************************************************************

// End ITURHFProp **********************************************************************************

//////////////////////////////////////////////////////////////////////////////
//      Copyright  International Telecommunication Union (ITU) 2018         //
//                     All rights reserved.                                 //
// No part of this publication may be reproduced, by any means whatsoever,  //
//              without written permission of ITU                           //
//////////////////////////////////////////////////////////////////////////////
