# ITU-R-HF
*******************************************************************************
  International Telecommunications Union - Radiocommunications Sector (ITU-R)
  
                         ITURHFProp(), P533(), and P372()
						 
     A software method for the prediction of the performance of HF circuits
	              based on ITU Recommendation P.533-14 and P.372-13
						   
*******************************************************************************

P533() 
	This program provides methods for the prediction of available frequencies, signal levels, and the predicted reliability for analogue and digital-modulated HF systems, taking into account not only the signal-to-noise ratio but also the expected time and frequency spreads of the channel. This program calculates the HF path parameters that appear in ITU-R Recommnedation P.533-14. 
	Great care has been taken in this implementation to adhere as closely as possible to the Recommendation P.533-14 and to verify the method against the measurement data in the ITU-R D1 databank. 
P372() 
	This program provides the calculations necessary to find the three components of noise that are important to HF propagation predictions: Man-made, Galactic, and Atmospheric. 
ITURHFProp() 
	This program's sole purpose to act as an input and output data wrapper for P533(). ITURHFProp() minimally is as an example of how the P533() model can be used in other applications. Please refer to ITUHFProp() to determine how data is loaded and unloaded from P533() to conduct successfully HF circuit analysis. 

USAGE
	ITURFHProp [Options] [Input File] [Output File]
		Options
			-c Simplified CSV output: The first line of the file is a header with the same format as
			   the subsequent record lines. The header contains names corresponding to the fields in the file and
			   contains the same number of fields as the records in the rest of the file.
			   Selecting this option also removes the header data.
			-h Help: Displays help
			-s Silent Mode: Suppresses display output except for error messages
			-t Strip the header: Creates and output file with the header data removed.
			-v Version: Displays the version of ITUHFProp() and P533()

		Input File
			Full path name of the input file. If no path is given the current directory is assumed.

		Output File
			Full path name of the output file. Note: Existing files will be overwritten. If no output file is 
			indicated a default output file, either a report or a path dump, will be created in the report file 
			directory specified by the RptFilePath parameter in the input file.

Behm 
May 2020
