import sys
import os

# Open output text file
bat_file = open("D1.bat", 'wt')

# Make the directories if they don't exist
inpath = "in" 
if not os.path.exists(inpath): os.makedirs(inpath)
outpath = "out" 
if not os.path.exists(outpath): os.makedirs(outpath)

in_file = open("D1.csv", "rt")

# Read the first line header and do nothing
in_file.readline()

for line in in_file:
	'''
	Now you can deal with each line which has the following format
	
	0	ID				179
	1	Tx Name			SHEPPART.LP
	2	Rx Name			JOKELA
	3	Freq (MHz)		11.9
	4	Tx Lat (D.M)	36.20S
	5	Tx Lng (D.M)	145.25E
	6	Rx Lat (D.M)	60.34N
	7	RX Lng (D.M)	25.00E
	8	Distance (km)	24935
	9	SSN				29
	10	Year			84
	11	Month			10
	'''
	
	# Convert the line to ASCII for convienience
	str = line.strip('\n').split(',')
	
	out_file_name = inpath+'\\'+str[0]+'-'+str[11]+'-'+str[10]+'.in'
	
	# Create the batch file command associated with this *.in file 
	batstr = 'C:\\Users\\Behm\\Documents\\GitHub\\GitHf\\Bin\\ITUHFProp -s '+out_file_name+' out\\'+str[0]+'-'+str[11]+'-'+str[10]+'.out'
	print(batstr, file=bat_file) 
	
	# Open output text file
	out_file = open(out_file_name, 'wt')
	
	# PathName
	outstr = 'PathName "Test case ID '+str[0]+': Year '+str[10]+' Month '+str[11]+'"'
	print(outstr, file=out_file) 
	
	# PathTXName
	outstr = 'PathTXName "'+str[1]+'"'
	print(outstr, file=out_file)
	
	# Path.L_tx.lat
	outstr = 'Path.L_tx.lat '
	if(str[4].find('S') != -1): # South
		txlat = '-'+str[4].strip('S')
	else:
		txlat = str[4].strip('N')
	outstr = outstr + txlat
	print(outstr, file=out_file)
	
	# Path.L_tx.lng
	outstr = 'Path.L_tx.lng '
	if(str[5].find('W') != -1): # West
		txlng = '-'+str[5].strip('W')
	else:
		txlng = str[5].strip('E')
	outstr = outstr + txlng
	print(outstr, file=out_file)

	outstr = 'TXAntFilePath "ISOTROPIC"'
	print(outstr, file=out_file)
	
	outstr = 'TXGOS 0.0'
	print(outstr, file=out_file)
	
	# PathRXName
	outstr = 'PathRXName "'+str[2]+'"'
	print(outstr, file=out_file)
	
	# Path.L_rx.lat
	outstr = 'Path.L_rx.lat '
	if(str[6].find('S') != -1): # South
		rxlat = '-'+str[6].strip('S')
	else:
		rxlat = str[6].strip('N')
	outstr = outstr + rxlat
	print(outstr, file=out_file)
	
	# Path.L_rx.lng
	outstr = 'Path.L_rx.lng '
	if(str[7].find('W') != -1): # West
		rxlng = '-'+str[7].strip('W')
	else:
		rxlng = str[7].strip('E')
	outstr = outstr + rxlng
	print(outstr, file=out_file)
	
	outstr = 'RXAntFilePath "ISOTROPIC"'
	print(outstr, file=out_file)
	outstr = 'RXGOS 0.0'
	print(outstr, file=out_file)
	outstr = 'AntennaOrientation "ARBITRARY"'
	print(outstr, file=out_file)
	
	outstr = 'TXBearing 0.0'
	print(outstr, file=out_file)
	outstr = 'RXBearing 0.0'
	print(outstr, file=out_file)
	
	# Path.year
	outstr = 'Path.year 19'+str[10]
	print(outstr, file=out_file)
	
	# Path.month
	outstr = 'Path.month '+str[11]
	print(outstr, file=out_file)
	
	# Path.hour 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24 
	outstr = 'Path.hour 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24'
	print(outstr, file=out_file)
	
	# Path.SSN 51
	outstr = 'Path.SSN '+str[9]
	print(outstr, file=out_file)
	
	# Path.frequency
	outstr = 'Path.frequency '+str[3]
	print(outstr, file=out_file)
	
	# Path.txpower 
	outstr = 'Path.txpower 0.0'
	print(outstr, file=out_file)
	
	outstr = 'Path.BW 6000.0'
	print(outstr, file=out_file)
	outstr = 'Path.SNRr 38.4'
	print(outstr, file=out_file)
	outstr = 'Path.Relr 86'
	print(outstr, file=out_file)
	outstr = 'Path.ManMadeNoise "RESIDENTIAL"'
	print(outstr, file=out_file)
	outstr = 'Path.Modulation "ANALOG"'
	print(outstr, file=out_file)
	outstr = 'Path.SIRr 23.76'
	print(outstr, file=out_file)
	outstr = 'Path.A 0.0'
	print(outstr, file=out_file)
	outstr = 'Path.TW 0.0'
	print(outstr, file=out_file)
	outstr = 'Path.FW 0.0'
	print(outstr, file=out_file)
	outstr = 'Path.T0 0.0'
	print(outstr, file=out_file)
	outstr = 'Path.F0 0.0'
	print(outstr, file=out_file)
	outstr = 'Path.SorL "SHORTPATH"'
	print(outstr, file=out_file)

	outstr = 'RptFilePath "C:\\Users\\Behm\\Documents\\GitHub\\GitHF\\Reports\\"'
	print(outstr, file=out_file)
	outstr = 'RptFileFormat = "RPT_E | RPT_PR | RPT_RXLOCATION | RPT_D"'
	print(outstr, file=out_file)

	outstr = 'LL.lat '+rxlat
	print(outstr, file=out_file)
	outstr = 'LL.lng '+rxlng
	print(outstr, file=out_file)
	outstr = 'LR.lat '+rxlat
	print(outstr, file=out_file)
	outstr = 'LR.lng '+rxlng
	print(outstr, file=out_file)
	outstr = 'UL.lat '+rxlat
	print(outstr, file=out_file)
	outstr = 'UL.lng '+rxlng
	print(outstr, file=out_file)
	outstr = 'UR.lat '+rxlat
	print(outstr, file=out_file)
	outstr = 'UR.lng '+rxlng
	print(outstr, file=out_file)
	outstr = 'latinc 1.0'
	print(outstr, file=out_file)
	outstr = 'lnginc 1.0'
	print(outstr, file=out_file)
	outstr = 'DataFilePath "C:\\Users\\Behm\\Documents\\GitHub\\GitHF\\Data\\"'
	print(outstr, file=out_file)
	
	out_file.close()
	
bat_file.close()
	
in_file.close()
