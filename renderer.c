#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

#include "io.h"
#include "renderer.h"

#include <mruby/string.h>

#define SCREEN_W 320
#define SCREEN_H 240

#define TILE_SIZE 16
#define OTLEN 8

typedef struct db_t {
    DISPENV disp;
    DRAWENV draw;
    u_long ot[OTLEN];
    int fnt_id;
    char pribuff[32768];
} DB;

typedef struct texture_t {
    u_long mode;
    u_char u, v;
    RECT prect, crect;

    u_short tpage, clut;
} Texture;

Texture texture;
DB db[2];
DB *cdb;
char *nextpri;

void rdr_render_sprite(RECT*, int, int);

void rdr_init()
{
    printf("[INFO]: init\n");

    ResetGraph(0);

    // First buffer
    SetDefDispEnv(&db[0].disp, 0, 0, SCREEN_W, SCREEN_H);
    SetDefDrawEnv(&db[0].draw, 0, SCREEN_H, SCREEN_W, SCREEN_H);

    db[0].draw.isbg = 1;
    setRGB0(&db[0].draw, 63, 0, 127);

    // Second buffer
    SetDefDispEnv(&db[1].disp, 0, SCREEN_H, SCREEN_W, SCREEN_H);
    SetDefDrawEnv(&db[1].draw, 0, 0, SCREEN_W, SCREEN_H);

    db[1].draw.isbg = 1;
    setRGB0(&db[1].draw, 63, 0, 127);

    cdb = &db[0];
    nextpri = cdb->pribuff;

    rdr_create_texture();

    FntLoad(960, 0);
    db[0].fnt_id = FntOpen(0, 8, 320, 224, 0, 100);
    db[1].fnt_id = FntOpen(0, 8, 320, 224, 0, 100);

    SetDispMask(1);
}

void rdr_create_texture()
{
    u_long file_size;
    int i;
    char *buff;

    TIM_IMAGE   *image;
    u_int       *filebuff;

    buff = load_file("\\SKB16.TIM;1", &file_size);
    if (buff == NULL) {
        printf("[ERROR]: error while loading texture\n");
        while(1);
    }

    OpenTIM((u_long*)buff);
    ReadTIM(image);

    // upload pixel data to framebuffer
    LoadImage(image->prect, image->paddr);
    DrawSync(0);

    // upload CLUT to framebuffer if any
    if (image->mode & 0x8) {
        LoadImage(image->crect, image->caddr);
        DrawSync(0);
    }

    // copy properties
    printf("[INFO]: %d %d %d\n", image->mode, image->prect->x, image->prect->y);
    texture.prect = *image->prect;
    texture.crect = *image->crect;
    texture.mode = image->mode;

    texture.u = (texture.prect.x % 0x40) << ( 2 - (texture.mode & 0x3));
    texture.v = (texture.prect.y & 0xff);

    texture.tpage = getTPage(texture.mode & 0x3, 0, texture.prect.x, texture.prect.y);
    texture.clut = getClut(texture.crect.x, texture.crect.y);

    printf("[INFO]: %d %d %d\n", texture.mode, texture.prect.x, texture.prect.y);

    free(buff);
}

void rdr_render(Level* level)
{
    ClearOTagR(cdb->ot, OTLEN);

    rdr_render_level(level);

    FntPrint(cdb->fnt_id, "LEVEL %d - STEPS %d", level->index+1, level->steps);
    FntFlush(cdb->fnt_id);
}

void rdr_render_level(Level* level)
{
    DR_TPAGE *tpage;
    int i, c;
    int hoff, voff;

    hoff = level->hoff * TILE_SIZE;
    voff = level->voff * TILE_SIZE;

    // player
    rdr_render_tile(hoff, voff, PLAYER_IDX, level->player_pos);

    // crates
    for (i = 0; i < level->crate_count; i++) {
        c = level->crates_pos[i];

        if (level->tiles[c] == TARGET_T)
            rdr_render_tile(hoff, voff, CRATET_IDX, c);
        else
            rdr_render_tile(hoff, voff, CRATEG_IDX, c);
    }

    for (i = 0; i < LVL_SIZE; i++) {
        switch (level->tiles[i]) {
        case VOID_T: break;
        case GROUND_T:
            rdr_render_tile(hoff, voff, GROUND_IDX, i);
            break;
        case TARGET_T:
            rdr_render_tile(hoff, voff, TARGET_IDX, i);
            break;
        case WALL_T:
            rdr_render_tile(hoff, voff, WALL_IDX, i);
            break;
        }
    }

    tpage = (DR_TPAGE*)nextpri;
    SetDrawTPage(tpage, 0, 1, texture.tpage);
    addPrim(cdb->ot, tpage);
    nextpri += sizeof(DR_TPAGE);
}

void rdr_render_tile(int hoff, int voff, int tile_index, int i)
{
    RECT src;
    int dst_x, dst_y;

    src.x = texture.u + (tile_index * TILE_SIZE);
    src.y = texture.v;
    src.w = TILE_SIZE;
    src.h = TILE_SIZE;

    dst_x = TILE_SIZE * (i % LVL_W) + hoff;
    dst_y = TILE_SIZE * (i / LVL_W) + voff;

    rdr_render_sprite(&src, dst_x, dst_y);
}

void rdr_render_sprite(RECT *src, int x, int y)
{
    SPRT *sprt;

    sprt = (SPRT*)nextpri;
    setSprt(sprt);

    setXY0(sprt, x, y);
    setWH(sprt, src->w, src->h);
    setUV0(sprt, src->x, src->y);
    setRGB0(sprt, 128, 128, 128);
    sprt->clut = texture.clut;

    addPrim(cdb->ot, sprt);
    nextpri += sizeof(SPRT);
}

unsigned int rdr_getticks()
{
    // TODO
    // VSync(-1)
    return 0;
}

void rdr_delay(int frame_start)
{
    DrawSync(0);
    VSync(0);

    PutDispEnv(&cdb->disp);
    PutDrawEnv(&cdb->draw);

    DrawOTag(&cdb->ot[OTLEN - 1]);

    cdb = (cdb == &db[0]) ? &db[1] : &db[0];
    nextpri = cdb->pribuff;
}

static mrb_value mrb_f_draw_tile(mrb_state* mrb, mrb_value self)
{
    mrb_int hoff, voff, tile_index, pos;

    mrb_get_args(mrb, "iiii", &hoff, &voff, &tile_index, &pos);
    rdr_render_tile(hoff * TILE_SIZE, voff * TILE_SIZE, tile_index, pos);

    return mrb_nil_value();
}

static mrb_value mrb_f_draw_tpage(mrb_state* mrb, mrb_value self)
{
    DR_TPAGE *tpage;

    tpage = (DR_TPAGE*)nextpri;
    SetDrawTPage(tpage, 0, 1, texture.tpage);
    addPrim(cdb->ot, tpage);
    nextpri += sizeof(DR_TPAGE);

    return mrb_nil_value();
}

static mrb_value mrb_f_clear_otag(mrb_state* mrb, mrb_value self)
{
    ClearOTagR(cdb->ot, OTLEN);

    return mrb_nil_value();
}

static mrb_value mrb_f_print(mrb_state* mrb, mrb_value self)
{
    mrb_value s = mrb_get_arg1(mrb);

    if (!mrb_string_p(s)) {
        return mrb_nil_value();
    }

    char* str = mrb_str_to_cstr(mrb, s);
    FntPrint(str);
    FntFlush(-1);

    return mrb_nil_value();
}

static mrb_value mrb_f_delay(mrb_state* mrb, mrb_value self)
{
    rdr_delay(0);

    return mrb_nil_value();
}

void mrb_graph_module_init(mrb_state *mrb, struct RClass* outer)
{
    struct RClass *psx_graph;

    rdr_init();

    psx_graph = mrb_define_module_under(mrb, outer, "Graph");

    mrb_define_const(mrb, psx_graph, "GROUND_IDX", mrb_int_value(mrb, GROUND_IDX));
    mrb_define_const(mrb, psx_graph, "TARGET_IDX", mrb_int_value(mrb, TARGET_IDX));
    mrb_define_const(mrb, psx_graph, "WALL_IDX", mrb_int_value(mrb, WALL_IDX));
    mrb_define_const(mrb, psx_graph, "PLAYER_IDX", mrb_int_value(mrb, PLAYER_IDX));
    mrb_define_const(mrb, psx_graph, "CRATEG_IDX", mrb_int_value(mrb, CRATEG_IDX));
    mrb_define_const(mrb, psx_graph, "CRATET_IDX", mrb_int_value(mrb, CRATET_IDX));

    mrb_define_module_function(mrb, psx_graph, "draw_tile", mrb_f_draw_tile, MRB_ARGS_REQ(4));
    mrb_define_module_function(mrb, psx_graph, "draw_tpage", mrb_f_draw_tpage, MRB_ARGS_NONE());
    mrb_define_module_function(mrb, psx_graph, "clear_otag", mrb_f_clear_otag, MRB_ARGS_NONE());
    mrb_define_module_function(mrb, psx_graph, "print", mrb_f_print, MRB_ARGS_REQ(1));
    mrb_define_module_function(mrb, psx_graph, "delay", mrb_f_delay, MRB_ARGS_NONE());

    printf("[INFO] PSX::Graph initialized\n");
}
