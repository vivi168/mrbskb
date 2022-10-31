#include <sys/types.h>

#include <mruby.h>

char* load_file(const char*, u_long*);
void mrb_io_module_init(mrb_state* mrb);
