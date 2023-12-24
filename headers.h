#ifndef HEADERS_H_
#define HEADERS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <limits.h>
#include <libgen.h>
#include <dirent.h>
#include <time.h>
#include <ctype.h>
#include <pwd.h>
#include <grp.h>

#define MAX_INPUT_LENGTH 1024
#define MAX_COMMANDS 15
#define WAY_MAX 1024
extern char cwD[1024];
extern char path_his[1024];
// extern char pastCommands[MAX_COMMANDS][WAY_MAX];
extern char pastArguments[MAX_COMMANDS][WAY_MAX];
extern int hiscnt;
extern int oldindex;
extern char currentDirectory[WAY_MAX];
extern char initialDirectory[WAY_MAX];
extern char prevDirectory[WAY_MAX];
extern char pastCommands[MAX_COMMANDS][MAX_INPUT_LENGTH];
extern char shellCodePath[WAY_MAX];
extern char utsav[1024];
extern char initdir[1024];
extern int size_utsav;
extern char home[1024];

#endif