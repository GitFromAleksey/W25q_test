#include <string.h>
#include "console.h"

// массив команд
static console_commands_t ConsoleCommands;

// добавлятель новых команд
void ConsoleCommandAdd( char * cmd_name, 
                        console_command_handler_t handler,
                        const char * cmd_help)
{
  uint16_t cmd_counter = ConsoleCommands.cmd_counter;

  if(cmd_counter < CNSL_MAX_CMDS_NUM)
  {
    ConsoleCommands.commands[cmd_counter].cmd_name = cmd_name;
    ConsoleCommands.commands[cmd_counter].cmd_help = cmd_help;
    ConsoleCommands.commands[cmd_counter].handler  = handler;

    ++cmd_counter;
    ConsoleCommands.cmd_counter = cmd_counter;
  }
}
// ----------------------------------------------------------------------------
// исполнятель команд
void ConsoleCommandExecutor(const char * cmd_name, console_commands_t *commands)
{
  char * res;

  for(int i = 0; i < commands->cmd_counter; ++i)
  {
    res = strstr(cmd_name, commands->commands[i].cmd_name );
    if(res != NULL)
    {
      commands->commands[i].handler(NULL);
      return;
    }
  }
}
// ----------------------------------------------------------------------------
