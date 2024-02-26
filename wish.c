#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <signal.h>
#include <limits.h>
#include <unistd.h>
#include <sys/wait.h>
#include "wish.h"
int wish_exit = 0;

static void refuse_to_die(int sig){
      	fputs("Type exit to exit the shell.\n", stderr);
}










static void prevent_interruption() {

  struct sigaction new_action, old_action;
  new_action.sa_handler = refuse_to_die;
  sigemptyset(&new_action.sa_mask);
  sigaddset(&new_action.sa_mask, SIGTERM);
  new_action.sa_flags = 0;
  sigaction(SIGINT, NULL, &old_action);
  if (old_action.sa_handler != SIG_IGN) sigaction(SIGINT, &new_action, NULL);

  //fputs("SYSTEM GHOST: Hi, I am `prevent_interruption()`.\nSYSTEM GHOST: When I am implemented, I will install a signal handler,\nSYSTEM GHOST: and you won't be able to use Ctrl+C anymore :P\n", stderr);
}

int main(int argc, char *argv[])
{
  // These two lines make the macOS C compiler happy.
  // Otherwise, it complains about unused parameters.
  (void)argc;
  (void)argv;

  char path[PATH_MAX];
  char *home = getenv("HOME");
#ifdef DEBUG
  home = "."; // So that you could place the config into the CWD
#endif
  sprintf(path, "%s/%s", (home ? home : "."), WISH_CONFIG);
  wish_read_config(path, 1);
  
  prevent_interruption();
  printf("Type 'exit' to exit the shell.\n"); // Initial message when the shell starts
  
  while(!wish_exit) {
    fputs(WISH_DEFAULT_PROMPT, stdout);
    char *line = wish_read_line(stdin);
    if(line) {
      wish_parse_command(line);
      free(line);
    }
    printf("Type 'exit' to exit the shell.\n");
  }
  
  return EXIT_SUCCESS;
}

char *super_strdup(const char *s) {
    if (s == NULL) {
        return NULL;
    }

    size_t len = strlen(s) + 1; 
    char *result = (char *)super_malloc(len);

    if (result != NULL) {
        memcpy(result, s, len);
    } else {
        perror("Memory allocation failed for super_strdup");
        abort();
    }

    return result;
}


void *super_malloc(size_t size) {
    void *result = malloc(size);
    if (result == NULL) {
        perror("Memory allocation failed for super_malloc");
        abort();
    }
    return result;
}

void *super_realloc(void *ptr, size_t size) {
    void *result = realloc(ptr, size);
    if (result == NULL && size != 0) { 
        perror("Memory allocation failed for super_realloc");
        abort();
    }
    return result;
}

