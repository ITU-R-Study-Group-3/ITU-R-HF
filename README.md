# ITU-R-HF
*******************************************************************************
  International Telecommunications Union - Radiocommunications Sector (ITU-R)
  
                         P533() and ITURHFProp()
						 
     A software method for the prediction of the performance of HF circuits
	              based on ITU Recommendation P.533-12
						   
*******************************************************************************


P533() 
	This program provides methods for the prediction of available frequencies, signal levels, and the predicted reliability for analogue and digital-modulated HF systems, taking into account not only the signal-to-noise ratio but also the expected time and frequency spreads of the channel. This program calculates the HF path parameters that appear in ITU-R P.533-12. 
	Great care has been taken in this implementation to follow the ITU published standard P.533-12. The flow of this implementation was designed primarily for readability and clarity, with performance being the secondary goal. It is hoped that this code will illuminate the standard and be easily maintainable.

ITURHFProp() 
	This program's sole purpose to act as an input and output data wrapper for P533(). This routine is as an example of how the P533() model can be used in other applications. Please refer to ITUHFProp() to determine how data is loaded and unloaded from P533() to conduct successfully HF circuit analysis. 

USAGE
	ITURFHProp [Options] [Input File] [Output File]
		Options
			-s Silent Mode: Suppresses display output except for error
			   messages
			-h Help: Displays help
			-v Version: Displays the version of ITUHFProp() and P533()

		Input File
			Full path name of the input file. If no path is given the
			current directory is assumed.

		Output File
			Full path name of the output file. Note: Existing files will be 
			overwritten. 
			If no output file is indicated a default output file, either a
			report or a path dump file will be created in
			the .\\Report file directory

Behm 
21 June 2018
