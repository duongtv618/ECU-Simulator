#include "mode_man.h"


void mode_man_enter_mode(mode_man_t mode)
{
    /** Switch context here */
    switch (mode)
    {
    case MODE_MAN_INIT:
        break;

    case MODE_MAN_RUN:
        break;

    case MODE_MAN_DEGRADE:
        break;

    case MODE_MAN_FAIL_SAFE:
        break;

    case MODE_MAN_BROKEN:
        break;

    default:
        /** In case mode > MODE_MAN_BROKEN, through warning */
        // os_log(OS_DEBUG_CONSOLE, "WARNING: mode not exist\r\n");
        break;
    }
}