#%% MakeP372figs.py ###########################################################
# This script produces Figures 13a through 36c in ITU-R Recommendation P.372.
# This script uses the data produced by ITURNoise.exe in Mode 2 as input for
# the plots.


#%% Imports ###################################################################
import os
import glob
import numpy as np
import cartopy.crs as ccrs
from cartopy.mpl.gridliner import LONGITUDE_FORMATTER, LATITUDE_FORMATTER
import matplotlib.pyplot as plt
from matplotlib.ticker import MultipleLocator
import matplotlib.ticker as mticker
import pandas as pd
from math import ceil
import re
import shutil


#%% Global ####################################################################
# Fixed arrays used for a) Figure axes
a_xtick = []
for tick in range(-180, 190, 15):
    a_xtick.append(tick)

a_ytick = []
for tick in range(-90, 100, 10):
    a_ytick.append(tick)
    
# Fixed arrays used for b) and c) Figure axes
bc_xtick = [ 0.01, 0.02, 0.03, 0.05, 0.07,
            0.1, 0.2, 0.3, 0.5, 0.7,
            1.0, 2.0, 3.0, 5.0, 7.0,
            10.0, 20.0, 30.0]

bc_xlabel = [ '0.01', '0.02', '0.03', '0.05', '0.07',
             '0.1', '0.2', '0.3', '0.5', '0.7',
             '1', '2', '3', '5', '7',
             '10', '20', '30']

# Input file directories
acsvfilepath = os.path.join(os.getcwd(), 'P372_figures', 'a', 'csv')
bcsvfilepath = os.path.join(os.getcwd(), 'P372_figures', 'b', 'csv')
ccsvfilepath = os.path.join(os.getcwd(), 'P372_figures', 'c', 'csv')

# Output file directories
asvgfilepath = os.path.join(os.getcwd(), 'P372_figures', 'a', 'svg')
bsvgfilepath = os.path.join(os.getcwd(), 'P372_figures', 'b', 'svg')
csvgfilepath = os.path.join(os.getcwd(), 'P372_figures', 'c', 'svg')

apngfilepath = os.path.join(os.getcwd(), 'P372_figures', 'a', 'png')
bpngfilepath = os.path.join(os.getcwd(), 'P372_figures', 'b', 'png')
cpngfilepath = os.path.join(os.getcwd(), 'P372_figures', 'c', 'png')

apdffilepath = os.path.join(os.getcwd(), 'P372_figures', 'a', 'pdf')
bpdffilepath = os.path.join(os.getcwd(), 'P372_figures', 'b', 'pdf')
cpdffilepath = os.path.join(os.getcwd(), 'P372_figures', 'c', 'pdf')


#%% parseFilename #############################################################
def parseFilename(file):
    
    (path,filename) = os.path.split(file)
    matches = re.findall('\d+', filename)
    m = int(matches[0])
    h = int(matches[1])
    
    if m == 12 or m == 1 or m == 2:
        season = 'DEC-JAN-FEB'
    elif m == 3 or m == 4 or m == 5:
        season = 'MAR-APR-MAY'
    elif m == 6 or m == 7 or m == 8:
        season = 'JUN-JUL-AUG'
    elif m == 9 or m == 10 or m == 11:
        season = 'SEP-OCT-NOV'
    
    if h >= 0 and h < 4:
        time_lt = '0000-0400_LT'
    elif h >= 4 and h < 8:
        time_lt = '0400-0800_LT'
    elif h >= 8 and h < 12:
        time_lt = '0800-1200_LT'
    elif h >= 12 and h < 16:
        time_lt = '1200-1600_LT'
    elif h >= 16 and h < 20:
        time_lt = '1600-2000_LT'
    elif h >= 20 and h < 24:
        time_lt = '2000-2400_LT'
            
    return season, time_lt


#%% Makeafigs #################################################################
def Makeafigs(cdafile):
    # Process the a) file data into figures
    print('\nCreating a figure for %s' % cdafile)
    
    season, time_lt = parseFilename(cdafile)
    

    # Pandas data frame for contour data
    dfcd = pd.read_csv(cdafile)
    
    # Create lists from the data
    z = dfcd['FaA'].tolist()
    
    zz = np.asarray(z)
    
    '''
        The data in the files scan from bottom-to-top (-90 to 90 degrees latitude)
        then left-to-right (-180 to 180 degrees longitude)
        matplotlib plots assume a left-to-right then bottom-to-top
        consequently the data has to be rotated to plot
        that is why the indicies are for zzz[yidx][xidx] and not zzz[xidx][yidx]
        below
    '''
    
    n = 181
    m = 361
    zzz = np.arange(n*m, dtype=float).reshape(m,n)
    for yidx in range(0, n):
        for xidx in range(0, m):
            zzz[xidx][yidx] = zz[xidx + yidx*m] 
    
    # We will need a 
    xx = np.fromiter((i-180 for i in range(m)), float)
    yy = np.fromiter((i-90  for i in range(n)), float)
    
    yyy,xxx = np.meshgrid(yy,xx)
    
    lvl = []
    for ilvl in range(5, 120, 5):
        lvl.append(ilvl)
        
    # Plot
    fig = plt.figure()
    ax = plt.axes(projection=ccrs.PlateCarree())
    
    CS = plt.contour(xxx, yyy, zzz, levels=lvl, cmap=plt.cm.nipy_spectral)
    
    plt.setp(CS.collections, linewidth=0.75)
    
    ax.set_extent([-180, 180, -90, 90], crs=ccrs.PlateCarree())
     
    plt.clabel(CS, fmt='%d', inline=1, fontsize=3)
    
    ax.coastlines(linewidth=0.3)
    
    gl = ax.gridlines(linewidth=0.3, draw_labels=True)
    gl.xlabel_style = {'size': 3.5}
    gl.ylabel_style = {'size': 3.5}
    
    gl.xlocator = mticker.FixedLocator(a_xtick)
    gl.ylocator = mticker.FixedLocator(a_ytick)
    
    gl.xformatter = LONGITUDE_FORMATTER
    gl.yformatter = LATITUDE_FORMATTER
    
    # Save the figures
    outfile = os.path.join(asvgfilepath, 'Fig_a_' + season + '_' + time_lt + '.svg')
    print('    Saving Figure %s' % outfile)
    plt.savefig(outfile, bbox_inches='tight')

    outfile = os.path.join(apngfilepath, 'Fig_a_' + season + '_' + time_lt + '.png')
    print('    Saving Figure %s' % outfile)
    plt.savefig(outfile, dpi=300, bbox_inches='tight')
    
    outfile = os.path.join(apdffilepath, 'Fig_a_' +  season + '_' + time_lt + '.pdf')
    print('    Saving Figure %s' % outfile)
    plt.savefig(outfile, dpi=300, bbox_inches='tight')

    fig.clear()
    plt.close(fig)    

    return


#%% Makebfigs #################################################################
def Makebfigs(cdbfile):
    # Process the b) file data into figures
    print('\nCreating b figure for %s' % cdbfile)
    
    season, time_lt = parseFilename(cdbfile)
    
    
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
       
    # Plot
    fig = plt.figure()
    ax = plt.gca()
    
    ax.set_prop_cycle

    # Ignore Fam5    
    for pidx in range(1, 11):
            strlabel = '%2d' % (pidx*10)
            ax.plot(x, z[pidx], label=strlabel, lw=0.5)
        
    ax.set_xscale('log')
    
    ax.yaxis.set_major_locator(MultipleLocator(20))
    ax.yaxis.set_minor_locator(MultipleLocator(5))
    
    for ymaj in ax.yaxis.get_majorticklocs():
      ax.axhline(y=ymaj, lw=0.5, color='k')
    for ymin in ax.yaxis.get_minorticklocs():
      ax.axhline(y=ymin, lw=0.1, color='k')
      
    plt.ylim(-20, 180)
    plt.xlim(0.01, 30.0)
      
    ax.grid(which='major', axis='both', linewidth=0.5)
    ax.grid(which='minor', axis='both', linewidth=0.1)
    
    ax.set_xticks(bc_xtick)
    ax.set_xticklabels(bc_xlabel)
    ax.tick_params(axis='both', which='both', labelsize=6, grid_color='k')
    
    ax.legend(title='$F_{am}$ 1 MHz (dB)', title_fontsize=6, fontsize=6, framealpha=1.0)
    
    plt.xlabel('Frequency (MHz)', fontsize=6)
    plt.ylabel('$F_{am}$ (dB above $kT_0B$)', fontsize=6)
    
    # Save the figures
    outfile = os.path.join(bsvgfilepath, 'Fig_b_' + season + '_' + time_lt + '.svg')
    print('    Saving Figure %s' % outfile)
    plt.savefig(outfile, bbox_inches='tight')
    
    outfile = os.path.join(bpngfilepath, 'Fig_b_' + season + '_' + time_lt + '.png')
    print('    Saving Figure %s' % outfile)
    plt.savefig(outfile, dpi=300, bbox_inches='tight')
    
    outfile = os.path.join(bpdffilepath, 'Fig_b_' + season + '_' + time_lt + '.pdf')
    print('    Saving Figure %s' % outfile)
    plt.savefig(outfile, dpi=300, bbox_inches='tight')
    
    fig.clear()
    plt.close(fig) 
    
    return


#%% Makecfigs #################################################################
def Makecfigs(cdcfile):
    # Process the c) file data into figures
    print('\nCreating c figure for %s' % cdcfile)
    
    season, time_lt = parseFilename(cdcfile)
    
    # Pandas data frame for contour data
    dfcd = pd.read_csv(cdcfile)
    
    z = []
    # Create lists from the data
    z.append(dfcd['DuA'].tolist())
    z.append(dfcd['DlA'].tolist())
    z.append(dfcd['sigmaFaA'].tolist())
    z.append(dfcd['sigmaDuA'].tolist())
    z.append(dfcd['sigmaDlA'].tolist())
    #z.append(dfcd['V_d'].tolist())
    #z.append(dfcd['sigma_V_d'].tolist())
    
    x = dfcd['freq'].tolist()
    
    # Plot
    fig = plt.figure()
    ax = plt.gca()
    
    ax.set_prop_cycle
    
    #strlabel = ['$D_u$', '$D_l$', '$\sigma_{F_{am}}$', '$\sigma_{D_u}$', '$\sigma_{D_l}$', '$V_d$', '$\sigma_{V_d}$']
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
      ax.axhline(y=ymaj, lw=0.5, color='k')
    for ymin in ax.yaxis.get_minorticklocs():
      ax.axhline(y=ymin, lw=0.1, color='k')
      
    ax.grid(which='major', axis='both', linewidth=0.5)
    ax.grid(which='minor', axis='both', linewidth=0.1)
    
    ax.set_xticks(bc_xtick)
    ax.set_xticklabels(bc_xlabel)
    ax.tick_params(axis='both', which='both', labelsize=6, grid_color='k')
    
    # Set the Figure upper limit roughly 2 more than the max DuA
    cmax = (ceil(max(z[0]))-ceil(max(z[0]))%2)+2
    plt.ylim(0, cmax)
    plt.xlim(0.01, 20.0)
    
    ax.legend(title='$F_{am}$\nStatistics', title_fontsize=5, fontsize=5, framealpha=1.0)
    
    plt.xlabel('Frequency (MHz)', fontsize=6)
    plt.ylabel('(dB)', fontsize=6)

    # Save the figures
    outfile = os.path.join(csvgfilepath, 'Fig_c_' + season + '_' + time_lt + '.svg')
    print('    Saving Figure %s' % outfile)
    plt.savefig(outfile, bbox_inches='tight')

    outfile = os.path.join(cpngfilepath, 'Fig_c_' + season + '_' + time_lt + '.png')
    print('    Saving Figure %s' % outfile)
    plt.savefig(outfile, dpi=300, bbox_inches='tight')
    
    outfile = os.path.join(cpdffilepath, 'Fig_c_' + season + '_' + time_lt + '.pdf')
    print('    Saving Figure %s' % outfile)
    plt.savefig(outfile, dpi=300, bbox_inches='tight')

    fig.clear()
    plt.close(fig)
    return


#%% openOutputDirectories #####################################################
def openOutputDirectories():
    # If the output directories already exist, delete them, then recreate them.
    
    if os.path.isdir(asvgfilepath):
        shutil.rmtree(asvgfilepath)
    os.makedirs(asvgfilepath)
    
    if os.path.isdir(bsvgfilepath):
        shutil.rmtree(bsvgfilepath)
    os.makedirs(bsvgfilepath)
    
    if os.path.isdir(csvgfilepath):
        shutil.rmtree(csvgfilepath)
    os.makedirs(csvgfilepath)
        
    if os.path.isdir(apngfilepath):
        shutil.rmtree(apngfilepath)
    os.makedirs(apngfilepath)
    
    if os.path.isdir(bpngfilepath):
        shutil.rmtree(bpngfilepath)
    os.makedirs(bpngfilepath)
    
    if os.path.isdir(cpngfilepath):
        shutil.rmtree(cpngfilepath)
    os.makedirs(cpngfilepath)
     
    if os.path.isdir(apdffilepath):
        shutil.rmtree(apdffilepath)
    os.makedirs(apdffilepath)
    
    if os.path.isdir(bpdffilepath):
        shutil.rmtree(bpdffilepath)
    os.makedirs(bpdffilepath)
    
    if os.path.isdir(cpdffilepath):
        shutil.rmtree(cpdffilepath)
    os.mkdir(cpdffilepath)
    
    return


#%% MakeP372figs ##############################################################
def MakeP372figs():
        
    if os.path.isdir(acsvfilepath) and \
       os.path.isdir(bcsvfilepath) and \
       os.path.isdir(ccsvfilepath):
           
        # Create the output directories  
        openOutputDirectories()
        
        # Make the a figures
        afiles = glob.glob(acsvfilepath + os.path.sep + '*.csv')
        for file in afiles:
            Makeafigs(file)
        
        # Make the b figures
        bfiles = glob.glob(bcsvfilepath + os.path.sep + '*.csv')
        for file in bfiles:
            Makebfigs(file)
          
        # Make the c figures
        cfiles = glob.glob(ccsvfilepath + os.path.sep + '*.csv')
        for file in cfiles:
            Makecfigs(file)
            
    else:
        print('\nError: one or more expected input directories do not exist.')
        print('acsvfilepath: ' + acsvfilepath)
        print('bcsvfilepath: ' + bcsvfilepath)
        print('ccsvfilepath: ' + ccsvfilepath)
        print('\nRun ITURNoise.exe in Mode 2 to produce atmospheric figure data.')
        print('From the command prompt, run: ITURNoise.exe "path' + os.path.sep + 'to' + os.path.sep + 'CCIV' + os.path.sep + 'data"')
        print('Exiting...')
        
    return


#%% Run MakeP327figs ##########################################################
if __name__ == "__main__":
    print("\n****************************************")
    print("********** BEGIN MakeP732figs **********")
    print("****************************************")
    MakeP372figs()
    print("\n****************************************")
    print("*********** END MakeP732figs ***********")
    print("****************************************")
