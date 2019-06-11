#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Local includes
#include "Common.h"
#include "P533.h"
#include "ITURHFProp.h"
// End local includes


int ValidateITURHFP(struct ITURHFProp ITURHFP) {
	
	if((ITURHFP.TXBearing > 2.0*PI) || (ITURHFP.TXBearing < 0.0))							return RTN_ERRTXBEARING;
	if((ITURHFP.RXBearing > 2.0*PI) || (ITURHFP.RXBearing < 0.0))							return RTN_ERRRXBEARING;
	if((ITURHFP.TXGOS < TINYDB) || (ITURHFP.TXGOS > 60.0))									return RTN_ERRTXGOS;
	if((ITURHFP.RXGOS < TINYDB) || (ITURHFP.RXGOS > 60.0))									return RTN_ERRRXGOS;

	if((ITURHFP.L_LL.lat < -PI/2.0) || (ITURHFP.L_LL.lat > PI/2.0))							return RTN_ERRLLLAT; 
	if((ITURHFP.L_LR.lat < -PI/2.0) || (ITURHFP.L_LR.lat > PI/2.0))							return RTN_ERRLRLAT; 
	if((ITURHFP.L_UL.lat < -PI/2.0) || (ITURHFP.L_UL.lat > PI/2.0))							return RTN_ERRULLAT; 
	if((ITURHFP.L_UR.lat < -PI/2.0) || (ITURHFP.L_UR.lat > PI/2.0))							return RTN_ERRURLAT; 

	if((ITURHFP.L_LL.lng < -PI) || (ITURHFP.L_LL.lng > PI))									return RTN_ERRLLLNG; 
	if((ITURHFP.L_LR.lng < -PI) || (ITURHFP.L_LR.lng > PI))									return RTN_ERRLRLNG; 
	if((ITURHFP.L_UL.lng < -PI) || (ITURHFP.L_UL.lng > PI))									return RTN_ERRULLNG; 
	if((ITURHFP.L_UR.lng < -PI) || (ITURHFP.L_UR.lng > PI))									return RTN_ERRURLNG; 

	// Lower/upper checks
	if((ITURHFP.L_LL.lat > ITURHFP.L_UL.lat) || (ITURHFP.L_LL.lat > ITURHFP.L_UR.lat))			return RTN_ERRLL;
	if((ITURHFP.L_LR.lat > ITURHFP.L_UL.lat) || (ITURHFP.L_LR.lat > ITURHFP.L_UR.lat))			return RTN_ERRLR;
	if((ITURHFP.L_UL.lat < ITURHFP.L_LL.lat) || (ITURHFP.L_UL.lat < ITURHFP.L_LR.lat))			return RTN_ERRUL;
	if((ITURHFP.L_UR.lat < ITURHFP.L_LL.lat) || (ITURHFP.L_UR.lat < ITURHFP.L_LR.lat))			return RTN_ERRUR;

	// Left/right checks
	if((ITURHFP.L_LL.lng > ITURHFP.L_LR.lng) || (ITURHFP.L_LL.lng > ITURHFP.L_UR.lng))			return RTN_ERRLL;
	if((ITURHFP.L_LR.lng < ITURHFP.L_UL.lng) || (ITURHFP.L_LR.lng < ITURHFP.L_LL.lng))			return RTN_ERRLR;
	if((ITURHFP.L_UL.lng > ITURHFP.L_LR.lng) || (ITURHFP.L_UL.lng > ITURHFP.L_UR.lng))			return RTN_ERRUL;
	if((ITURHFP.L_UR.lng < ITURHFP.L_UL.lng) || (ITURHFP.L_UR.lng < ITURHFP.L_LL.lng))			return RTN_ERRUR;

	// Make sure the user is asking for a box.
	if(ITURHFP.L_LL.lat != ITURHFP.L_LR.lat)													return RTN_ERRLLAT;
	if(ITURHFP.L_UL.lat != ITURHFP.L_UR.lat)													return RTN_ERRULAT;
	if(ITURHFP.L_LL.lng != ITURHFP.L_UL.lng)													return RTN_ERRLLNG;
	if(ITURHFP.L_LR.lng != ITURHFP.L_UR.lng)													return RTN_ERRRLNG;

	if ((ITURHFP.AntennaOrientation != MANUAL) && (ITURHFP.AntennaOrientation != TX2RX))		return RTN_ERRANTENNAORN;

	return RTN_VALIDATEITURHFPOK;
};

//////////////////////////////////////////////////////////////////////////////
//      Copyright  International Telecommunication Union (ITU) 2019         //
//                     All rights reserved.                                 //
// No part of this publication may be reproduced, by any means whatsoever,  //
//              without written permission of ITU                           //
//////////////////////////////////////////////////////////////////////////////


