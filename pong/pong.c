/**
 * pong.c - Classic Pong implementation on the Amiga
 */
#include <stdio.h>
#include <string.h>
#include <hardware/custom.h>
#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/graphics_protos.h>
#include <clib/alib_protos.h>

#include <devices/keyboard.h>
#include <devices/input.h>

#include <exec/execbase.h>
#include <graphics/gfxbase.h>
#include <ahpc_registers.h>

#include <SDI/SDI_compiler.h>
#include "../ptplayer/ptplayer.h"

#include <tilesheet.h>

#include "pong_defs.h"

static UWORD __chip coplist[] = {
    COP_MOVE(FMODE,   0), // set fetch mode = 0

    // sprites first
    COP_MOVE(SPR0PTH, 0), COP_MOVE(SPR0PTL, 0),
    COP_MOVE(SPR1PTH, 0), COP_MOVE(SPR1PTL, 0),
    COP_MOVE(SPR2PTH, 0), COP_MOVE(SPR2PTL, 0),
    COP_MOVE(SPR3PTH, 0), COP_MOVE(SPR3PTL, 0),
    COP_MOVE(SPR4PTH, 0), COP_MOVE(SPR4PTL, 0),
    COP_MOVE(SPR5PTH, 0), COP_MOVE(SPR5PTL, 0),
    COP_MOVE(SPR6PTH, 0), COP_MOVE(SPR6PTL, 0),
    COP_MOVE(SPR7PTH, 0), COP_MOVE(SPR7PTL, 0),

    COP_MOVE(DDFSTRT, DDFSTRT_VALUE),
    COP_MOVE(DDFSTOP, DDFSTOP_VALUE),
    COP_MOVE(DIWSTRT, DIWSTRT_VALUE),
    COP_MOVE(DIWSTOP, DIWSTOP_VALUE_PAL),
    COP_MOVE(BPLCON0, BPLCON0_VALUE),
    COP_MOVE(BPLCON2, BPLCON2_VALUE),
    COP_MOVE(BPL1MOD, 0),
    COP_MOVE(BPL2MOD, 0),

    // set up the display colors
    COP_MOVE(COLOR00, 0x000), COP_MOVE(COLOR01, 0xfff),
    COP_MOVE(COLOR02, 0xfff), COP_MOVE(COLOR03, 0xfff),
    COP_MOVE(COLOR04, 0xfff), COP_MOVE(COLOR05, 0xfff),
    COP_MOVE(COLOR06, 0xfff), COP_MOVE(COLOR07, 0xfff),
    COP_MOVE(COLOR08, 0xfff), COP_MOVE(COLOR09, 0xfff),
    COP_MOVE(COLOR10, 0xfff), COP_MOVE(COLOR11, 0xfff),
    COP_MOVE(COLOR12, 0xfff), COP_MOVE(COLOR13, 0xfff),
    COP_MOVE(COLOR14, 0xfff), COP_MOVE(COLOR15, 0xfff),
    COP_MOVE(COLOR16, 0xfff), COP_MOVE(COLOR17, 0xfff),
    COP_MOVE(COLOR18, 0xfff), COP_MOVE(COLOR19, 0xfff),
    COP_MOVE(COLOR20, 0xfff), COP_MOVE(COLOR21, 0xfff),
    COP_MOVE(COLOR22, 0xfff), COP_MOVE(COLOR23, 0xfff),
    COP_MOVE(COLOR24, 0xfff), COP_MOVE(COLOR25, 0xfff),
    COP_MOVE(COLOR26, 0xfff), COP_MOVE(COLOR27, 0xfff),
    COP_MOVE(COLOR28, 0xfff), COP_MOVE(COLOR29, 0xfff),
    COP_MOVE(COLOR30, 0xfff), COP_MOVE(COLOR31, 0xfff),

    COP_MOVE(BPL1PTH, 0), COP_MOVE(BPL1PTL, 0),

    COP_WAIT_END,
    COP_WAIT_END
};

static volatile ULONG *custom_vposr = (volatile ULONG *) 0xdff004;
static volatile UWORD *custom_clxdat = (volatile UWORD *) 0xdff00e;

// Wait for this position for vertical blank
// translated from http://eab.abime.net/showthread.php?t=51928
static vb_waitpos;

static void wait_vblank()
{
    while (((*custom_vposr) & 0x1ff00) != (vb_waitpos<<8)) ;
}

static BOOL init_display(void)
{
    LoadView(NULL);  // clear display, reset hardware registers
    WaitTOF();       // 2 WaitTOFs to wait for 1. long frame and
    WaitTOF();       // 2. short frame copper lists to finish (if interlaced)
    return (((struct GfxBase *) GfxBase)->DisplayFlags & PAL) == PAL;
}

static void reset_display(void)
{
    LoadView(((struct GfxBase *) GfxBase)->ActiView);
    WaitTOF();
    WaitTOF();
    custom.cop1lc = (ULONG) ((struct GfxBase *) GfxBase)->copinit;
    RethinkDisplay();
}

static struct Ratr0TileSheet image;

// To handle input
static struct MsgPort *input_mp;
static struct IOStdReq *input_io;
static struct Interrupt handler_info;


// *******************************
// Global game state

// Game objects
struct Paddle {
    UWORD x, y;
    UWORD __chip *data;
};

struct Ball {
    UWORD x, y;
    WORD speed_x, speed_y;
    UWORD __chip *data;
};

struct PlayerScore {
    UWORD score;
    UWORD digit0_data[DIGIT_SPRITE_WORDS], digit1_data[DIGIT_SPRITE_WORDS];
};

struct Paddle paddle_l = { PADDLE_L_INIT_X, PADDLE_INIT_Y, paddle_l_data },
    paddle_r = { PADDLE_R_INIT_X, PADDLE_INIT_Y, paddle_r_data };

struct Ball ball = { BALL_INIT_X, BALL_INIT_Y, BALL_START_X_SPEED, BALL_START_Y_SPEED, ball_data};
// maximum y-positions (PAL/NTSC)
static UWORD paddle_max_y, ball_max_y;

// Player score
struct PlayerScore __chip player1, player2;
// *******************************

// To handle input
static BYTE error;
static struct MsgPort *kb_mp;
static struct IOStdReq *kb_io;
static UBYTE *kb_matrix;
#define MATRIX_SIZE (16L)

static int init_keyboard_device(void)
{
    kb_mp = CreatePort(0, 0);
    kb_io = (struct IOStdReq *) CreateExtIO(kb_mp, sizeof(struct IOStdReq));
    error = OpenDevice("keyboard.device", 0L, (struct IORequest *) kb_io, 0);
    kb_matrix = AllocMem(MATRIX_SIZE, MEMF_PUBLIC|MEMF_CLEAR);
    return 1;
}

static void cleanup_keyboard_device(void)
{
    if (kb_matrix) FreeMem(kb_matrix, MATRIX_SIZE);
    if (kb_io) {
        CloseDevice((struct IORequest *) kb_io);
        DeleteExtIO((struct IORequest *) kb_io);
    }
    if (kb_mp) DeletePort(kb_mp);
}

static void read_keyboard(void)
{
    kb_io->io_Command = KBD_READMATRIX;
    kb_io->io_Data = (APTR) kb_matrix;
    kb_io->io_Length = SysBase->LibNode.lib_Version >= 36 ? MATRIX_SIZE : 13;
    DoIO((struct IORequest *) kb_io);
}

static int is_keydown(BYTE keycode) {
    return kb_matrix[keycode / 8] & (1 << (keycode % 8));
}

static void set_sprite_pos(UWORD *sprite_data, UWORD hstart, UWORD vstart, UWORD vstop)
{
    sprite_data[0] = ((vstart & 0xff) << 8) | ((hstart >> 1) & 0xff);
    // vstop + high bit of vstart + low bit of hstart
    sprite_data[1] = ((vstop & 0xff) << 8) |  // vstop 8 low bits
        ((vstart >> 8) & 1) << 2 |  // vstart high bit
        ((vstop >> 8) & 1) << 1 |   // vstop high bit
        (hstart & 1) |              // hstart low bit
        sprite_data[1] & 0x80;      // preserve attach bit
}

static void cleanup(void)
{
    cleanup_keyboard_device();
    ratr0_free_tilesheet_data(&image);
    reset_display();
    mt_remove_cia(&custom);
}

/**
 * Copies the data for a digit to the destination sprite.
 */
static void set_digit(UWORD *sprite, int digit)
{
    memcpy(&sprite[2], &digit_data[32 * digit], 4 * 16);
}

/**
 * Increments the player's score and updates the display.
 */
static void increment_score(struct PlayerScore *player)
{
    player->score++;
    set_digit(player->digit0_data, player->score % 10);
    set_digit(player->digit1_data, player->score / 10);
}

void reset_ball(BOOL player1_adv)
{
    ball.x = BALL_INIT_X;
    ball.y = BALL_INIT_Y;
    ball.speed_x = player1_adv ? BALL_START_X_SPEED: -BALL_START_X_SPEED;
    ball.speed_y = BALL_START_Y_SPEED;
}

void update_paddle_and_ball_positions(void)
{
    set_sprite_pos(paddle_l.data, paddle_l.x, paddle_l.y, paddle_l.y + PADDLE_HEIGHT);
    set_sprite_pos(paddle_r.data, paddle_r.x, paddle_r.y, paddle_r.y + PADDLE_HEIGHT);
    set_sprite_pos(ball.data, ball.x, ball.y, ball.y + BALL_SIZE);
}

/**
 * Reads the input and updates the paddle positions.
 * Return value: TRUE if the game should exit, FALSE otherwise
 */
BOOL handle_input(void)
{
    // now check the keyboard status
    read_keyboard();
    if (is_keydown(RAW_KEY_ESCAPE)) return TRUE;
    if (is_keydown(RAW_KEY_CURSOR_DOWN)) {
        paddle_r.y += PADDLE_SPEED;
        if (paddle_r.y > paddle_max_y) paddle_r.y = paddle_max_y;
    } else if (is_keydown(RAW_KEY_CURSOR_UP)) {
        paddle_r.y -= PADDLE_SPEED;
        if (paddle_r.y < PADDLE_MIN_Y) paddle_r.y = PADDLE_MIN_Y;
    }
    if (is_keydown(RAW_KEY_S)) {
        paddle_l.y += PADDLE_SPEED;
        if (paddle_l.y > paddle_max_y) paddle_l.y = paddle_max_y;
    } else if (is_keydown(RAW_KEY_W)) {
        paddle_l.y -= PADDLE_SPEED;
        if (paddle_l.y < PADDLE_MIN_Y) paddle_l.y = PADDLE_MIN_Y;
    }
    return FALSE;
}

// Sound effects for PTPlayer
UBYTE __chip beep_data[BEEP_BYTES];
UBYTE __chip buzz_data[BUZZ_BYTES];
static struct SfxStructure beep = { beep_data, BEEP_BYTES / 2, 0, 64, 0, 1};
static struct SfxStructure buzz = { buzz_data, BUZZ_BYTES / 2, 0, 64, 0, 1 };

static void init_sound_system(BOOL is_pal)
{
    mt_install_cia(&custom, NULL, is_pal);
    FILE *fp = fopen(BEEP_PATH, "r");
    fread(beep_data, sizeof(UBYTE), BEEP_BYTES, fp);
    fclose(fp);
    fp = fopen(BUZZ_PATH, "r");
    fread(buzz_data, sizeof(UBYTE), BUZZ_BYTES, fp);
    fclose(fp);
    beep.sfx_per = buzz.sfx_per = is_pal ? SAMPLE_PERIOD_PAL : SAMPLE_PERIOD_NTSC;
}

static BOOL initialize(void)
{
    init_keyboard_device();
    BOOL is_pal = init_display();
    const char *bgfile = is_pal ? IMG_FILENAME_PAL : IMG_FILENAME_NTSC;
    if (!ratr0_read_tilesheet(bgfile, &image)) {
        puts("Could not read background image");
        cleanup();
        return FALSE;
    }
    init_sound_system(is_pal);

    if (is_pal) {
        coplist[COPLIST_IDX_DIWSTOP_VALUE] = DIWSTOP_VALUE_PAL;
        vb_waitpos = 303;
    } else {
        coplist[COPLIST_IDX_DIWSTOP_VALUE] = DIWSTOP_VALUE_NTSC;
        vb_waitpos = 262;
    }
    int img_row_bytes = image.header.width / 8;
    UBYTE num_colors = 2;

    // Set up copper list
    // 1. background
    ULONG addr = (ULONG) image.imgdata;
    coplist[COPLIST_IDX_BPL1PTH_VALUE] = (addr >> 16) & 0xffff;
    coplist[COPLIST_IDX_BPL1PTH_VALUE + 2] = addr & 0xffff;

    // 2. sprite data pointers
    // sprites 0 and 1: paddles, sprite 2: ball
    coplist[COPLIST_IDX_SPR0_PTH_VALUE] = (((ULONG) paddle_l_data) >> 16) & 0xffff;
    coplist[COPLIST_IDX_SPR0_PTH_VALUE + 2] = ((ULONG) paddle_l_data) & 0xffff;

    coplist[COPLIST_IDX_SPR0_PTH_VALUE + 4] = (((ULONG) paddle_r_data) >> 16) & 0xffff;
    coplist[COPLIST_IDX_SPR0_PTH_VALUE + 6] = ((ULONG) paddle_r_data) & 0xffff;

    coplist[COPLIST_IDX_SPR0_PTH_VALUE + 8] = (((ULONG) ball_data) >> 16) & 0xffff;
    coplist[COPLIST_IDX_SPR0_PTH_VALUE + 10] = ((ULONG) ball_data) & 0xffff;

    // sprite 3-6: counter
    coplist[COPLIST_IDX_SPR0_PTH_VALUE + 12] = (((ULONG) player1.digit0_data) >> 16) & 0xffff;
    coplist[COPLIST_IDX_SPR0_PTH_VALUE + 14] = ((ULONG) player1.digit0_data) & 0xffff;

    coplist[COPLIST_IDX_SPR0_PTH_VALUE + 16] = (((ULONG) player1.digit1_data) >> 16) & 0xffff;
    coplist[COPLIST_IDX_SPR0_PTH_VALUE + 18] = ((ULONG) player1.digit1_data) & 0xffff;

    coplist[COPLIST_IDX_SPR0_PTH_VALUE + 20] = (((ULONG) player2.digit0_data) >> 16) & 0xffff;
    coplist[COPLIST_IDX_SPR0_PTH_VALUE + 22] = ((ULONG) player2.digit0_data) & 0xffff;

    coplist[COPLIST_IDX_SPR0_PTH_VALUE + 24] = (((ULONG) player2.digit1_data) >> 16) & 0xffff;
    coplist[COPLIST_IDX_SPR0_PTH_VALUE + 26] = ((ULONG) player2.digit1_data) & 0xffff;

    // sprite 7 is unused, initialize with empty
    coplist[COPLIST_IDX_SPR0_PTH_VALUE + 28] = (((ULONG) NULL_SPRITE_DATA) >> 16) & 0xffff;
    coplist[COPLIST_IDX_SPR0_PTH_VALUE + 30] = ((ULONG) NULL_SPRITE_DATA) & 0xffff;

    // and set the sprite position
    ball_max_y = is_pal ? BALL_MAX_Y_PAL : BALL_MAX_Y_NTSC;
    paddle_max_y = is_pal ? PADDLE_MAX_Y_PAL : PADDLE_MAX_Y_NTSC;
    update_paddle_and_ball_positions();

    set_sprite_pos(player1.digit0_data, DIGIT_L0_X, DIGIT_Y, DIGIT_Y + DIGIT_HEIGHT);
    set_sprite_pos(player1.digit1_data, DIGIT_L1_X, DIGIT_Y, DIGIT_Y + DIGIT_HEIGHT);
    set_sprite_pos(player2.digit0_data, DIGIT_R0_X, DIGIT_Y, DIGIT_Y + DIGIT_HEIGHT);
    set_sprite_pos(player2.digit1_data, DIGIT_R1_X, DIGIT_Y, DIGIT_Y + DIGIT_HEIGHT);
    // SET SPRITE DATA END

    set_digit(player1.digit0_data, 0);
    set_digit(player1.digit1_data, 0);
    set_digit(player2.digit0_data, 0);
    set_digit(player2.digit1_data, 0);

    // initialize and activate the copper list
    custom.cop1lc = (ULONG) coplist;
    custom.clxcon = 0x1000; // include both sprite 0 and 1 in collision detection
    return TRUE;
}

int main(int argc, char **argv)
{
    initialize();
    UWORD coll_state = *custom_clxdat; // read to clear
    int frames_since_collision = 0;

    // the event loop
    while (TRUE) {
        wait_vblank();

        if (handle_input()) break;
        ball.x += ball.speed_x;
        ball.y += ball.speed_y;
        update_paddle_and_ball_positions();

        coll_state = *custom_clxdat;

        if (ball.x < BALL_MIN_X) {
            mt_playfx(&custom, &buzz);
            increment_score(&player2);
            reset_ball(FALSE);
        } else if (ball.x > BALL_MAX_X) {
            mt_playfx(&custom, &buzz);
            increment_score(&player1);
            reset_ball(TRUE);
        } else if (ball.y <= BALL_MIN_Y || ball.y >= ball_max_y) {
            mt_playfx(&custom, &beep);
            ball.speed_y = -ball.speed_y;
        } else if (coll_state & 0x200 && frames_since_collision > 10) {
            // collision sprite 2 (ball) with 0 or 1 (paddles)
            mt_playfx(&custom, &beep);
            // where does the ball hit ? hit_y is between 0 and PADDLE_HEIGHT
            int hit_y = (ball.y + BALL_SIZE / 2) - (ball.speed_x < 0 ? paddle_l.y : paddle_r.y);

            // rel_y is between - PADDLE_HEIGHT / 2 and + PADDLE_HEIGHT / 2
            // rel_y > 0 => in the upper part
            // rel_y < 0 => in the lower part
            int rel_y = PADDLE_HEIGHT / 2 - hit_y;

            ball.speed_y = - (rel_y / 3);
            ball.speed_x = -ball.speed_x;
            frames_since_collision = 0;
        }
        frames_since_collision++;
    }
    cleanup();
    return 0;
}
