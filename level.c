#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include "io.h"

#include "level.h"

void fill_ground(Level*);
void empty_crate_pos(Level*);
int lvl_move_crate(Level*, int, Direction);
void lvl_next_level(Level*);

void lvl_init(Level* level, int index)
{
    const char *filename;
    char *pdef = &level->def[0];
    u_long file_size;
    int i;
    char *buff;

    level->index = index;
    filename = level_LUT[index];

    level->size = 0;

    buff = load_file(filename, &file_size);

    if (buff == NULL) {
        printf("[ERROR]: error while loading level file %s\n", filename);
        while(1);
    }

    for(i = 0; i < file_size; i++) {
        *pdef++ = buff[i];

        level->size++;
        if (level->size > LVL_SIZE) {
            printf("[ERROR]: Level too large\n");
            while(1);
        }
    }
    printf("[INFO]: reading level done!!\n");

    free(buff);
    lvl_reset(level);
}

void lvl_reset(Level* level)
{
    char c;
    int *pcrate_pos = &level->crates_pos[0];
    int k, l, i;
    int w, h;
    int draw_ground = 0;

    w = 0; h = 0;
    level->crate_count = 0;
    level->steps = 0;
    k = 0; l = 0;

    fill_ground(level);
    empty_crate_pos(level);

    for (i = 0; i < level->size; i++) {
        c = level->def[i];
        printf("%x ", c);
        switch(c) {
        case GROUND_CHAR:
            if (draw_ground) level->tiles[k] = GROUND_T;
            break;
        case TARGET_CHAR:
            level->tiles[k] = TARGET_T;
            break;
        case TARGET_PCHAR:
            level->tiles[k] = TARGET_T;
            level->player_pos = k;
            break;
        case TARGET_CCHAR:
            level->tiles[k] = TARGET_T;
            *pcrate_pos++ = k;
            level->crate_count++;
            break;
        case WALL_CHAR:
            level->tiles[k] = WALL_T;
            draw_ground = 1;
            break;
        case CRATE_CHAR:
            *pcrate_pos++ = k;
            level->crate_count++;
            if (draw_ground) level->tiles[k] = GROUND_T;
            break;
        case PLAYER_CHAR:
            level->player_pos = k;
            if (draw_ground) level->tiles[k] = GROUND_T;
            break;
        case NEWLINE_CHAR:
            if (l > w) w = l;
            h++;
            draw_ground = 0;

            k += LVL_W - l;
            l = 0;
            continue;
        }

        l++;
        k++;
    }

    level->hoff = (LVL_W - w) / 2;
    level->voff = (LVL_H - h) / 2;
}

void fill_ground(Level* level)
{
    int i;

    for (i = 0; i < LVL_SIZE; i++) {
        level->tiles[i] = VOID_T;
    }
}

void empty_crate_pos(Level* level)
{
    int i;

    for (i = 0; i < MAX_CRATES; i++) {
        level->crates_pos[i] = -1;
    }
}

int lvl_move_player(Level* level, Direction dir)
{
    int prev_pos = level->player_pos;
    int i, moved;

    switch (dir) {
    case DIR_UP:
        level->player_pos -= LVL_W;
        break;
    case DIR_LEFT:
        level->player_pos --;
        break;
    case DIR_DOWN:
        level->player_pos += LVL_W;
        break;
    case DIR_RIGHT:
        level->player_pos++;
        break;
    }

    for (i = 0; i < level->crate_count; i++) {
        if (level->player_pos == level->crates_pos[i]) {
            moved = lvl_move_crate(level, i, dir);
            if (!moved) level->player_pos = prev_pos;
            break;
        }
    }

    if (level->player_pos < 0 || level->player_pos > LVL_SIZE) {
        level->player_pos = prev_pos;
    }

    if (level->tiles[level->player_pos] == WALL_T) level->player_pos = prev_pos;

    return prev_pos != level->player_pos;
}

int lvl_move_crate(Level* level, int i, Direction dir)
{
    int crate_pos = level->crates_pos[i];
    int c, moved;

    switch (dir) {
    case DIR_UP:
        crate_pos -= LVL_W;
        break;
    case DIR_LEFT:
        crate_pos--;
        break;
    case DIR_DOWN:
        crate_pos += LVL_W;
        break;
    case DIR_RIGHT:
        crate_pos++;
        break;
    }

    if (crate_pos < 0 || crate_pos > LVL_SIZE) {
        crate_pos = level->crates_pos[i];
    }

    if (level->tiles[crate_pos] == WALL_T) crate_pos = level->crates_pos[i];

    for (c = 0; c < level->crate_count; c++) {
        if (c != i && crate_pos == level->crates_pos[c]) {
            crate_pos = level->crates_pos[i];
            break;
        }
    }

    moved = crate_pos != level->crates_pos[i];

    level->crates_pos[i] = crate_pos;

    return moved;
}

int lvl_check_level_done(Level* level)
{
    int cleared;
    int done;
    int i, c;

    cleared = 0; done = 0;

    for (i = 0; i < level->crate_count; i++) {
        c = level->crates_pos[i];
        if (level->tiles[c] == TARGET_T)
            cleared++;
    }

    if (cleared == level->crate_count) {
        lvl_next_level(level);
        done = 1;
    }

    return done;
}

void lvl_next_level(Level* level)
{
    int next_level;

    next_level = level->index+1;

    if (next_level >= LEVEL_COUNT) {
        next_level = 0;
    }

    printf("[INFO]: loading next level %d\n", next_level);

    lvl_init(level, next_level);
}
