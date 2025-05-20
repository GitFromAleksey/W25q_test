#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <stdint.h>

#ifdef __cplusplus
    extern "C" {
#endif

#define CNSL_MAX_CMDS_NUM    10u
// указатель обработчика команды
typedef void (*console_command_handler_t)(const void *);

typedef struct
{
  const char * cmd_name;
  const char * cmd_help;
  console_command_handler_t handler;

} console_command_t; // структура команды
// ----------------------------------------------------------------------------
typedef struct
{
  uint16_t cmd_counter;
  console_command_t commands[CNSL_MAX_CMDS_NUM];

} console_commands_t; // структура массива команд
// ----------------------------------------------------------------------------
void ConsoleCommandAdd( char * cmd_name, 
                        console_command_handler_t handler,
                        const char * cmd_help);
// ----------------------------------------------------------------------------
void ConsoleCommandExecutor(const char * cmd_name, console_commands_t *commands);


#ifdef __cplusplus
    }
#endif

#endif /* _CONSOLE_H_ */
