#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#include "tokenizer.h"


/*
Notes:
======

struct tokens {
  size_t tokens_length;
  char** tokens;
  size_t buffers_length;
  char** buffers;
};

*/

/*
------------------------------------------------------------
*/
/* Convenience macro to silence compiler warnings about unused function parameters. */
#define unused __attribute__((unused))

/* Whether the shell is connected to an actual terminal or not. */
bool shell_is_interactive;

/* File descriptor for the shell input */
int shell_terminal;

/* Terminal mode settings for the shell */
struct termios shell_tmodes;

/* Process group id for the shell */
pid_t shell_pgid;

struct tokens {
  size_t tokens_length;
  char** tokens;
  size_t buffers_length;
  char** buffers;
};

int cmd_exit(struct tokens* tokens);
int cmd_help(struct tokens* tokens);

/*The new functions implemented*/
int cmd_cwd(struct tokens *tokens);
int cmd_cd(struct tokens *tokens);

/* Built-in command functions take token array (see parse.h) and return int */
typedef int cmd_fun_t(struct tokens* tokens);

/* Built-in command struct and lookup table */
typedef struct fun_desc {
  cmd_fun_t* fun;
  char* cmd;
  char* doc;
} fun_desc_t;

fun_desc_t cmd_table[] = {
    {cmd_help, "?", "show this help menu"},
    {cmd_exit, "exit", "exit the command shell"},
    {cmd_cwd, "cwd", "print current working directory"},
    {cmd_cd, "cd", "change directory"},
};

int cmd_cwd(unused struct tokens *tokens) {

    /*

    We used the following inbuild c-function to change the .urrent working directoryl
   
    char *getcwd(char *buf, size_t size);

    The getcwd() function places an absolute pathname of the current working directory 
    in the array pointed to by buf, and returns buf. The size argument is the size in 
    bytes of the character array pointed to by the buf argument. If buf is a null pointer, 
    the behaviour of getcwd() is undefined.The return value represent our current working 
    directory.

    Ref: https://iq.opengenus.org/chdir-fchdir-getcwd-in-c/
    */
  
  long size;
  char *buf;
  char *ptr;
  size = pathconf(".", _PC_PATH_MAX);
  if ((buf = (char *)malloc((size_t)size)) != NULL)
      {ptr = getcwd(buf, (size_t)size);}  //inbuilt c command to get current working directory.
  printf("%s\n",buf);
  return 1;
}

int cmd_cd(struct tokens *token) {
  /*
    It changes the current working directory to the directory path given in the second argument
    of shell.

    Function Definitin of built in c function chdir:
        int chdir( const char *pathname );

    Return value : The function return a integer value ,it returns 0 
    if the change of directory was successful otherwise it returns -1 
    and the current working directory remains unchanged and errno is set 
    to to indicate the error type.

    Ref : https://iq.opengenus.org/chdir-fchdir-getcwd-in-c/


  */

  char *new_dir = tokens_get_token(token, 1); //get the second argument in the commandline (second line would be the new path)
  return chdir(new_dir); // change the directory to the new path
}

/* Prints a helpful description for the given command */
int cmd_help(unused struct tokens* tokens) {
  for (unsigned int i = 0; i < sizeof(cmd_table) / sizeof(fun_desc_t); i++)
    printf("%s - %s\n", cmd_table[i].cmd, cmd_table[i].doc);
  return 1;
}

/* Exits this shell */
int cmd_exit(unused struct tokens* tokens) { exit(0); }

/* Looks up the built-in command, if it exists. */
int lookup(char cmd[]) {
  for (unsigned int i = 0; i < sizeof(cmd_table) / sizeof(fun_desc_t); i++)
    if (cmd && (strcmp(cmd_table[i].cmd, cmd) == 0))
      return i;
  return -1;
}

/* Intialization procedures for this shell */
void init_shell() {
  /* Our shell is connected to standard input. */
  shell_terminal = STDIN_FILENO;

  /* Check if we are running interactively */
  shell_is_interactive = isatty(shell_terminal);

  if (shell_is_interactive) {
    /* If the shell is not currently in the foreground, we must pause the shell until it becomes a
     * foreground process. We use SIGTTIN to pause the shell. When the shell gets moved to the
     * foreground, we'll receive a SIGCONT. */
    while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp()))
      kill(-shell_pgid, SIGTTIN);

    /* Saves the shell's process id */
    shell_pgid = getpid();

    /* Take control of the terminal */
    tcsetpgrp(shell_terminal, shell_pgid);

    /* Save the current termios to a variable, so it can be restored later. */
    tcgetattr(shell_terminal, &shell_tmodes);
  }
}

void pipes(struct tokens* tokens)
{
  int i;
  int start=0;
  i=0;

 
  while (i<tokens->tokens_length)
  {
    start=i;
    
    while(i<tokens->tokens_length && strcmp(tokens_get_token(tokens,i),"|")!=0)
     { 
        i++;
      }
   
  
    int fundex = lookup(tokens_get_token(tokens, start));

    if (fundex >= 0) {
      cmd_table[fundex].fun(tokens);
    } 
    else {
      /* REPLACE this to run commands as programs. */
      int status;
     
      pid_t pid = fork(); 
      if (pid > 0) {
        wait(&status); 
      }
      else {
        
        char **arg = (char **) malloc(i-start);
   
        for (int j = start; j < i; ++j) 
        {
          arg[j-start] = (char *) malloc(100); 
          strcpy(arg[j-start], tokens_get_token(tokens, j)); 
        }
        //executing the program.
        execv(tokens_get_token(tokens, start), arg);
      }
     
    }
    i++;
  }
    
}

int main(unused int argc, unused char* argv[]) {
  init_shell();

  static char line[4096];
  int line_num = 0;

  /* Please only print shell prompts when standard input is not a tty */
  if (shell_is_interactive)
    fprintf(stdout, "%d: ", line_num);

  while (fgets(line, 4096, stdin)) {
    /* Split our line into words. */
    struct tokens* tokens = tokenize(line);
    
    int i=0;
    int flag=0;

    while(i<tokens->tokens_length)
    {
      if (strcmp(tokens_get_token(tokens,i),"|")==0){ // If pipes are present
        pipes(tokens); // pipes function is executed.
        flag=1;
        break;
      }
      i++;
    }

    if (flag==1)
    {
      fprintf(stdout, "%d: ", ++line_num);
      continue;
    }
    if (flag==0) // if there are no pipes
    {
      /* Find which built-in function to run. */
    int fundex = lookup(tokens_get_token(tokens, 0));

    if (fundex >= 0) {
      cmd_table[fundex].fun(tokens);
    } else {
      /* REPLACE this to run commands as programs. */
      int status;

      /*We create a new child process to execute the process*/
      pid_t pid = fork(); 
      
      if (pid > 0) {
        wait(&status); // The parent process waits for the child process to die.
      }
       else {

         // Child Process: Executing the current command.
        char **arg = (char **) malloc(tokens_get_length(tokens));

        //getting all the arguments into arg

        for (int i = 0; i < tokens_get_length(tokens); ++i) 
        {
          arg[i] = (char *) malloc(100); 
          strcpy(arg[i], tokens_get_token(tokens, i)); //copy the current input argument into arg
        }

        //executing the program.
        execv(tokens_get_token(tokens, 0), arg);
      }
      // fprintf(stdout, "This shell doesn't know how to run programs.\n");
    }

    if (shell_is_interactive)
      /* Please only print shell prompts when standard input is not a tty */
      fprintf(stdout, "%d: ", ++line_num);

    /* Clean up memory */
    tokens_destroy(tokens);
  }
    }
    
    

  return 0;
}
