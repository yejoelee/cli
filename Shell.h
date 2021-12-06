/***************************************************************************//**
 * @file
 * @author     lizhijun
 * @version    V1.0
 * @date       2019-08-29 17:16:47
 * @brief      The most important part of Shell with command parsing and run.
 * @details    Below dependents must be satisfied to make Shell run properly:
 *             - #Shell_RecvData should be put into uart receiver handle.
 *             - #Shell_Command_Task should be put into main task loop.
 * @note
 ******************************************************************************/

#include "ShellCustomerPort.h"

#ifndef __SHELL_H__
#define __SHELL_H__

typedef enum {
    /// Command is finished.
    COMMAND_PROCESSED = 0,
    /// Command is running as state machine. It will be called again at next main loop.
    /// @note Other commands are blocked untill last command return #COMMAND_PROCESSED.
    COMMAND_IN_PROCESSING,
    /// An error occured while running the command.
    COMMAND_ERROR
} COMMAND_STATE;


typedef COMMAND_STATE(*shell_program_t)(int, char **);
typedef void (*shell_program_usage_help_t)(void);

#ifndef CONFIG_CLI
#define Shell_RecvData(c)
#define Shell_Command_Task()
#else
/**
 * @brief      Get a char from console and response.
 * @details    This function handle the console input and give back a response.\n
 *             The command is input finished once '\\n' is received.\n
 *             And then #Shell_Command_Task() will parse the command.
 * @note       This function should be put into uart receiver handle.
 *
 * @param[in]  rxchar  The char get from console.
 */
void Shell_RecvData(char rxchar);

/**
 * @brief      Parsing command and run.
 * @note       This funciton should be put into main task loop.
 */
void Shell_Command_Task(void);

/**
 * @brief      Console Init
 * @details    Call this function if console device initialize by Shell.
 *             This function is implemented on @ref platform_depends.h
 */
#ifdef shell_device_init
int Shell_Console_init(void);
#endif
#endif

#endif
