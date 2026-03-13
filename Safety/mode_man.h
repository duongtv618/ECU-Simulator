#ifndef MODE_MAN_H
#define MODE_MAN_H

typedef enum mode_man_e {
    MODE_MAN_INIT = 0u,
    MODE_MAN_RUN,
    MODE_MAN_DEGRADE,
    MODE_MAN_FAIL_SAFE,
    MODE_MAN_BROKEN
} mode_man_t;



void mode_man_enter_mode(mode_man_t mode);  

#endif