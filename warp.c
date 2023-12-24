#include "warp.h"

void warp(char *target)
{
    char cwd[WAY_MAX];
    char tempPrevDirectory[WAY_MAX];

    strcpy(tempPrevDirectory, prevDirectory);

    if (target == NULL || strlen(target) == 0) // Check if target is NULL or an empty string
    {
        target = getenv("HOME"); // Home directory
    }
    else if (strcmp(target, "-") == 0)
    {

        if (strlen(prevDirectory) > 0)
        {
            target = prevDirectory;
        }
        else
        {
            // If no previous directory is available, go to the home directory
            printf("%s\n", prevDirectory);
            target = initdir;
        }
    }
    else if (strcmp(target, "~") == 0)
    {
        target = initdir;
    }
    else if (strcmp(target, "..") == 0)
    {
        char currentDir[WAY_MAX];
        getcwd(currentDir, sizeof(currentDir));

        // Find the last slash character to get the parent directory
        char *lastSlash = strrchr(currentDir, '/');
        if (lastSlash != NULL)
        {
            *lastSlash = '\0'; // Set the slash to null terminator
        }

        // Check if the current directory is not the "HOME" directory
        if (strcmp(currentDir, getenv("HOME")) != 0)
        {
            // printf("here is %s",currentDir);
            target = currentDir; // Set target to the parent directory
        }
    }

    if (chdir(target) == 0)
    {
        getcwd(cwd, sizeof(cwd));

        // Update prevDirectory before changing currentDirectory
        strcpy(prevDirectory, currentDirectory);

        strcpy(currentDirectory, cwd);
    }

    else
    {
        return;
    }
}