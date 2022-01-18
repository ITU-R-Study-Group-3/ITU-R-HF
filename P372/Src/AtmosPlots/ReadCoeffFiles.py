import os
import math
import glob as gb

# This function can be used to remove contiguous spaces since the phase 1, 
# phase2, and low antenna data has fixed spaced fields from FORTRAN output
deduplicate = lambda s,c: c.join([substring for substring in s.strip().split(c) if substring])

COEFFdir = r'C:\Users\behm\Documents\GitHub\ITU-R-HF\P372\Data'
COEFFfn = r'COEFF??W.TXT'

COEFFfpath = os.path.join(COEFFdir, COEFFfn)

allCOEFFfiles = gb.glob(COEFFfpath)

outfp = open('atmosdataedges.txt', 'w')

N = []
S = []
for COEFFfile in allCOEFFfiles:
    
    print(r'CCIR COEFF FILE: %s\n', COEFFfile)
    COEFFfp = open(COEFFfile, 'r')
    lines = COEFFfp.readlines()
    
    imonth = COEFFfile.split('\\')[-1].split('.')[0][5:7]
    print(imonth, end=',', file=outfp)
    
    for iline, line in enumerate(lines):
        
        A = []
        if(r'fam(14,12)' in line ):
            nol = math.ceil(14*12/5) # number of lines including 
            for Adata in lines[iline+1:iline+1+nol]: 
               Adata = deduplicate(Adata.strip('\n'), ' ').split(' ')
               for Aval in Adata:
                   A.append(Aval)

            for i in range(12): # j = 0 to 11
                istart = 14*i
                iend = 14*i + 13
                
                if (i < 6):
                    print('\tTime Block %d-%d North: %s %s\n' % (4*i, 4*i+4, A[istart], A[iend]))
                    N.append([A[istart], A[iend]])
                    print(A[istart], end=',', file=outfp)
                    print(A[iend], end=',', file=outfp)
                elif ((i >= 6) and (i <= 10)):
                    print('\tTime Block %d-%d South: %s %s\n' % (4*(i-6), 4*(i-6)+4, A[istart], A[iend]))
                    S.append([A[istart], A[iend]])
                    print(A[istart], end=',', file=outfp)
                    print(A[iend], end=',', file=outfp)
                elif (i == 11):
                    print('\tTime Block %d-%d South: %s %s\n' % (4*(i-6), 4*(i-6)+4, A[istart], A[iend]))
                    S.append([A[istart], A[iend]])
                    print(A[istart], end=',', file=outfp)
                    print(A[iend], file=outfp)

'''

        if(r'dud(5,12,5)' in line ):
            print(line)
       
        if(r'fakabp(2,6)' in line ):
            print(line)
               
        if(r'fakp(29,16,6)' in line ):
            print(line)
'''
           
outfp.close()
            
        