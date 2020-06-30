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

// Successfull return codes 
#define RTN_ITURNOISEOK					 90 // Normal successful exit
#define RTN_ATMOSFILESOK                 91 // All atmospheric noise files created 
// End ITURNoise return codes

// ITURNoise Print Flag 
#define PRINTCSV	101 // Print just csv output line for single run
#define PRINTCSVALL 102 // Print CSV header and line


