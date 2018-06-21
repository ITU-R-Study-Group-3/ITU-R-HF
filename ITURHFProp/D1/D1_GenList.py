''' 
D1_GenList.txt - This program reads the file dbank_d1.txt and creates a CSV file
				D1.csv. D1.csv is read by the program D1_InputFiles.py which creates 
				the input files necessary to run ITUHFProp().
'''				

import sys
import struct

###############################################################################################
# There are three tables in dbank_d1.txt
# Table 1 gives the discription of the measured circuit
# Table 2 gives 24 hrs of measurements for a given circuit, year and month
# Table 3 gives SSN for a given year and month
#
# The output file D1.csv requires all three tables to create prediction scenarios for 
# each measurement in Table 2. So for each prediction scenario Table 2 gives the year, month 
# and circuit ID. Given the month and year from Table 2 the SSN is found from Table 3. Given the 
# circuit ID the coordinates of the TX and RX are found from Table 1. All this information is 
# is then used to describe a prediction scenario that can be run by ITUHFProp()	
#################################################################################################

# Open data bank D1 
D1 = open('dbank_d1.txt','r')

# Ignore the first 11 lines in dbank_d1.txt as header and ignore them
for n in range(11):
	line = D1.readline()

# Read Table 1 into variable Table1
Table1 = []
for n in range(181):
	Table1.append(D1.readline())
	
# Ignore the next 7 lines in dbank_d1.txt
for n in range(7):
	line = D1.readline()

# read Table 2 into varible Table2
Table2 = []
for n in range(1613):
	Table2.append(D1.readline())
	
# Ignore the next 9 lines in dbank_d1.txt
for n in range(9):
	line = D1.readline()

# Read Table 3 into variable Table3
Table3 = []
for n in range(22):
	Table3.append(D1.readline())

# Close dbank_d1.tat
D1.close()

# Now unpack each line that was read into Table1
# There are 9 values in each line
# 1) Circuit ID
# 2) TX name
# 3) RX name
# 4) Frequency (MHz)
# 5) TX latitude (Degrees.Minutes)
# 6) TX longitude (Degrees.Minutes)
# 7) RX latitude (Degrees.Minutes)
# 8) RX longitude (Degrees.Minutes)
# 9) distance (km)
T1 = []
for data in Table1:
	T1line = []
	field = struct.unpack('3s13s13s5s7s8s7s8s7s', data.encode("utf-8"))
	for n in range(9):
		T1line.append(field[n].decode("utf-8").strip(' \n'))
	T1.append(T1line)

# Unpack each line in Table2
# There are 27 values in each line
# 1) Circuit ID
# 2) Year
# 3) Month
# 4-27) Measurements (99 represent no measurement
T2 = []
for data in Table2:
	T2line = []
	field = struct.unpack('3s3s2s3s3s3s3s3s3s3s3s3s3s3s3s3s3s3s3s3s3s3s3s3s3s3s4s', data.encode("utf-8"))
	for n in range(27):
		T2line.append(field[n].decode("utf-8").strip(' \n'))
	T2.append(T2line)

# Unpack each line in Table3
# There are 13 values in each line
# 1) Year
# 2-13) Monthly SSN starting in January
T3 = []
for data in Table3:
	T3line = []
	field = struct.unpack('6s5s5s5s5s5s5s5s5s5s5s5s6s', data.encode("utf-8"))
	for n in range(13):
		T3line.append(field[n].decode("utf-8").strip(' \n'))
	T3.append(T3line)

# Open the output file
OPF = open('D1.csv','wt')

# Print a header line
print('ID ,TX,RX,Freq (MHz),TX Lat (D.M),TX Long (D.M),RX Lat (D.M),RX Long (D.M),Distance (km),SSN,Year,Month', file = OPF)

# For every measurement there will be a prediction scenario
for data in T2:
	n = 0 # number of circuits counter for Table 1
	# Stop if you find the circuit ID of interest or run out of circuit IDs (181)
	while ((int(data[0]) != int(T1[n][0])) and (n < 181)):
		n += 1
	m = 0 # month counter
	# The year in Table 2 has 4 digits and Table 1 is only 2 digits so
	# strip 1900 from the 4 digit year
	yr = int(data[1]) + 1900
	# Load the first SSN year from Table 3 to find the SSN of interest
	ssnyr = int(T3[m][0])
	# Stop if the SSN year is found in Table 3 or there are no yearly SSN data (m)
	while ((ssnyr != yr) and (m < 22)):
		m += 1
		ssnyr = int(T3[m][0])
		
	# The SSN month index has been found set it
	ssnmonth = int(data[2])
	
	# At this point everything is correlated for the present prediction scenario data
	# so the line can be generated to output to D1.csv file
	outstr = []
	outstr = data[0] + ',' + T1[n][1] + ','+ T1[n][2] + ','+ T1[n][3] + ','+ T1[n][4] + ','+ T1[n][5] + ','+ T1[n][6] + ','+ T1[n][7] + ',' + T1[n][8] + ',' + T3[m][ssnmonth] + ',' + data[1] + ',' + data[2]
	# Print the prediction scenario line to the 
	print(outstr, file = OPF)
	
# Close D1.csv
OPF.close()
	
