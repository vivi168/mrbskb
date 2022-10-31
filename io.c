#include <stdio.h>
#include <stdlib.h>
#include <libcd.h>
#include "io.h"

#include <mruby/string.h>

char* load_file(const char* filename, u_long *size)
{
    CdlFILE file;
    int     sectors;
    char    *buff;

    buff = NULL;

    printf("[INFO]: looking for %s\n", filename);

    if(CdSearchFile(&file, filename) == NULL) {
        printf("[ERROR]: File not found %s\n", filename);
        return buff;
    }

    printf("[INFO]: found %s\n", filename);
    sectors = (file.size + 2047) / 2048;
    buff = (char*)malloc(2048 * sectors);
    CdControl(CdlSetloc, (u_char*)&file.pos, 0);
    CdRead(sectors, (u_long*)buff, CdlModeSpeed);
    printf("[INFO]: sectors: %d, file size: %d\n", sectors, file.size);
    CdReadSync(0, 0);

    printf("[INFO]: read sync done\n");
    *size = file.size;

    return buff;
}

static mrb_value mrb_f_load_file(mrb_state* mrb, mrb_value self)
{
    long file_size;
    char* filename, *buff;
    mrb_value s = mrb_get_arg1(mrb);
    mrb_ensure_string_type(mrb, s);

    filename = mrb_str_to_cstr(mrb, s);

    printf("LOADING FILE THROUGH RUBY %s\n", filename);

    buff = load_file(filename, &file_size);

    for (long i = 0; i < file_size; i++) {
        printf("%x ", buff[i]);
    }
    printf("\n");

    mrb_value str = mrb_str_new(mrb, buff, file_size);

    free(buff);
    return str;
}

void mrb_io_module_init(mrb_state *mrb)
{
    struct RClass *psx_io;

    psx_io = mrb_define_module(mrb, "PSXIO");
    mrb_define_module_function(mrb, psx_io, "load_file", mrb_f_load_file, MRB_ARGS_REQ(1));
}

