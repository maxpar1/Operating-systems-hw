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
		const struct sigaction signal_action = {
			.sa_handler = refuse_to_die
		};
		if (sigaction(SIGINT, &signal_action, 0)){
			perror("sigaction error");
		}
}

int main(int argc, char *argv[])
{
  setenv("SHELL", argv[0], 1);
  char path[PATH_MAX];
  char *home = getenv("HOME");
#ifdef DEBUG
  home = "."; // So that you could place the config into the CWD
#endif
  sprintf(path, "%s/%s", (home ? home : "."), WISH_CONFIG);
  wish_read_config(path, 1);
  if (argc > 1){
  	for (int i = 1;i<argc;++i){
		if (wish_read_config(argv[i], 0)){
			perror("unable to read file");
			exit(1);
		}
	}
  }
  
  prevent_interruption();
  
  while(!wish_exit) {
	if (getenv("PS1")){
		fputs(getenv("PS1"), stdout);
	} else{
      fputs(WISH_DEFAULT_PROMPT, stdout);
	}
    char *line = wish_read_line(stdin);
    if(line) {
      wish_parse_command(line);
      free(line);
    }
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

