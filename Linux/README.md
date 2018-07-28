#### Introduction
This directory contains the top-level Makefile for building ITURHFProp and the associated libp533.so and libp372.so libraries on Linux systems.  The Makefile recognises the targets all, clean and install.
#### Building and Installing the Application
The application is built using the following command;
```
$ make all
```
The following command installs the libraries and application in /usr/local/lib and /usr/local/bin ;
```
$ sudo make install
$ sudo ldconfig
```

#### Data Directory
Operation of the application requires the presence a number of data files. This may be performed manually using a location of the user's choice or by using the command ```sudo make install-data``` to copy the files to /usr/local/share/p533/data.  The location of the data directory is a required parameter in input files.  e.g. If the files are copied to /usr/local/share/p533/data, input files should contain the line;
```
DataFilePath "/usr/local/share/p533/data/"
```
The following files should be copied from the P372/Data directory to the nominated data directory on the host system. 

COEFF01W.txt
COEFF02W.txt
COEFF03W.txt
COEFF04W.txt
COEFF05W.txt
COEFF06W.txt
COEFF07W.txt
COEFF08W.txt
COEFF09W.txt
COEFF10W.txt
COEFF11W.txt
COEFF12W.txt
ionos01.bin
ionos02.bin
ionos03.bin
ionos04.bin
ionos05.bin
ionos06.bin
ionos07.bin
ionos08.bin
ionos09.bin
ionos10.bin
ionos11.bin
ionos12.bin
P1239-3 Decile Factors.txt
