#ifndef ITURNOISE_H
#define ITURNOISE_H

// ITURNoise Return codes
//  Error codes
#define RTN_ERRYEAR						100	// ERROR: Invalid Input Year
#define RTN_ERRMONTH					101 // ERROR: Invalid Input Month
#define RTN_ERRHOUR						102 // ERROR: Invalid Input Hour
#define RTN_ERRFREQ						103 // ERROR: Invalid Input Frequency
#define RTN_ERRDATAFILEPATH				104 // ERROR: Invalid Input Data File Path
#define RTN_ERRCOMMANDLINEARGS			105 // ERROR: Insufficient Number of Command Line Arguments
#define RTN_ERRCANTOPENFILE				106 // ERROR: Can't open output file
#define RTN_ERRBADDATAFILEPATH          107 // ERROR: Data file path does not exist
#define RTN_ERRV_DCANTOPENFILE			108 // ERROR: Input file .\P372\DATA\V_d.txt can't be opened
#define RTN_ERRSIGMA_V_DCANTOPENFILE	109 // ERROR: Input file .\P372\DATA\sigma_V_d.txt can't be opened

// Successfull return codes 
#define RTN_ITURNOISEOK					 90 // Normal successful exit
#define RTN_ATMOSFILESOK                 91 // All atmospheric noise files created 
// End ITURNoise return codes

// ITURNoise Print Flag 
#define PRINTCSV	101 // Print just csv output line for single run
#define PRINTCSVALL 102 // Print CSV header and line


#endif // ITURNOISE_H
