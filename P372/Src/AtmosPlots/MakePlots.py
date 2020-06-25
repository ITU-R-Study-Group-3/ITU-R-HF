import numpy as np
import cartopy.crs as ccrs
import matplotlib.pyplot as plt
from matplotlib.ticker import (MultipleLocator, FormatStrFormatter,
                               AutoMinorLocator)
import pandas as pd

# Contour data file
cdfile = "G:\\Behm\\GitHub\\ITU-R-HF\\P372\\Src\\AtmosPlots\\csv\\A_1m1h-24620d-161057t.csv"

# Pandas data frame for contour data
dfcd = pd.read_csv(cdfile)

# Create lists from the data
z = dfcd["FaA"].tolist()

zz = np.asarray(z)

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
for ilvl in range(5, 90, 5):
    lvl.append(ilvl)

ax = plt.axes(projection=ccrs.PlateCarree())

CS = plt.contour(xxx,yyy,zzz,levels=lvl, cmap=plt.cm.gist_ncar)

plt.setp(CS.collections , linewidth=0.75)

ax.set_extent([-180, 180, -90, 90], crs=ccrs.PlateCarree())
#ax.set_extent([-136, -134, 50, 70], crs=ccrs.PlateCarree())


plt.clabel(CS, fmt="%d", inline=1, fontsize=5)
#CS2 = plt.contourf(zzz, levels=lvl, color='k')


#ax.coastlines(resolution='50m')
ax.coastlines(linewidth=0.3)
#ax.gridlines(draw_labels=True, dms=True, x_inline=False, y_inline=False)
ax.gridlines(linewidth=0.3, draw_labels=True)

# Save the plot by calling plt.savefig() BEFORE plt.show()
#plt.savefig('coastlines.pdf')
#plt.savefig('coastlines.png')
plt.savefig('coastlines.svg')

plt.show()

