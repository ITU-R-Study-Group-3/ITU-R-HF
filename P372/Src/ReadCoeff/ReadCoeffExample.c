#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Local include
#include "ReadCoeff.h"
// End local include 
 
void main() {

		struct IonoCoeff Coeff;

		long What2Read;

		// Clear the coeff structure
		NullCoeffArrays(&Coeff);
		
		What2Read = GETXF2 | GETXFM3 | GETXE | GETXESU | GETXES | GETXESL | 
							 GETXHPO1 | GETXHPO2 | GETXHP | GETFAKP | GETFAKABP | GETDUD |
							 GETFAM | GETSYS1 | GETSYS2 | GETPERR | GETF2D | GETPKO | GETSLP | GETCCR;
		
		ReadCoeff(&Coeff, 4, What2Read);

		// Do something with this wonderful data.

		FreeCoeffArrays(&Coeff);

};
