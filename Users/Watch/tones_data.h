#ifndef TONES_DATA_H_
#define TONES_DATA_H_

#include "typedefs.h"

enum
{
    LA = 262,
    LB = 294,
    LC = 330,
    LD = 349,
    LE = 392,
    LF = 440,
    LG = 494,

    MA = 523,
    MB = 578,
    MC = 659,
    MD = 698,
    ME = 784,
    MF = 880,
    MG = 988,

    HA = 1064,
    HB = 1175,
    HC = 1318,
    HD = 1397,
    HE = 1568,
    HF = 1760,
    HG = 1976
};

extern const tune_t STAY[];
extern const tune_t TUNE[];

#endif /* TONES_DATA_H_ */