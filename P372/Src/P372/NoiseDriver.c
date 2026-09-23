#include <stdio.h>
#include <stdlib.h>
#include "Common.h"
#include "Noise.h"

int main() {
    struct NoiseParams noiseP;
    double manMadeNoise = RESIDENTIAL;
    double longitude = 10.0*D2R;
    double latitude = 40.0*D2R;
    double frequency_MHz = 10.0;
    int hour = 7;
    int month = 1;
    const char *dataFilePath = ".\\";

    AllocateNoiseMemory(
        &noiseP
    );
    printf("AllocateNoiseMemory() Run Successful\n");

    ReadFamDud(
        &noiseP,
        dataFilePath,
        month
    );
    printf("ReadFamDud() Run Successful\n\n");

    printf("ManMadeNoise: %f\n", manMadeNoise);
    printf("longitude: %f\n", longitude);
    printf("latitude: %f\n", latitude);
    printf("frequency: %f\n", frequency_MHz);
    printf("hour: %d\n", hour);
    printf("month: %d\n\n", month);

    Noise(
        &noiseP,
        hour,
        longitude,
        latitude,
        frequency_MHz
    );
    printf("Noise Run Successful\n\n");

    printf("FaA: %f\n", noiseP.FaA);
    printf("DuA: %f\n", noiseP.DuA);
    printf("DlA: %f\n", noiseP.DlA);
    printf("FaM: %f\n", noiseP.FaM);
    printf("DuM: %f\n", noiseP.DuM);
    printf("DlM: %f\n", noiseP.DlM);
    printf("FaG: %f\n", noiseP.FaG);
    printf("DuG: %f\n", noiseP.DuG);
    printf("DlG: %f\n", noiseP.DlG);
    printf("DuT: %f\n", noiseP.DuT);
    printf("DlT: %f\n", noiseP.DlT);
    printf("FamT: %f\n", noiseP.FamT);

    printf("ManMadeNoise: %f\n", noiseP.ManMadeNoise);

/*
for(i=0; i<6; i++) {
    for(j=0; j<16; j++) {
        for(k=0; k<29; k++) {
            printf("fakp[%d][%d][%d]: %f\n", i, j, k, noiseP.fakp[i][j][k]);
        }
    }
}

for(j=0; j<6; j++) {
    for(k=0; k<2; k++) {
        printf("fakabp[%d][%d]: %f\n", j, k, noiseP.fakabp[j][k]);
    };
};

for(i=0; i<5; i++) {
    for(j=0; j<12; j++) {
        for(k=0; k<5; k++) {
            printf("dud[%d][%d][%d]: %f\n", i, j, k, noiseP.dud[i][j][k]);
        };
    };
};

for(j=0; j<12; j++) {
    for(k=0; k<14; k++) {
        printf("fam[%d][%d]: %f\n", j, k, noiseP.fam[j][k]);
    };
};
*/

    printf("path->dud addr             %p\n", (void *)&noiseP.dud);
    printf("\tpath->dud[0][0][0] addr             %p contents %lf\n", (void *)&noiseP.dud[0][0][0], noiseP.dud[0][0][0]);
    printf("\tpath->dud[4][0][0] addr             %p contents %lf\n", (void *)&noiseP.dud[4][0][0], noiseP.dud[4][0][0]);
    printf("\tpath->dud[0][11][0] addr             %p contents %lf\n", (void *)&noiseP.dud[0][11][0], noiseP.dud[0][11][0]);
    printf("\tpath->dud[0][0][4] addr             %p contents %lf\n", (void *)&noiseP.dud[0][0][4], noiseP.dud[0][0][4]);
    printf("path->fam addr           %p\n", (void *)&noiseP.fam);
    printf("\tpath->fam[0][0] addr             %p contents %lf\n", (void *)&noiseP.fam[0][0], noiseP.fam[0][0]);
    printf("\tpath->fam[11][0] addr             %p contents %lf\n", (void *)&noiseP.fam[11][0], noiseP.fam[11][0]);
    printf("\tpath->fam[0][13] addr             %p contents %lf\n", (void *)&noiseP.fam[0][13], noiseP.fam[0][13]);
    printf("path->fakp addr           %p\n", (void *)&noiseP.fakp);
    printf("\tpath->fakp[0][0][0] addr             %p contents %lf\n", (void *)&noiseP.fakp[0][0][0], noiseP.fakp[0][0][0]);
    printf("\tpath->fakp[5][0][0] addr             %p contents %lf\n", (void *)&noiseP.fakp[5][0][0], noiseP.fakp[5][0][0]);
    printf("\tpath->fakp[0][15][0] addr             %p contents %lf\n", (void *)&noiseP.fakp[0][15][0], noiseP.fakp[0][15][0]);
    printf("\tpath->fakp[0][0][28] addr             %p contents %lf\n", (void *)&noiseP.fakp[0][0][28], noiseP.fakp[0][0][28]);
    printf("path->fakabp addr           %p\n", (void *)&noiseP.fakabp);
    printf("\tpath->fakabp[0][0] addr             %p contents %lf\n", (void *)&noiseP.fakabp[0][0], noiseP.fakabp[0][0]);
    printf("\tpath->fakabp[5][0] addr             %p contents %lf\n", (void *)&noiseP.fakabp[5][0], noiseP.fakabp[5][0]);
    printf("\tpath->fakabp[0][1] addr             %p contents %lf\n", (void *)&noiseP.fakabp[0][1], noiseP.fakabp[0][1]);

  return 0;
}
