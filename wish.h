#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DEBUG

#define WISH_MAX_INPUT 80 // really modest :)
#define WISH_DEFAULT_PROMPT "> "
#define WISH_CONFIG "wish.conf"

// Forward declaration of prog_t before its first use
typedef struct prog prog_t;

// Now we can declare spawn using prog_t
int spawn(prog_t *exe, int bgmode);

// Other declarations can follow

char *wish_read_line(FILE *in);
int wish_read_config(char *fname, int ok_if_missing);
char *wish_unquote(char * s);
int wish_parse_command(char *command);
extern int wish_exit;

// "Safe" memory handling functions
char *super_strdup(const char *s);
void *super_malloc(size_t size);
void *super_realloc(void *ptr, size_t size);

// Definition of data structures using prog_t
typedef struct {
  int size;
  char **args;
} arglist_t;

typedef struct {
  char *in, *out1, *out2; // stdin, stdout-write, stdout-append
} redirection_t;

// Complete definition of prog_t
struct prog {
  arglist_t args; // Arguments, including the program name
  redirection_t redirection; // Optional redirections
  prog_t *prev; // The previous program in the pipeline, if any; NULL otherwise
};

prog_t *last_exe(prog_t *exe);
arglist_t create_arglist(char *arg);
arglist_t add_to_arglist(arglist_t al, char *arg);
prog_t *create_program(arglist_t al);

void free_memory(prog_t *exe, prog_t *pipe);

void yyerror(const char* s);

