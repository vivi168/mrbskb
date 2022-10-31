#include <stdio.h>
#include <sys/types.h>
#include <libapi.h>

#include "input.h"

typedef struct _PADTYPE
{
    unsigned char   stat;
    unsigned char   len:4;
    unsigned char   type:4;
    unsigned short  btn;
    unsigned char   rs_x,rs_y;
    unsigned char   ls_x,ls_y;
} PADTYPE;

PADTYPE *pad;
u_char padbuff[2][34];

InputManager input_manager;

void iptm_init()
{
    printf("[INFO]: pad init\n");
    InitPAD( padbuff[0], 34, padbuff[1], 34 );

    padbuff[0][0] = padbuff[0][1] = 0xff;
    padbuff[1][0] = padbuff[1][1] = 0xff;

    StartPAD();
    ChangeClearPAD(1);

    input_manager.new_keystate = 0;
    input_manager.old_keystate = 0;

    input_manager.keys_held = 0;
    input_manager.keys_pressed = 0;
    input_manager.keys_released = 0;
}

void iptm_poll_events()
{
    // Parse controller input
    pad = (PADTYPE*)padbuff[0];

    // Only parse inputs when a controller is connected
    if( pad->stat == 0 )
    {
        // Only parse when a digital pad,
        // dual-analog and dual-shock is connected
        if( ( pad->type == 0x4 ) ||
            ( pad->type == 0x5 ) ||
            ( pad->type == 0x7 ) )
        {
            input_manager.old_keystate = input_manager.new_keystate;
            input_manager.new_keystate = pad->btn ^ 0xffff;

            input_manager.keys_pressed =  (input_manager.old_keystate ^ input_manager.new_keystate) & input_manager.new_keystate;
            input_manager.keys_held = input_manager.old_keystate & input_manager.new_keystate;
            input_manager.keys_released = input_manager.old_keystate ^ input_manager.new_keystate;
        }
    }
}

int iptm_is_held(int k) {
    return (1 << k) & input_manager.keys_held;
}

int iptm_is_pressed(int k) {
    return (1 << k) & input_manager.keys_pressed;
}

int iptm_is_released(int k) {
    return (1 << k) & input_manager.keys_released;
}

void mrb_pad_module_init(mrb_state* mrb)
{

    struct RClass *psx_pad;

    psx_pad = mrb_define_module(mrb, "PSXPad");

    // iptm_init()

    /* mrb_define_module_function(mrb, psx_pad, "poll", mrb_f_poll, MRB_ARGS_NONE()); */
    /* mrb_define_module_function(mrb, psx_pad, "held?", mrb_f_is_held, MRB_ARGS_REQ(1)); */
    /* mrb_define_module_function(mrb, psx_pad, "pressed?", mrb_f_is_pressed, MRB_ARGS_REQ(1)); */
    /* mrb_define_module_function(mrb, psx_pad, "released?", mrb_f_is_released, MRB_ARGS_REQ(1)); */

    mrb_define_const(mrb, psx_pad, "KEY_UP", mrb_int_value(mrb, KEY_UP));
    mrb_define_const(mrb, psx_pad, "KEY_RIGHT", mrb_int_value(mrb, KEY_RIGHT));
    mrb_define_const(mrb, psx_pad, "KEY_DOWN", mrb_int_value(mrb, KEY_DOWN));
    mrb_define_const(mrb, psx_pad, "KEY_LEFT", mrb_int_value(mrb, KEY_LEFT));
    mrb_define_const(mrb, psx_pad, "KEY_TRIANGLE", mrb_int_value(mrb, KEY_TRIANGLE));
}
