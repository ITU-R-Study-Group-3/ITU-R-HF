import numpy as np
import cartopy.crs as ccrs
from cartopy.mpl.gridliner import LONGITUDE_FORMATTER, LATITUDE_FORMATTER
import matplotlib.pyplot as plt
from matplotlib.ticker import (MultipleLocator, FormatStrFormatter,
                               AutoMinorLocator)
import matplotlib.ticker as mticker
import pandas as pd

from math import floor
from math import ceil

# Fixed arrays used for a) Figure axies
a_xtick = []
for tick in range(-180, 190, 15):
    a_xtick.append(tick)

a_ytick = []
for tick in range(-90, 100, 10):
    a_ytick.append(tick)
    
# Fixed arrays used for b) and c) Figure axies
bc_xtick = [ 0.01, 0.02, 0.03, 0.05, 0.07,
            0.1, 0.2, 0.3, 0.5, 0.7,
            1.0, 2.0, 3.0, 5.0, 7.0,
            10.0, 20.0, 30.0]

bc_xlabel = [ '0.01', '0.02', '0.03', '0.05', '0.07',
             '0.1', '0.2', '0.3', '0.5', '0.7',
             '1', '2', '3', '5', '7',
             '10', '20', '30']


#******************************************************************************
# Find the Figure Data Files
#******************************************************************************

# Find the matching b) file data 
cdafile = 'G:\\Behm\\GitHub\\ITU-R-HF\\P372\\Src\\AtmosPlots\\csv\\A_1m1h-26620d-101032t.csv'
cdbfile = 'G:\\Behm\\GitHub\\ITU-R-HF\\P372\\Src\\AtmosPlots\\csv\\B_1m1h-26620d-101032t.csv'
cdcfile = 'G:\\Behm\\GitHub\\ITU-R-HF\\P372\\Src\\AtmosPlots\\csv\\C_1m1h-26620d-101032t.csv'

nfig = 1 # This is the number of output file types
afigfile[0] = 'a_fig.svg'
bfigfile[0] = 'b_fig.svg'
cfigfile[0] = 'c_fig.svg'

#******************************************************************************
# Process the a) file data into a Figure
#******************************************************************************

# Pandas data frame for contour data
dfcd = pd.read_csv(cdafile)

# Create lists from the data
z = dfcd['FaA'].tolist()

zz = np.asarray(z)

# Find the extrema of this data. 
# These valuses will be used to constrain the b) Figure 
minFam1MHz = min(z)
maxFam1MHz = max(z)

'''
    The data in the files scan from bottom-to-top (-90 to 90 degrees latitude)
    then left-to-right (-180 to 180 degrees longitude)
    matplotlib plots assume a left-to-right then bottom-to-top
    consequently the data has to be rotated to plot
    that is why the indecies are for zzz[yidx][xidx] and not zzz[xidx][yidx]
    below
'''

n = 181
m = 361
zzz = np.arange(n*m, dtype=float).reshape(m,n)
for yidx in range(0, n):
    for xidx in range(0, m):
        zzz[xidx][yidx] = zz[xidx + yidx*m] 

# We will need a 
xx =  np.fromiter((i-180 for i in range(m)), float)
yy  =  np.fromiter((i-90 for i in range(n)), float)

yyy,xxx = np.meshgrid(yy,xx)

lvl = []
for ilvl in range(5, 120, 5):
    lvl.append(ilvl)

ax = plt.axes(projection=ccrs.PlateCarree())

#CS = plt.contour(xxx,yyy,zzz,levels=lvl, cmap=plt.cm.gist_ncar)
CS = plt.contour(xxx,yyy,zzz,levels=lvl, cmap=plt.cm.nipy_spectral)

plt.setp(CS.collections , linewidth=0.75)

ax.set_extent([-180, 180, -90, 90], crs=ccrs.PlateCarree())
#ax.set_extent([-136, -134, 50, 70], crs=ccrs.PlateCarree())


plt.clabel(CS, fmt='%d', inline=1, fontsize=3)
#CS2 = plt.contourf(zzz, levels=lvl, color='k')


#ax.coastlines(resolution='50m')
ax.coastlines(linewidth=0.3)
#ax.gridlines(draw_labels=True, dms=True, x_inline=False, y_inline=False)
gl = ax.gridlines(linewidth=0.3, draw_labels=True)
gl.xlabel_style = {'size': 3.5}
gl.ylabel_style = {'size': 3.5}

gl.xlocator = mticker.FixedLocator(a_xtick)
gl.ylocator = mticker.FixedLocator(a_ytick)

gl.xformatter = LONGITUDE_FORMATTER
gl.yformatter = LATITUDE_FORMATTER

# Save the plot by calling plt.savefig() BEFORE plt.show()
#plt.savefig('coastlines.pdf')
#plt.savefig('coastlines.png')
plt.savefig('a_fig.svg')

#******************************************************************************
# Process the b) file data into a Figure
#******************************************************************************

# Pandas data frame for contour data
dfcd = pd.read_csv(cdbfile)

z = []
# Create lists from the data
z.append(dfcd['Fam5'].tolist())
z.append(dfcd['Fam10'].tolist())
z.append(dfcd['Fam20'].tolist())
z.append(dfcd['Fam30'].tolist())
z.append(dfcd['Fam40'].tolist())
z.append(dfcd['Fam50'].tolist())
z.append(dfcd['Fam60'].tolist())
z.append(dfcd['Fam70'].tolist())
z.append(dfcd['Fam80'].tolist())
z.append(dfcd['Fam90'].tolist())
z.append(dfcd['Fam100'].tolist())

x = dfcd['freq'].tolist()

# Determine the relevant indecies for the Fam!MHz data plotted in a) Figure
if(minFam1MHz > 0 and minFam1MHz < 10):
    loFam1MHz = 0
else:
    loFam1MHz = int(((ceil(minFam1MHz)-ceil(minFam1MHz)%10)+10)/10)-1
hiFam1MHz = int(((floor(maxFam1MHz)-floor(maxFam1MHz)%10)+10)/10)
if(hiFam1MHz > 11):
    hiFam1MHz = 11
    
fig,ax = plt.subplots(1,1)

ax.set_prop_cycle

for pidx in range(loFam1MHz, hiFam1MHz):
    strlabel = '%2d' % (pidx*10)
    ax.plot(x, z[pidx],  label=strlabel, lw=0.5)
    
ax.set_xscale('log')

ax.yaxis.set_major_locator(MultipleLocator(20))
ax.yaxis.set_minor_locator(MultipleLocator(5))

for ymaj in ax.yaxis.get_majorticklocs():
  ax.axhline(y=ymaj, lw=0.5)
for ymin in ax.yaxis.get_minorticklocs():
  ax.axhline(y=ymin, lw=0.1)
  
plt.ylim(-20, 180)
plt.xlim(0.01, 30.0)
  
ax.grid(which='both', axis='both')

ax.set_xticks(bc_xtick)
ax.set_xticklabels(bc_xlabel)
ax.tick_params(axis='both', which='both', labelsize=6)

ax.legend(title='$F_{am}$ 1MHz (dB)', title_fontsize=6, fontsize=6,framealpha = 1.0)

plt.xlabel('Frequency (MHz)', fontsize=6)
plt.ylabel('$F_{am}$ (dB above k$T_0$b)', fontsize=6)

plt.savefig('b_fig.svg')
#ax.set_xscale('log')

#******************************************************************************
# Process the c) file data into a Figure
#******************************************************************************

# Find the matching b) file data 
cdcfile = 'G:\\Behm\\GitHub\\ITU-R-HF\\P372\\Src\\AtmosPlots\\csv\\C_1m1h-26620d-101032t.csv'

# Pandas data frame for contour data
dfcd = pd.read_csv(cdcfile)

z = []
# Create lists from the data
z.append(dfcd['DuA'].tolist())
z.append(dfcd['DlA'].tolist())
z.append(dfcd['sigmaFaA'].tolist())
z.append(dfcd['sigmaDuA'].tolist())
z.append(dfcd['sigmaDlA'].tolist())

x = dfcd['freq'].tolist()

fig,ax = plt.subplots(1,1)

ax.set_prop_cycle

strlabel = ['$D_u$', '$D_l$', '$\sigma_{F_{am}}$', '$\sigma_{D_u}$', '$\sigma_{D_l}$']

for pidx in range(0,5):
    if (pidx == 2):
        # The sigma Fam never goes beyond 10 MHz so truncate
        ax.plot(x[:37], z[pidx][:37],  label=strlabel[pidx], lw=0.5)
    else:
        ax.plot(x, z[pidx],  label=strlabel[pidx], lw=0.5)
    
ax.set_xscale('log')

ax.yaxis.set_major_locator(MultipleLocator(2))
ax.yaxis.set_minor_locator(MultipleLocator(1))

for ymaj in ax.yaxis.get_majorticklocs():
  ax.axhline(y=ymaj, lw=0.5)
for ymin in ax.yaxis.get_minorticklocs():
  ax.axhline(y=ymin, lw=0.1)
  
ax.grid(which='both', axis='both')

ax.set_xticks(bc_xtick)
ax.set_xticklabels(bc_xlabel)
ax.tick_params(axis='both', which='both', labelsize=6)

# Set the Figure upper limit roughly 2 more than the max DuA
cmax = (ceil(max(z[0]))-ceil(max(z[0]))%2)+2
plt.ylim(0,cmax)
plt.xlim(0.01, 20.0)

ax.legend(title='$F_{am}$\nStatistics', title_fontsize=5, fontsize=5,framealpha = 1.0)

plt.xlabel('Frequency (MHz)', fontsize=6)
plt.ylabel('(dB)', fontsize=6)

plt.savefig('c_fig.svg')

plt.show()

