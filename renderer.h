#ifndef RENDERER_H
#define RENDERER_H

#include "level.h"

// tiles indices in spritesheet
#define GROUND_IDX 0
#define TARGET_IDX 1
#define WALL_IDX 2
#define PLAYER_IDX 3
#define CRATEG_IDX 4
#define CRATET_IDX 5

void rdr_init();
void rdr_create_texture();
void rdr_render(Level*);
void rdr_render_level(Level*);
void rdr_render_tile(int, int, int, int);
void rdr_cleanup();

unsigned int rdr_getticks();
void rdr_delay();

#undef gte_rtps
#define gte_rtps() __asm__ volatile (\
    "nop;"                  \
    "nop;"                  \
    "cop2 0x0180001;")

#undef gte_rtpt
#define gte_rtpt() __asm__ volatile (\
    "nop;"                  \
    "nop;"                  \
    "cop2 0x0280030;")

#undef gte_nclip
#define gte_nclip() __asm__ volatile (\
    "nop;"                  \
    "nop;"                  \
    "cop2 0x1400006;")

#undef gte_avsz4
#define gte_avsz4() __asm__ volatile (\
    "nop;"                  \
    "nop;"                  \
    "cop2 0x168002E;")

#undef gte_avsz3
#define gte_avsz3() __asm__ volatile (\
    "nop;"                  \
    "nop;"                  \
    "cop2 0x158002D;")

#endif
