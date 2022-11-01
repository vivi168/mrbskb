#include <stdlib.h>
#include <stdio.h>
#include <libcd.h>

#include "header.h"

#include <mruby.h>
#include <mruby/string.h>
#include <mruby/internal.h>
#include <mruby/irep.h>
#include <mruby/compile.h>

#include "renderer.h"
#include "io.h"
#include "input.h"
#include "sokoban_rb.h"

#define HEAP_SIZE (1024 * 1024)
char heap[HEAP_SIZE];

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
    struct RClass *psx;

    mrb_define_method(mrb, mrb->kernel_module, "puts", mrb_f_puts, MRB_ARGS_REQ(1));

    psx = mrb_define_module(mrb, "PSX");

    mrb_io_module_init(mrb, psx);
    mrb_pad_module_init(mrb, psx);
    mrb_graph_module_init(mrb, psx);
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

    InitHeap((void*)&heap, HEAP_SIZE);

    mrb = mrb_open();
    mrb_helper_init(mrb);
    mrb_load_irep(mrb, sokoban_rb);

    print_mrb_exception(mrb);

    for (;;) { /* should never get there unless ruby error */ }

    return 0;
}
