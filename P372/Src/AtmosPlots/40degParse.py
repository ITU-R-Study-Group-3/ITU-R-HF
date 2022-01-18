deg40file = r'C:\Users\behm\Documents\GitHub\ITU-R-HF\P372\Src\AtmosPlots\40deg0.txt'

d40fp = open(deg40file, 'r')

# Now parse the output files 
lines = d40fp.readlines()

print('*********************************************************')
print('Month, time (UTC), time (LT), lat (deg), Long (deg), Fam (dB)')
for line in lines:

    if('(UTC' in line):
        month = line.split(' ')[0].strip('\t')
        time = line.split(' ')[2]
    if('(deg lat)' in line):
        lat = line.split(' ')[0].strip('\t')
        long = line.split(' ')[3]
    if('Noise Component (Atmospheric)' in line):
        Fam = line.split(' ')[-1].strip('\n')
        lmt = int(time)+int(float(long)/15.0)%24
        if(lmt >= 24):
            lmt = 0
        print('%s,\t%s,\t%s,\t%s,\t%s,\t%s' %(month, time, lmt,lat, long, Fam))

print('*********************************************************')           
    
d40fp.close()