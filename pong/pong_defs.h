#pragma once
#ifndef __PONG_DEFS_H__
#define __PONG_DEFS_H__

#define RAW_KEY_ESCAPE       (0x45)
#define RAW_KEY_SPACE        (0x40)
#define RAW_KEY_CURSOR_UP    (0x4c)
#define RAW_KEY_CURSOR_DOWN  (0x4d)
#define RAW_KEY_CURSOR_RIGHT (0x4e)
#define RAW_KEY_CURSOR_LEFT  (0x4f)
#define RAW_KEY_W            (0x11)
#define RAW_KEY_S            (0x21)

#define DIWSTRT_VALUE      0x2c81
#define DIWSTOP_VALUE_PAL  0x2cc1
#define DIWSTOP_VALUE_NTSC 0xf4c1

// Data fetch
#define DDFSTRT_VALUE      0x0038
#define DDFSTOP_VALUE      0x00d0

// Display dimensions and data size
#define DISPLAY_WIDTH    (320)
#define DISPLAY_HEIGHT   (256)
#define DISPLAY_ROW_BYTES (DISPLAY_WIDTH / 8)

#define IMG_FILENAME_PAL "graphics/bg320x256.ts"
#define IMG_FILENAME_NTSC "graphics/bg320x200.ts"

// playfield control
// single playfield, 1 bitplanes (2 colors)
#define BPLCON0_VALUE (0x1200)
// We have single playfield, so priority is determined in bits
// 5-3 and we need to set the playfield 2 priority bit (bit 6)
#define BPLCON2_VALUE (0x0048)

// copper instruction macros
#define COP_MOVE(addr, data) addr, data
#define COP_WAIT_END  0xffff, 0xfffe

// copper list indexes
#define COPLIST_IDX_SPR0_PTH_VALUE (3)
#define COPLIST_IDX_DIWSTOP_VALUE (COPLIST_IDX_SPR0_PTH_VALUE + 32 + 6)
#define COPLIST_IDX_BPL1MOD_VALUE (COPLIST_IDX_DIWSTOP_VALUE + 6)
#define COPLIST_IDX_BPL2MOD_VALUE (COPLIST_IDX_BPL1MOD_VALUE + 2)
#define COPLIST_IDX_COLOR00_VALUE (COPLIST_IDX_BPL2MOD_VALUE + 2)
#define COPLIST_IDX_BPL1PTH_VALUE (COPLIST_IDX_COLOR00_VALUE + 64)

extern struct ExecBase *SysBase;
extern struct GfxBase *GfxBase;
extern struct Custom custom;

extern UWORD __chip NULL_SPRITE_DATA[];
extern UWORD __chip paddle_l_data[], paddle_r_data[], ball_data[];
extern UWORD digit_data[];

#define PADDLE_SPEED       (2)
#define PADDLE_HEIGHT      (20)
#define PADDLE_INIT_Y      (160)
#define PADDLE_L_INIT_X    (120)
#define PADDLE_R_INIT_X    (440)
#define PADDLE_MIN_Y       (44)
#define PADDLE_MAX_Y_NTSC  (PADDLE_MIN_Y + 200 - PADDLE_HEIGHT)
#define PADDLE_MAX_Y_PAL   (PADDLE_MIN_Y + 256 - PADDLE_HEIGHT)

#define BALL_INIT_X        (300)
#define BALL_INIT_Y        (160)
#define BALL_SIZE          (4)
#define BALL_START_X_SPEED (2)
#define BALL_START_Y_SPEED (0)
#define BALL_MIN_X         (110)
#define BALL_MAX_X         (450)
#define BALL_MIN_Y         (44)
#define BALL_MAX_Y_NTSC    (BALL_MIN_Y + 200 - BALL_SIZE)
#define BALL_MAX_Y_PAL     (BALL_MIN_Y + 256 - BALL_SIZE)

#define DIGIT_L0_X         (240)
#define DIGIT_L1_X         (224)
#define DIGIT_R0_X         (380)
#define DIGIT_R1_X         (364)
#define DIGIT_Y            (60)
#define DIGIT_HEIGHT       (16)
#define DIGIT_SPRITE_WORDS (36)

#define BEEP_PATH "sounds/beep.raw8"
#define BUZZ_PATH "sounds/buzz.raw8"
#define SAMPLE_PERIOD_NTSC (162)
#define SAMPLE_PERIOD_PAL  (161)
#define BEEP_BYTES (1818)
#define BUZZ_BYTES (8938)

#endif /* __PONG_DEFS_H__ */
