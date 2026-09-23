#include <stdlib.h>

// Local includes
#include "Common.h"
#include "Noise.h"

#include <stdlib.h>
#include <stddef.h>

int AllocateNoiseMemory(
    struct NoiseParams* noiseP
) {
    /*
    Allocate the memory necessary for the noiseP structure.
    The data must be read into these structures elsewhere.

        INPUT
            struct NoiseParams *noiseP

        OUTPUT
            noiseP->fakp
            noiseP->fakabp
            noiseP->fam
            noiseP->dud

        SUBROUTINES
            None
     */

    int m, n;

    // Every array is created with calloc() so that the child pointers are NULL
    // until they are successfully allocated. That allows FreeNoiseMemory() to
    // release a partially built structure if any allocation below fails.
    noiseP->fakp = NULL;
    noiseP->fakabp = NULL;
    noiseP->fam = NULL;
    noiseP->dud = NULL;

    // Create the fakp array.
    noiseP->fakp = (double***)calloc(6, sizeof(double**));
    if (noiseP->fakp == NULL) {
        FreeNoiseMemory(noiseP);
        return RTN_ERRALLOCATEFAKP;
    }
    for (n = 0; n < 6; n++) {
        noiseP->fakp[n] = (double**)calloc(16, sizeof(double*));
        if (noiseP->fakp[n] == NULL) {
            FreeNoiseMemory(noiseP);
            return RTN_ERRALLOCATEFAKP;
        }
        for (m = 0; m < 16; m++) {
            noiseP->fakp[n][m] = (double*)calloc(29, sizeof(double));
            if (noiseP->fakp[n][m] == NULL) {
                FreeNoiseMemory(noiseP);
                return RTN_ERRALLOCATEFAKP;
            }
        }
    }

    // Create the fakabp array.
    noiseP->fakabp = (double**)calloc(6, sizeof(double*));
    if (noiseP->fakabp == NULL) {
        FreeNoiseMemory(noiseP);
        return RTN_ERRALLOCATEFAKABP;
    }
    for (m = 0; m < 6; m++) {
        noiseP->fakabp[m] = (double*)calloc(2, sizeof(double));
        if (noiseP->fakabp[m] == NULL) {
            FreeNoiseMemory(noiseP);
            return RTN_ERRALLOCATEFAKABP;
        }
    }

    // Create the dud array.
    noiseP->dud = (double***)calloc(5, sizeof(double**));
    if (noiseP->dud == NULL) {
        FreeNoiseMemory(noiseP);
        return RTN_ERRALLOCATEDUD;
    }
    for (n = 0; n < 5; n++) {
        noiseP->dud[n] = (double**)calloc(12, sizeof(double*));
        if (noiseP->dud[n] == NULL) {
            FreeNoiseMemory(noiseP);
            return RTN_ERRALLOCATEDUD;
        }
        for (m = 0; m < 12; m++) {
            noiseP->dud[n][m] = (double*)calloc(5, sizeof(double));
            if (noiseP->dud[n][m] == NULL) {
                FreeNoiseMemory(noiseP);
                return RTN_ERRALLOCATEDUD;
            }
        }
    }

    // Create the fam array.
    noiseP->fam = (double**)calloc(12, sizeof(double*));
    if (noiseP->fam == NULL) {
        FreeNoiseMemory(noiseP);
        return RTN_ERRALLOCATEFAM;
    }
    for (m = 0; m < 12; m++) {
        noiseP->fam[m] = (double*)calloc(14, sizeof(double));
        if (noiseP->fam[m] == NULL) {
            FreeNoiseMemory(noiseP);
            return RTN_ERRALLOCATEFAM;
        }
    }

    return RTN_ALLOCATEP372OK;
}

int FreeNoiseMemory(
    struct NoiseParams *noiseP
) {
    /*
    Free the memory that was dynamically (m) allocated for the structure 
    NoiseParams noiseP.

        INPUT
            struct NoiseParams *noiseP

        OUTPUT
            void

         SUBROUTINES
             None
       */

    int m, n;

    // Each pointer is checked before it is dereferenced so that this routine can
    // also clean up a structure that AllocateNoiseMemory() only partially built.
    // Freed pointers are set to NULL so that a second call is harmless.

    // Free DUD
    if (noiseP->dud != NULL) {
        for (n = 0; n < 5; n++) {
            if (noiseP->dud[n] == NULL) continue;
            for (m = 0; m < 12; m++) {
                free(noiseP->dud[n][m]);
            }
            free(noiseP->dud[n]);
        }
        free(noiseP->dud);
        noiseP->dud = NULL;
    }

    // Free FAM
    if (noiseP->fam != NULL) {
        for (m = 0; m < 12; m++) {
            free(noiseP->fam[m]);
        }
        free(noiseP->fam);
        noiseP->fam = NULL;
    }

    // Free FAKP
    if (noiseP->fakp != NULL) {
        for (n = 0; n < 6; n++) {
            if (noiseP->fakp[n] == NULL) continue;
            for (m = 0; m < 16; m++) {
                free(noiseP->fakp[n][m]);
            }
            free(noiseP->fakp[n]);
        }
        free(noiseP->fakp);
        noiseP->fakp = NULL;
    }

    // Free fakabp
    if (noiseP->fakabp != NULL) {
        for (m = 0; m < 6; m++) {
            free(noiseP->fakabp[m]);
        }
        free(noiseP->fakabp);
        noiseP->fakabp = NULL;
    }

    return RTN_NOISEFREED;
}
