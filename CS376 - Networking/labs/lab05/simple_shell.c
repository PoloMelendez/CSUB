/* This code is modified from the s_sh.c code provided by Marc Thomas
 *
 * This program is a VERY simple shell that just allows you to do 'echo',
 * 'ls' and 'exit'. All other commands from Dr. Thomas's simple shell
 * program have been removed as there is NO authentication with this code
 * so ANYONE can telnet to your s_daemon port and issue these commands.
 *
 * From the perspective of this program, the pipes created by the simple
 * daemon dialog_with_client() function are simply stdin and stdout.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[], char *envp[])
{
  char buf[512], command[128];
  int i, k, ret;

  while(1)
  {
    printf("\r\ns_sh> ");
    fflush(stdout);
    bzero(buf, sizeof(buf));
    bzero(command, sizeof(command));

    ret = read(0, buf, sizeof(buf));
    k = 0;

    /* Extract the first word into the command string */
    for(i = 0; i < sizeof(buf); i++)
    {
      if(buf[i] == '\n' || buf[i] == '\r' || buf[i] == '\0' || buf[i] == ' ' ||
          k >= 127)
      {
        break;
      }
      command[k++] = buf[i];
    }
    command[k] = '\0';
      
    if(strcmp(command, "echo") == 0)
    {
      if(k < strlen(buf))
        printf("%s\r\n", &(buf[k+1]));
      else
        printf("Missing argument to echo command.\n");
    }
    else if(strcmp(command, "ls") == 0)
    {
      printf("Directory listing disabled for security reasons.\r\n");
    }
    else if(strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0)
    {
      printf("Goodbye!\r\n");
      fflush(stdout);
      break;
    }
    else
    {
      printf("%s command not found.\n", command);
    }
    fflush(stdout);
  }
  return 0;
}
