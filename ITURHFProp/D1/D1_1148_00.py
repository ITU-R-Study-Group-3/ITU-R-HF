import sys
import os
import operator
from glob import glob
import csv
import math

# Constants
D2R = math.pi/180.0
R2D = 180.0/math.pi
# Earth radius
R0 = 6371.0

# Open output text file
D1Comp = open("D1Comp.csv", 'wt')
# Print the header
print(' ID, Y, M,01,02,03,04,05,06,07,08,09,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24', file=D1Comp)

# This program assumes that it is in the .\D1\ directory
# and that there is a .\D1\out\ directory that is populated with the *.out 
# files generated from ITUHFProp from the D1 database
outfiles = glob(".\\out\\*.out")

for D1file in outfiles:
	file = open(D1file, "rt")
	
	# Read the file in chunks
	for line in file:
		# Read to the Header
		if(line.count('* P533 Input Parameters *') != 0):
			break
	# Read Header until you find 'Data Format'
	for line in file:
		if(line.count('Data Format') != 0):
			break
		# Find the following in the header
		if(line.count('Test case ID') != 0):
			TestCase = line.split()
			Year = int(TestCase[5])
			Month = int(TestCase[7])
			ID = int(TestCase[3].split(":")[0])
			
		elif(line.count('Rx Location') != 0):
			RX = line.split()[2]
		elif(line.count('SSN (R12) =') != 0):
			SSN = int(line.split(" = ")[1])
		elif (line.count('Hour =') != 0):
			Hour = int(line.split()[2])
		elif (line.count('Distance =') != 0):
			Distance = float(line.split()[2])
		elif (line.count('Frequency') != 0):
			Freq = float(line.split()[2])
	
	# Read to the data
	for line in file:
		if(line.count('Calculated Parameters') != 0):
			break
	
	data = []
	for line in file:
		if(line.count(',') == 0):
			continue # Ignore the line
		elif(line.count('End Calculated Parameters') != 0):
			break # End of the file
		else:
			data.append(-float(line.replace(' |\n',' ').split(',')[6]))
	
	# Format the output
	print('%3d,' % ID, file=D1Comp, end="")
	print('%02d,' % Year, file=D1Comp, end="")
	print('%02d,' % Month, file=D1Comp, end="")
	for n in range(len(data)):
		if(data[n] >= 99.5):
			print('99', file=D1Comp, end="")
		else:
			print('%2.0f' % data[n], file=D1Comp, end="")
		if(n != 23):
			print(',', file=D1Comp, end="")
	print(file=D1Comp)
	
D1Comp.close()

# Now sort the file	
D1Comp = open('D1Comp.csv','r')
D1Comp_csv = csv.reader(D1Comp)	
sortedlist = sorted(D1Comp_csv, key=operator.itemgetter(0,1,2))
D1Comp.close()

# Replace the file
D1Comp = open('D1Comp.csv','w')
D1Comp_csv = csv.writer(D1Comp, lineterminator = "\r")	

for row in sortedlist:
	D1Comp_csv.writerow(row)	

D1Comp.close()		

#################################################################
############ Now Start the Comparison ###########################
#################################################################
# Open the D1 Compare file
D1Comp = open('D1Comp.csv','r')

# Open D1 Table 1
D1T1 = open('D1_Table1.csv','r')

# Open D1 Table 2
D1T2 = open('D1_Table2.csv','r')

# Open D1 Table 3
D1T3 = open('D1_Table3.csv','r')

FG1 = []
FG2 = []
FG3 = []
FG4 = []

D01 = []
D02 = []
D03 = []
D04 = []
D05 = []
D06 = []
D07 = []
D08 = []
D09 = []
D10 = []
D11 = []
D12 = []

SSN1 = []
SSN2 = []
SSN3 = []
SSN4 = []
SSN5 = []
SSN6 = []

WINTER = []
SUMMER = []
SPRING = []
AUTUMN = []

GL1 = []
GL2 = []
GL3 = []
GL4 = []

MLT1 = []
MLT2 = []
MLT3 = []
MLT4 = []
MLT5 = []
MLT6 = []

GER	= []
JPN	= []
CHN = []
IND	= []
DW = []
BBC = []
AUS = []

AD1 = []
AD2 = []

LT1 = []
LT2 = []
LT3 = []
LT4 = []
LT5 = []
LT6 = []

# Ignore the first line in the D1 Table 1 file
D1T1data = D1T1.readline()

# Ignore the first line in the D1 Table 2 file
D1T2data = D1T2.readline()

m = 1

for D1Compdata in D1Comp:

	# Parse the data to compare
	D1Compdata = D1Compdata.split(',')
	
	# Read the corresponding D1 Table 1 data
	D1T1data = D1T1.readline().split(',')

	ID = int(D1T1data[0])
	freq = float(D1T1data[3])
	dist = float(D1T1data[8])
	ssn = int(D1T1data[9])
	season = int(D1T1data[11])
	TXname = D1T1data[1]	
	
	# Determine where there the receiver is
	TXlat = math.radians(float(D1T1data[4].strip('NS').split('.')[0])+(float(D1T1data[4].strip('NS').split('.')[1])/60))
	NorS = D1T1data[4].strip('-0123456789.')
	TXlng = math.radians(float(D1T1data[5].strip('EW').split('.')[0])+(float(D1T1data[5].strip('EW').split('.')[1])/60))
	EorW = D1T1data[5].strip('-0123456789.')
		
	if(NorS == 'S'):
		TXlat = -TXlat
	if(EorW == 'W'):
		TXlng = -TXlng
			
	RXlat = math.radians(float(D1T1data[6].strip('NS').split('.')[0])+(float(D1T1data[6].strip('NS').split('.')[1])/60))
	NorS = D1T1data[6].strip('-0123456789.')
	RXlng = math.radians(float(D1T1data[7].strip('EW').split('.')[0])+(float(D1T1data[7].strip('EW').split('.')[1])/60))
	EorW = D1T1data[6].strip('-0123456789.')

	if(NorS == 'S'):
		RXlat = -RXlat
	if(EorW == 'W'):
		RXlng = -RXlng
	
	# Determine the path distance
	distance = 2.0*R0*math.asin(math.sqrt(math.pow((math.sin((TXlat-RXlat)/2.0)),2.0) + math.cos(TXlat)*math.cos(RXlat)*math.pow((math.sin((TXlng - RXlng)/2.0)),2.0))) 

	ID = int(D1T1data[0])
	if(ID >= 169): # Long way round
		distance = 2.0*R0*math.pi - distance
		
	# Find the path mid point
	d = distance/R0
	A = math.sin((0.5)*d)/math.sin(d)
	B = math.sin(0.5*d)/math.sin(d)
	x = A*math.cos(TXlat)*math.cos(TXlng) +  B*math.cos(RXlat)*math.cos(RXlng);
	y = A*math.cos(TXlat)*math.sin(TXlng) +  B*math.cos(RXlat)*math.sin(RXlng);
	z = A*math.sin(TXlat) +  B*math.sin(RXlat);
	midlat = math.atan2(z,math.sqrt(math.pow(x,2)+math.pow(y,2)));
	midlng = math.atan2(y,x);
	
	# Find the gormagnetic latitude
	GeoMagNPolelat = 78.5*D2R; 
	GeoMagNPolelng = -68.2*D2R;

	gmmidlat = math.asin(math.sin(midlat)*math.sin(GeoMagNPolelat) + math.cos(midlat)*math.cos(GeoMagNPolelat)*math.cos(midlng - GeoMagNPolelng));
	gmmidlng = math.asin(math.cos(midlat)*math.sin(midlng - GeoMagNPolelng)/math.cos(gmmidlat));
	
	# Determine the mid path time offset from the Rx
	if(ID >= 169): # Long way round
		ltimeoffset = int((midlat - RXlat)/(15.0*D2R))
	else:
		ltimeoffset = int((RXlat-midlat)/(15.0*D2R))


	# Read the measurement data
	# Read the corresponding D1 Table 2 data
	D1T2data = D1T2.readline().split(',')
	if(int(D1Compdata[0]) != int(D1T2data[0])):
		print('Error: IDs #',m,' do not match. Prediction ID ', int(D1Compdata[0]), 'D1 ', int(D1T2data[0])) 
		sys.exit()
	
	# Check to see if the IDs match if not print an error and exit
	

	for hr in range(24):
		
		ltime = hr + ltimeoffset
		# roll over the hour
		if(ltime > 23):
			ltime = 0
		elif(ltime < 0):
			ltime = 23
				
		if(int(D1T2data[hr+3]) != 99):
			diff = int(D1Compdata[hr+3]) - int(D1T2data[hr+3])
			# Frequency Groups
			if(freq <= 5):
				FG1.append(diff)
			elif((freq > 5 ) and (freq <= 10)):
				FG2.append(diff)
			elif((freq > 10 ) and (freq <= 15)):
				FG3.append(diff)
			elif((freq > 15 ) and (freq <= 30)):
				FG4.append(diff)
				
			# Distances
			if((dist >= 0 ) and (dist <= 999)):
				D01.append(diff)
			elif((dist > 1000 ) and (dist <= 1999)):
				D02.append(diff)
			elif((dist > 2000 ) and (dist <= 2999)):
				D03.append(diff)
			elif((dist > 3000 ) and (dist <= 3999)):
				D04.append(diff)
			elif((dist > 4000 ) and (dist <= 4999)):
				D05.append(diff)
			elif((dist > 5000 ) and (dist <= 6999)):
				D06.append(diff)
			elif((dist > 7000 ) and (dist <= 8999)):
				D07.append(diff)
			elif((dist > 9000 ) and (dist <= 11999)):
				D08.append(diff)
			elif((dist > 12000 ) and (dist <= 14999)):
				D09.append(diff)
			elif((dist > 15000 ) and (dist <= 17999)):
				D10.append(diff)
			elif((dist > 18000 ) and (dist <= 21999)):
				D11.append(diff)
			elif((dist > 22000 ) and (dist <= 40000)):
				D12.append(diff)
				
			# Long or Short distances
			if(dist < 9000 ):
				AD1.append(diff)
			elif(dist > 7000 ):
				AD2.append(diff)
				
			# Sun Spot Number
			if((ssn >= 0 ) and (ssn <= 14)):
				SSN1.append(diff)
			elif((ssn >= 15 ) and (ssn <= 44)):
				SSN2.append(diff)
			elif((ssn >= 45 ) and (ssn <= 74)):
				SSN3.append(diff)
			elif((ssn >= 75 ) and (ssn <= 104)):
				SSN4.append(diff)
			elif((ssn >= 105 ) and (ssn <= 149)):
				SSN5.append(diff)
			elif(ssn > 149):
				SSN6.append(diff)
			
			# Mdiffpoint hour
			if((ltime >= 0 ) and (ltime <= 3)):
				LT1.append(diff)
			elif((ltime > 3 ) and (ltime <= 7)):
				LT2.append(diff)
			elif((ltime > 8 ) and (ltime <= 11)):
				LT3.append(diff)
			elif((ltime > 12 ) and (ltime <= 15)):
				LT4.append(diff)
			elif((ltime > 16 ) and (ltime <= 19)):
				LT5.append(diff)
			elif((ltime > 19 ) and (ltime <= 23)):
				LT6.append(diff)

			
			# Season
			if(midlat >= 0): # Northern hemisphere
				if((season == 11 ) or (season == 12) or (season == 1) or (season == 2)):
					WINTER.append(diff)
				elif((season == 3 ) or (season == 4)):
					SPRING.append(diff)
				elif((season == 5 ) or (season == 6) or (season == 7) or (season == 8)):
					SUMMER.append(diff)
				elif((season == 9 ) or (season == 10)):
					AUTUMN.append(diff)
			elif(midlat < 0):
				if((season == 11 ) or (season == 12) or (season == 1) or (season == 2)):
					SUMMER.append(diff)
				elif((season == 3 ) or (season == 4)):
					AUTUMN.append(diff)
				elif((season == 5 ) or (season == 6) or (season == 7) or (season == 8)):
					WINTER.append(diff)
				elif((season == 9 ) or (season == 10)):
					SPRING.append(diff)
					
			# Geomagnetic Latitude
			gmmidlat = gmmidlat*R2D
			
			if((gmmidlat == 0 ) and (gmmidlat <= 20)):
				GL1.append(diff)
			elif((gmmidlat > 20 ) and (gmmidlat <= 40)):
				GL2.append(diff)
			elif((gmmidlat > 40 ) and (gmmidlat <= 60)):
				GL3.append(diff)
			elif(gmmidlat > 60 ):
				GL4.append(diff)
				
			# Origin of data
			if(TXname == 'ALLOUIS'):
				BBC.append(diff)
			elif(TXname == 'ANKARA'):
				BBC.append(diff)
			elif(TXname == 'ASCENSION'):
				BBC.append(diff)
			elif(TXname == 'BEIJING'):
				CHN.append(diff)
			elif(TXname == 'BOMBAY'):
				IND.append(diff)
			elif(TXname == 'BRACKNELL'):
				BBC.append(diff)
			elif(TXname == 'CANBERRA'):
				AUS.append(diff)
			elif(TXname == 'CANBERRA LP'):
				AUS.append(diff)
			elif(TXname == 'CARNARVON'):
				AUS.append(diff)
			elif(TXname == 'DARWIN'):
				AUS.append(diff)
			elif(TXname == 'DAVENTRY'):
				BBC.append(diff)
			elif(TXname == 'DELANO'):
				BBC.append(diff)
			elif(TXname == 'DERBY'):
				AUS.append(diff)
			elif(TXname == 'EKALA'):
				IND.append(diff)
			elif(TXname == 'FORT COLLINS'):
				BBC.append(diff)
			elif(TXname == 'GREENVILLE'):
				BBC.append(diff)
			elif(TXname == 'HYDERABAD'):
				IND.append(diff)
			elif(TXname == 'JERUSALEM'):
				BBC.append(diff)
			elif(TXname == 'KAUAI'):
				AUS.append(diff)
			elif(TXname == 'KAVALLA'):
				BBC.append(diff)
			elif(TXname == 'KOGANEI'):
				JPN.append(diff)
			elif(TXname == 'KRANJI'):
				CHN.append(diff)
			elif(TXname == 'KURSEONG'):
				IND.append(diff)
			elif(TXname == 'KUWAIT'):
				BBC.append(diff)
			elif(TXname == 'LUXEMBURG'):
				DW.append(diff)
			elif(TXname == 'MAHE'):
				IND.append(diff)
			elif(TXname == 'MASIRAH'):
				BBC.append(diff)
			elif(TXname == 'MAURITIUS'):
				IND.append(diff)
			elif(TXname == 'MEYERTON'):
				BBC.append(diff)
			elif(TXname == 'NEW YORK'):
				BBC.append(diff)
			elif(TXname == 'NORFOLK'):
				BBC.append(diff)
			elif(TXname == 'OSLO'):
				BBC.append(diff)
			elif(TXname == 'PLYMOUTH'):
				BBC.append(diff)
			elif(TXname == 'PORI'):
				BBC.append(diff)
			elif(TXname == 'PORO'):
				CHN.append(diff)
			elif(TXname == 'QUITO'):
				AUS.append(diff)
			elif(TXname == 'RANCHI'):
				IND.append(diff)
			elif(TXname == 'SACKVILLE'):
				AUS.append(diff)
			elif(TXname == 'SANWA'):
				JPN.append(diff)
			elif(TXname == 'SANWA LP'):
				JPN.append(diff)
			elif(TXname == 'SHANNON'):
				BBC.append(diff)
			elif(TXname == 'SHEPPART LP'):
				BBC.append(diff)
			elif(TXname == 'SHEPPARTON'):
				BBC.append(diff)
			elif(TXname == 'SKELTON'):
				BBC.append(diff)
			elif(TXname == 'TEHERAN'):
				IND.append(diff)
			elif(TXname == 'TINANG'):
				CHN.append(diff)
			elif(TXname == 'TOKYO'):
				JPN.append(diff)
			elif(TXname == 'WASHINGTON'):
				BBC.append(diff)
			elif(TXname == 'WERTACHTAL'):
				BBC.append(diff)
			elif(TXname == 'XIAN'):
				CHN.append(diff)
	# Increment the line counter
	m = m + 1
# Output the report						
print(' Frequency group (MHz)\t\tcount\t\tMean(dB)\t\tStd (dB)')	
n = 0	
sum = 0
sqrsum = 0
for data in FG1:
	n =  n + 1
	sum = sum + data
	sqrsum = sqrsum + math.pow(data, 2.0)
mean = sum/n
std = math.sqrt((sqrsum-(math.pow(sum,2.0)/n))/(n - 1))	
	
print(' 2 - 5 ',  n, mean, std) 

n = 0	
sum = 0
sqrsum = 0
for data in FG2:
	n =  n + 1
	sum = sum + data
	sqrsum = sqrsum + math.pow(data, 2.0)
mean = sum/n
std = math.sqrt((sqrsum-(math.pow(sum,2.0)/n))/(n - 1))	
	
print(' >5 -10 ',  n, mean, std) 

n = 0	
sum = 0
sqrsum = 0
for data in FG3:
	n =  n + 1
	sum = sum + data
	sqrsum = sqrsum + math.pow(data, 2.0)
mean = sum/n
std = math.sqrt((sqrsum-(math.pow(sum,2.0)/n))/(n - 1))	
	
print(' >10 - 15 ',  n, mean, std) 

n = 0	
sum = 0
sqrsum = 0
for data in FG4:
	n =  n + 1
	sum = sum + data
	sqrsum = sqrsum + math.pow(data, 2.0)
mean = sum/n
std = math.sqrt((sqrsum-(math.pow(sum,2.0)/n))/(n - 1))	
	
print(' >15 - 30 ',  n, mean, std) 
