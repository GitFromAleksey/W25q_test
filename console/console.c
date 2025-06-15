#include <stdio.h>
#include <string.h>
#include "console.h"

#define HELP_CMD_NAME    "help"

static uint8_t rxbuf[20];
// массив команд
static console_commands_t ConsoleCommands = {0};

void CmdHelp(const void *param)
{
  printf("\r\n<<< ------ HELP ------ >>>\r\n");
  for(int i = 0; i < ConsoleCommands.cmd_counter; ++i)
  {
    printf("Command: %s\t", ConsoleCommands.commands[i].cmd_name);
    printf("desc: %s\r\n", ConsoleCommands.commands[i].cmd_help);
  }
}
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

  res = strstr(cmd_name, HELP_CMD_NAME );
  if(res != NULL)
  {
    CmdHelp(NULL);
    return;
  }

  for(int i = 0; i < commands->cmd_counter; ++i)
  {
    res = strstr(cmd_name, commands->commands[i].cmd_name );
    if(res != NULL)
    {
      commands->commands[i].handler(cmd_name);
      return;
    }
  }
}
// ----------------------------------------------------------------------------
void ConsoleRun(void)
{
  if(fgets((char*)&rxbuf[0], sizeof(rxbuf), stdin) != NULL)
  {
    if(rxbuf[0] != '\n')
    {
      ConsoleCommandExecutor((char*)rxbuf, &ConsoleCommands);
      //Cmd((char *)rxbuf);
    }
  }
}
// ----------------------------------------------------------------------------
