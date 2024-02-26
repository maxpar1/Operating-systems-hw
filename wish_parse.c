#include <stdio.h>
#include "wish.h"
#include <sys/wait.h> // For waitpid
#include <unistd.h>
#define _POSIX_C_SOURCE 200809L
#include <string.h>
// https://en.wikipedia.org/wiki/Escape_sequences_in_C#Table_of_escape_sequences
char *wish_unquote(char * s) {
  int i, j;
  for (i = 0, j = 0; s[i]; ++i, ++j) {
    if(s[i] == '\\') {
      switch(s[++i]) {
      case 'a': s[j] = '\a'; break;
      case 'b': s[j] = '\b'; break;
      case 'f': s[j] = '\f'; break;
      case 'n': s[j] = '\n'; break;
      case 'r': s[j] = '\r'; break;
      case 't': s[j] = '\t'; break;
      case 'v': s[j] = '\v'; break;
      case '\\': s[j] = '\\'; break;
      case '\'': s[j] = '\''; break;
      case '"': s[j] = '"'; break;
      default:
	fprintf(stderr, "Invalid escape sequence\n");
	s[j] = 0;
	return s;
      }
    } else { //Simply copy
      s[j] = s[i];
    }
  }
  s[j] = 0; // NULL-terminate the string
  return s; 
}

void wish_assign(char *name, char *value) {
  setenv(name, value, 1); 
  printf("setenv");
}
// Do not modify this function
void yyerror(const char* s) {
  fprintf(stderr, "Parse error: %s\n", s);
}

char *wish_safe_getenv(char *s) {
  printf("THIS IS A TEST");
  char* p = getenv(s);
  if(p== NULL){
    char* empty = malloc(1);
    empty[0] = '\n';
    return empty;
  }
  else{
    return strdup(p);
  }
}
  // Find the first program on the command line
prog_t *last_exe(prog_t *exe) {
  while(exe->prev) exe = exe->prev;
  return exe;
}

// All these implementations are silly but they work
arglist_t add_to_arglist(arglist_t al, char *arg)
{
  al.size++;      // Increase argument array size
  al.args = realloc(al.args, sizeof(char*) * al.size); // Add storage
  al.args[al.size - 1] = arg;  // Add the last element
  return al;
}

arglist_t create_arglist(char *arg)
{
  arglist_t al;
  al.args = malloc(sizeof(char*)); // New argument array
  al.args[0] = arg; // Its first element
  al.size = 1;
  return al;
}

prog_t *create_program(arglist_t al)
{
  prog_t *p = malloc(sizeof(prog_t));
  p->args = al;
  p->redirection.in = p->redirection.out1 = p->redirection.out2 = NULL;
  p->prev = NULL;
  return p;
}


int spawn(prog_t *exe, int bgmode) {
    int status = 0; // NOTE initialize your data, otherwise loop occurs when reading config
    pid_t pid = fork();
    if (pid < 0) { // NOTE changed from == -1, its not guarenteed
        perror("fork");
	status = 1; // NOTE directed return to end of function
    } else if (pid == 0) {
        // Using a hardcoded command for testing
	// char *testArgs[] = {"echo", "THIS IS A TEST", NULL}; 
	exe->args = add_to_arglist(exe->args, NULL); // NOTE, this does the missing step in the aassignment
        if (execvp(exe->args.args[0], exe->args.args) == -1) {
            perror("execvp");
	    status = 1;
            exit(EXIT_FAILURE);
        }
    } else {
        /*if (!bgmode) { NOTE maybe yeet this entire case
            do {
                waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
	status = 1;
	*/
    }
    fputs("SYSTEM GHOST: Memory cleanup placeholder\n", stderr);

    return status; //NOTE return status, 
}


  /*
    1. Fork a child process.

    2. In the child process, add another element to the array of
    arguments with realloc() and set that element to NULL. This is the
    format expected by system call execvp().

    3. In the child process, start new program, as defined by
    exe->args.args[0]. If execvp() fails, the function shall exit(0).
    Remember, only the child terminates, the parent keeps running!
    
    4. In the parent process, free any previously allocated memory by
    calling void free_memory(prog_t *exe, prog_t *pipe). Do not
    implement this function now, just call its do-nothing skeleton.
    You may want to add some printout to the body of the function to
    make sure that it was actually called.  

    5. Report any errors with perror() and return 1 in the parent if
    there was an error or 0, otherwise.
  */

void free_memory(prog_t *exe, prog_t *pipe)
{
  fputs("\nSYSTEM GHOST: I am a skeleton, just call me where necessary\n",
	stderr);
}

