/***************************************************************************//**
 * @file
 * @author     lizhijun
 * @version    V1.0
 * @date       2019-09-03 09:42:02
 * @brief      Define shell commands structs.
 * @details
 * @note
 ******************************************************************************/

#ifndef __Shell_CMD_LIST__H__
#define __Shell_CMD_LIST__H__

#include "Shell.h"
#include "ShellPort.h"

#ifndef NULL
#define NULL        0
#endif

typedef struct _shell_command_entry {
    const char *shell_command_string; ///< command name
    shell_program_t shell_program;  ///< command main funciton
    shell_program_usage_help_t usage_help; ///< command help function
} shell_command_entry;


/// Declare a shell command functions with `NAME`.
/// There are two functions required as format:
/// - void cmd_<NAME>_usage_help(void)
/// - int cmd_<NAME>_main(int argc, char **argv)
#define CMD_EXPORT(NAME)  void cmd_##NAME##_usage_help(void);COMMAND_STATE cmd_##NAME##_main(int argc, char **argv);

/// Add command `NAME` to #cmdList
#define CMD_ADD(NAME)     { #NAME,            cmd_##NAME##_main,                   cmd_##NAME##_usage_help    },

#define CMD_MAIN_DEFINE(NAME)       COMMAND_STATE cmd_##NAME##_main(int argc, char **argv)
#define CMD_HELP_MSG_DEFINE(NAME)   void cmd_##NAME##_usage_help(void)

/// #cmdList is extern defined accroding platform
extern RAM_TYPE shell_command_entry cmdList[];

#endif
