/***************************************************************************//**
 * @file
 * @author     lizhijun
 * @version    V1.0
 * @date       2020-06-09 21:34:50
 * @brief
 * @details
 * @note
 ******************************************************************************/
#include "ShellPrintf.h"

#ifndef shell_print
void shell_print(const char *string)
{
    while(* string != '\0') {
        shell_putc(* string++);
    }
}
#endif

#ifndef shell_println
void shell_println(const char *string)
{
    shell_print(string);
    shell_print((const char *) "\n");
    string = string;
}
#endif

#ifndef shell_message_handler
int shell_message_handler(void)
{
    return 0;
}
#endif
