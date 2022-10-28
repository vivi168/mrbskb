#ifndef LEVEL_H
#define LEVEL_H

#define LVL_W 20
#define LVL_H 15
#define LVL_SIZE (LVL_W * LVL_H)
#define MAX_CRATES 31

#define GROUND_CHAR  ' '
#define TARGET_CHAR  '.'
#define TARGET_PCHAR '+'
#define TARGET_CCHAR '*'
#define WALL_CHAR    '#'
#define CRATE_CHAR   '$'
#define PLAYER_CHAR  '@'
#define NEWLINE_CHAR '\n'

#define LEVEL_COUNT 99

extern const char* level_LUT[LEVEL_COUNT];

typedef enum tile_t {
    GROUND_T = 0,
    TARGET_T,
    WALL_T,
    VOID_T
} Tile;

typedef enum direction_t {
    DIR_UP = 0,
    DIR_LEFT,
    DIR_DOWN,
    DIR_RIGHT
} Direction;

typedef struct level_t {
    char def[LVL_SIZE];
    Tile tiles[LVL_SIZE];
    int crates_pos[MAX_CRATES];
    int size, crate_count;
    int player_pos;
    int hoff, voff;
    int steps;
    int index;
} Level;

void lvl_init(Level*, int);
void lvl_reset(Level*);
int lvl_move_player(Level*, Direction);
int lvl_check_level_done(Level*);

#endif
