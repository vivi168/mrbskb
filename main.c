#include <stdlib.h>
#include <stdio.h>
#include <libcd.h>

#include "header.h"

#include "renderer.h"
#include "level.h"
#include "input.h"
#include "sokoban_rb.h"

#include <mruby.h>
#include <mruby/string.h>
#include <mruby/internal.h>
#include <mruby/irep.h>
#include <mruby/compile.h>

#define HEAP_SIZE (1024 * 1024)
char heap[HEAP_SIZE];

Level level;

void process_input()
{
    if (iptm_quit_requested() || iptm_is_pressed(KEY_QUIT)) {
        printf("[INFO]: quit requested\n");
    }

    if (iptm_is_pressed(KEY_UP)) {
        if (lvl_move_player(&level, DIR_UP)) level.steps++;
    }
    else if (iptm_is_pressed(KEY_DOWN)) {
        if (lvl_move_player(&level, DIR_DOWN)) level.steps++;
    }
    else if (iptm_is_pressed(KEY_LEFT)) {
        if (lvl_move_player(&level, DIR_LEFT)) level.steps++;
    }
    else if (iptm_is_pressed(KEY_RIGHT)) {
        if (lvl_move_player(&level, DIR_RIGHT)) level.steps++;
    }

    if (iptm_is_pressed(KEY_RESTART)) {
        lvl_reset(&level);
    }
}

void mainloop()
{
    unsigned int frame_start;

    while (1) {
        frame_start = rdr_getticks();

        iptm_poll_events();
        process_input();
        lvl_check_level_done(&level);

        rdr_render(&level);
        rdr_delay(frame_start);
    }
}

static mrb_value mrb_f_foobar(mrb_state *mrb, mrb_value self)
{
    mrb_int i;

    mrb_get_args(mrb, "i", &i);

    return mrb_fixnum_value(i);
}

void puts_mrb_str(const char* s, int len)
{
    for (int i = 0; i < (int)len; i++) {
        putchar(s[i]);
    }
    putchar('\n');
}

static mrb_value mrb_f_puts(mrb_state *mrb, mrb_value self)
{
    mrb_value s = mrb_get_arg1(mrb);

    if (!mrb_string_p(s)) {
        s = mrb_inspect(mrb, mrb_get_arg1(mrb));
    }

    const char *p = RSTRING_PTR(s);
    mrb_int len = RSTRING_LEN(s);

    puts_mrb_str(p, (int)len);

    return mrb_nil_value();
}

void mrb_helper_init(mrb_state *mrb)
{
    mrb_define_method(mrb, mrb->kernel_module, "foobar", mrb_f_foobar, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, mrb->kernel_module, "puts", mrb_f_puts, MRB_ARGS_REQ(1));
}

void print_mrb_exception(mrb_state *mrb)
{
    if (!mrb->exc) return;

    mrb_value mesg = mrb_exc_inspect(mrb, mrb_obj_value(mrb->exc));
    puts_mrb_str(RSTRING_PTR(mesg), (int)RSTRING_LEN(mesg));
}

int main(int argc, char** argv)
{
    mrb_state *mrb;
    mrb_value v;

    InitHeap((void*)&heap, HEAP_SIZE);
    CdInit();

    rdr_init();
    iptm_init();

    printf("[INFO]: init done !\n");

    lvl_init(&level, 0);
    printf("[INFO]: level init done !\n");

    //----

    mrb = mrb_open();
    mrb_helper_init(mrb);

    v = mrb_load_irep(mrb, sokoban_rb);
    printf("EXECUTING MRUBY ON PSX : %d\n", mrb_fixnum(v));
    mrb_load_string(mrb, "a = { issou: 123, rire: '456' }; b = [1,2,3,4]; puts b; puts 'hello world'; puts a.inspect");

    print_mrb_exception(mrb);
    //----

    mainloop();

    rdr_cleanup();

    return 0;
}
