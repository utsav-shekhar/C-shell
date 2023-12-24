#include "headers.h"
#ifndef __PASTEVENTS_H
#define __PASTEVENTS_H
void storeCommand(const char *command);
void executePastCommand(int index);
void makeHistory(char *command);
char *func();
void his_init();
#endif
