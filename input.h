#ifndef INPUT_H
#define INPUT_H

#define TRUE 1
#define FALSE 0

// kept aligned with PSX PAD specs
#define KEY_UP      4
#define KEY_RIGHT   5
#define KEY_DOWN    6
#define KEY_LEFT    7
#define KEY_RESTART 12   // triangle 1 << 12
#define KEY_QUIT    15   // square   1 << 15

typedef struct input_manager_t {
    int quit;
    unsigned int new_keystate, old_keystate;
    unsigned int keys_pressed, keys_held, keys_released;
} InputManager;

void iptm_init();
void iptm_update();
void iptm_poll_events();
int iptm_quit_requested();
int iptm_is_held(int);
int iptm_is_pressed(int);
int iptm_is_released(int);

#endif
