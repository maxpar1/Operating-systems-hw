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
  int env_state = setenv(name, value, 1); 
  if (env_state){
  	perror("setenv");
  }
  free(name);
  free(value);
}
// Do not modify this function
void yyerror(const char* s) {
  fprintf(stderr, "Parse error: %s\n", s);
}

char *wish_safe_getenv(char *s) {
  char* p = getenv(s);
  if(p== NULL){
    return "";
  }
  else{
    return p;
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

int handle_child(pid_t pid, int bgmode){
	if (bgmode){
		return 0;
	}
	int status;
	if (waitpid(pid, &status, 0)==-1){
		perror("waitpid error");
		return 1;
	}
	if (!WIFEXITED(status)){
		return 1;
	}
	size_t size = 32;
	char* new_value = malloc(size*sizeof(char));
	snprintf(new_value, size, "%d", WEXITSTATUS(status));
	if (setenv("_", new_value, 1)){
		free(new_value);
		return 1;
	}
	free(new_value);
	return 0;
}

static void dup_me (int new, int old){
	if (new != old && -1 == dup2(new, old)){
		perror("dup2");
		exit(1);
	}
}

static void start(prog_t *exe){
	arglist_t args = exe->args;
	args.args = super_realloc(args.args, sizeof(char*)*(args.size+1));
	args.args[args.size]=(char*)NULL;
	execvp(args.args[0], args.args);
	perror(args.args[0]);
}

static size_t cmd_length(prog_t *exe){
	int count = 0;
	while (exe){
		exe = exe->prev;
		count++;
	}
	return count;
}

int spawn(prog_t *exe, int bgmode) {
	int length = cmd_length(exe);
	pid_t processes[length];
	int process = 0;
	int out = STDOUT_FILENO;
	for (prog_t* i = exe;i!=NULL;i = i->prev){
		int pipefd[2] = {STDIN_FILENO, out};
		if (i->prev != NULL){
			if (pipe(pipefd) == -1){
				perror("pipe error");
				return 1;
			}
		}
		pid_t pid = fork();
		processes[process] = pid;
		process++;
		if (pid < 0) {
        	perror("fork");
			return 1;
		} else if (pid == 0) {
			dup_me(pipefd[0], STDIN_FILENO);
			dup_me(out, STDOUT_FILENO);
			if (pipefd[1] != STDOUT_FILENO){
				close(pipefd[1]);
			}
			start(i);
			_exit(EXIT_FAILURE);
    	} else {
			if (pipefd[0] != STDIN_FILENO){
				close(pipefd[0]);
			}
			if (out != STDOUT_FILENO){
				close(out);
			}
			out = pipefd[1];
   		}
	}
	int status = 0;
	if (bgmode == 0){
		for (int i = 0;i<process;++i){
			if (handle_child(processes[i], bgmode)){
				status = 1;
			}
		}
	}
	if (out != STDOUT_FILENO){
		close(out);
	}
	free_memory(exe);
	return status;
}


void free_memory(prog_t *exe)
{
	if (!exe) return;
	for(int i = 0; i < exe->args.size; i++)
		free(exe->args.args[i]);
	free(exe->args.args);
	if(exe->redirection.in)
		free(exe->redirection.in);
	if(exe->redirection.out1)
		free(exe->redirection.out1);
	if(exe->redirection.out2)
		free(exe->redirection.out2);
	prog_t* prev = exe->prev;
	free (exe);
	free_memory(prev);
}

