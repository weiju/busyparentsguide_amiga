#include <exec/types.h>
#include "pong_defs.h"

// null sprite data for sprites that are supposed to be inactive
UWORD __chip NULL_SPRITE_DATA[] = {
    0x0000, 0x0000,
    0x0000, 0x0000
};

UWORD __chip paddle_l_data[] = {
  0x0014, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x0000, 0x0000
};

UWORD __chip paddle_r_data[] = {
  0x0014, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x07e0, 0x0000,
  0x0000, 0x0000
};

UWORD __chip ball_data[] = {
  0x0004, 0x0000,
  0xf000, 0x0000,
  0xf000, 0x0000,
  0xf000, 0x0000,
  0xf000, 0x0000,
  0x0000, 0x0000
};

// serves as the data source for the digit sprites
// The code was generated from ratr0-maketiles --generatec
// just as the other sprites, but with the structure words
// (first and last line) taken out
UWORD digit_data[] = {
    //0x00a0, 0x0000,
  0x03ff, 0x0000,
  0x03ff, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x03ff, 0x0000,
  0x03ff, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x03ff, 0x0000,
  0x03ff, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x03ff, 0x0000,
  0x03ff, 0x0000,
  0x0300, 0x0000,
  0x0300, 0x0000,
  0x0300, 0x0000,
  0x0300, 0x0000,
  0x0300, 0x0000,
  0x03ff, 0x0000,
  0x03ff, 0x0000,
  0x03ff, 0x0000,
  0x03ff, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x03ff, 0x0000,
  0x03ff, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x03ff, 0x0000,
  0x03ff, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x03ff, 0x0000,
  0x03ff, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x03ff, 0x0000,
  0x03ff, 0x0000,
  0x0300, 0x0000,
  0x0300, 0x0000,
  0x0300, 0x0000,
  0x0300, 0x0000,
  0x0300, 0x0000,
  0x03ff, 0x0000,
  0x03ff, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x03ff, 0x0000,
  0x03ff, 0x0000,
  0x03ff, 0x0000,
  0x03ff, 0x0000,
  0x0300, 0x0000,
  0x0300, 0x0000,
  0x0300, 0x0000,
  0x0300, 0x0000,
  0x0300, 0x0000,
  0x03ff, 0x0000,
  0x03ff, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x03ff, 0x0000,
  0x03ff, 0x0000,
  0x03ff, 0x0000,
  0x03ff, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x03ff, 0x0000,
  0x03ff, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x03ff, 0x0000,
  0x03ff, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x03ff, 0x0000,
  0x03ff, 0x0000,
  0x03ff, 0x0000,
  0x03ff, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x0303, 0x0000,
  0x03ff, 0x0000,
  0x03ff, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x0003, 0x0000,
  0x03ff, 0x0000,
  0x03ff, 0x0000,
  //0x0000, 0x0000
};