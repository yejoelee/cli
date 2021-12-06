/***************************************************************************//**
 * @file
 * @author     lizhijun
 * @version    V1.0
 * @date       2019-08-29 17:16:59
 * @brief      Define all support commands.
 * @details    It's a #shell_command_entry arrary end with {NULL, NULL, NULL}
 * @note
 ******************************************************************************/

#include "ShellCMDList.h"

CMD_EXPORT(help)

/**
 * Regist shell commands to arrary #cmdList.
 */
RAM_TYPE shell_command_entry cmdList[] = {
    { "help",            cmd_help_main,         cmd_help_usage_help    },

    //End line flag. Do not remove below line.
    {  NULL,             NULL,                            NULL    }
};
