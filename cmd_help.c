/***************************************************************************//**
 * @file
 * @author     lizhijun
 * @version    V1.0
 * @date       2019-09-03 09:42:02
 * @brief      Command shell help function to access each commands usage help.
 * @details    This help function is treated as shell system function to run
 *             each commands usage help function.
 * @note
 ******************************************************************************/

#include <string.h>
#include "ShellCMDList.h"
#include "ShellPrintf.h"

/**
 * @defgroup   ShellCommands  The Shell Commands
 * Shell Commands
 */

/**
 * @defgroup   ShellCommands_Help The help funcitons of shell commands.
 * @ingroup    ShellCommands
 * Shell command help functions
 */

/**
 * @defgroup   ShellCommands_Main The main funcitons of shell commands.
 * @ingroup    ShellCommands
 * Shell command main functions
 */


#ifndef FIRMWAR_VERSION
#define FIRMWAR_VERSION   "SW:V1.1"
#endif

/**
 * @ingroup    ShellCommands_Help
 * @brief      Usage help for command `help`
 */
void cmd_help_usage_help(void)
{
    shell_print("\033[31mVersion: ");
    shell_print(FIRMWAR_VERSION);
    shell_print("\n\033[0mUsage:help <command name>\n");
}

/**
 * @ingroup    ShellCommands_Main
 * @brief      The main fucntion of command `help`
 *
 * @param[in]  argc  The count of arguments
 * @param      argv  The arguments array
 *
 * @return     The state of command proecess.
 * @retval     COMMAND_PROCESSED  Command is finished(only this will be return)
 */
COMMAND_STATE cmd_help_main(int argc, char **argv)
{
    unsigned int i;
    if(argc <= 1) {
        cmd_help_usage_help();
        shell_print("\033[33mAvailable Commands:\033[0m\n\033[36m");
        for(i = 0; (cmdList[i].shell_program != NULL) && (cmdList[i].shell_command_string != NULL); i++) {
            if(cmdList[i].shell_program != 0 || cmdList[i].shell_command_string != 0) {
                shell_print(cmdList[i].shell_command_string);
                shell_print("\t");
            }
        }
        shell_print("\033[0m\n");
    } else if(argc == 2) {
        for(i = 0; (cmdList[i].shell_program != NULL) && (cmdList[i].shell_command_string != NULL); i++) {
            if(!strcmp(argv[1], cmdList[i].shell_command_string)) {
                cmdList[i].usage_help();
                break;
            }
        }
    } else {
        cmd_help_usage_help();
    }

    return COMMAND_PROCESSED;
}
