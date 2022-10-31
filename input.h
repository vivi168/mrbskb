#ifndef INPUT_H
#define INPUT_H

#include <mruby.h>

#define KEY_UP      4
#define KEY_RIGHT   5
#define KEY_DOWN    6
#define KEY_LEFT    7
#define KEY_RESTART 12   // triangle 1 << 12

typedef struct input_manager_t {
    unsigned int new_keystate, old_keystate;
    unsigned int keys_pressed, keys_held, keys_released;
} InputManager;

void iptm_init();
void iptm_poll_events();
int iptm_is_held(int);
int iptm_is_pressed(int);
int iptm_is_released(int);

void mrb_pad_module_init(mrb_state* mrb);

#endif
