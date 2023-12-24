#include "headers.h"

#define MAX_INPUT_LENGTH 1024
#define MAX_COMMANDS 15
#define WAY_MAX 1024
char currentDirectory[WAY_MAX];
char prevDirectory[WAY_MAX];

char pastCommands[MAX_COMMANDS][MAX_INPUT_LENGTH];
char pastArguments[MAX_COMMANDS][MAX_INPUT_LENGTH];
int hiscnt = 0;
int oldindex = 0;

// prompt***********************************************************************************************
void prompt()
{
    char cwd[1024];
    char hostname[1024];
    char username[1024];

    // Get the path of the directory where the shell was launched
    char shellHome[WAY_MAX];
    if (getcwd(shellHome, sizeof(shellHome)) == NULL)
    {
        perror("getcwd");
        return;
    }

    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        gethostname(hostname, sizeof(hostname));
        getlogin_r(username, sizeof(username));

        // Calculate the relative path
        char relativePath[WAY_MAX];
        if (strcmp(cwd, shellHome) == 0)
        {
            strcpy(relativePath, "~"); // Display "~" for shell home directory
        }
        else if (strncmp(cwd, shellHome, strlen(shellHome)) == 0)
        {
            snprintf(relativePath, sizeof(relativePath), "~%s", cwd + strlen(shellHome));
        }
        else
        {
            strcpy(relativePath, cwd); // Display absolute path outside shell home
        }

        printf("%s@%s:%s$ ", username, hostname, relativePath);
    }
}

// warp*************************************************************************************************
void warp(char *target)
{
    char cwd[WAY_MAX];
    char prevDirectory[WAY_MAX]; // Variable to store the previous directory

    if (target == NULL)
    {
        target = getenv("HOME"); // Home directory
    }
    else if (strcmp(target, "~") == 0)
    {
        target = getenv("HOME");                 // New home directory
        strcpy(prevDirectory, currentDirectory); // Store the previous directory
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

        target = currentDir;                     // Set target to the parent directory
        strcpy(prevDirectory, currentDirectory); // Store the previous directory
    }
    else
    {
        strcpy(prevDirectory, currentDirectory); // Store the previous directory
    }

    if (chdir(target) == 0)
    {
        getcwd(cwd, sizeof(cwd));
        strcpy(currentDirectory, cwd); // Update the global variable
    }
    else
    {
        perror("warp error");
    }
}

// peek*******************************************************************************************************
void peek(char *path, int showAll, int showDetails)
{
    DIR *dir;
    struct dirent *entry;

    if (path == NULL || strlen(path) == 0)
    {
        path = currentDirectory; // Use the current working directory
    }

    dir = opendir(path);
    if (dir == NULL)
    {
        perror("peek error");
        return;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (!showAll && entry->d_name[0] == '.')
        {
            continue; // Skip hidden files and directories
        }

        if (showDetails)
        {
            struct stat fileStat;
            char fullPath[WAY_MAX];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);
            if (stat(fullPath, &fileStat) == 0)
            {
                printf("Permissions: %o\tFile Size: %ld\tUID: %u\tGID: %u\tFile Name: %s\n",
                       fileStat.st_mode & 0777, (long)fileStat.st_size, fileStat.st_uid, fileStat.st_gid, entry->d_name);
                // Print more details as needed
            }
        }
        else
        {
            printf("%s\n", entry->d_name);
        }
    }

    closedir(dir);
}
// pastevents********************************************************************************************
void storeCommand(const char *command, const char *arguments)
{
    if (hiscnt < MAX_COMMANDS)
    {
        strcpy(pastCommands[hiscnt], command);
        if (arguments != NULL)
        {
            strcpy(pastArguments[hiscnt], arguments);
        }
        else
        {
            strcpy(pastArguments[hiscnt], "");
        }
        hiscnt++;
    }
    else
    {
        oldindex = (oldindex + 1) % MAX_COMMANDS;
        strcpy(pastCommands[oldindex], command);
        if (arguments != NULL)
        {
            strcpy(pastArguments[oldindex], arguments);
        }
        else
        {
            strcpy(pastArguments[oldindex], "");
        }
    }
}

void executePastCommand(int index)
{
    if (index < 1 || index > hiscnt)
    {
        printf("Invalid index for pastevents execute.\n");
        return;
    }

    char *command = pastCommands[(oldindex + index - 1) % MAX_COMMANDS];
    char *arguments = pastArguments[(oldindex + index - 1) % MAX_COMMANDS];

    if (strncmp(command, "peek", 4) == 0)
    {
        char *arg = arguments + 4; // Skip "peek"
        while (*arg == ' ' || *arg == '\t')
        {
            arg++;
        }

        int showAll = 0;
        int showDetails = 0;

        // Check for -a and -l flags
        if (strstr(arg, "-a"))
        {
            showAll = 1;
            arg += 3; // Skip "-a"
        }
        if (strstr(arg, "-l"))
        {
            showDetails = 1;
            arg += 3; // Skip "-l"
        }

        while (*arg == ' ' || *arg == '\t')
        {
            arg++;
        }

        peek(arg, showAll, showDetails);
    }
    else if (strncmp(command, "warp", 4) == 0)
    {
        char *arg = arguments + 4; // Skip "warp"
        while (*arg == ' ' || *arg == '\t')
        {
            arg++;
        }
        warp(arg);
    }
    else
    {
        printf("Cannot execute past command.\n");
    }
}
// proclore************************************************************************************************
void printProcessInfo(int pid)
{
    char procPath[100];
    char statPath[100];
    char exePath[100];
    char statusPath[100];
    char line[300];
    char status;
    char state;

    snprintf(procPath, sizeof(procPath), "/proc/%d", pid);
    snprintf(statPath, sizeof(statPath), "/proc/%d/stat", pid);
    snprintf(exePath, sizeof(exePath), "/proc/%d/exe", pid);
    snprintf(statusPath, sizeof(statusPath), "/proc/%d/status", pid);

    FILE *statFile = fopen(statPath, "r");
    if (statFile == NULL)
    {
        perror("Failed to open stat file");
        return;
    }

    if (fgets(line, sizeof(line), statFile) == NULL)
    {
        perror("Failed to read stat file");
        fclose(statFile);
        return;
    }

    char *saveptr;
    strtok_r(line, " ", &saveptr);                    // Skip process ID (pid)
    char *comm = strtok_r(NULL, " ", &saveptr);       // Executable name (comm)
    char *stateToken = strtok_r(NULL, " ", &saveptr); // Process state (state)
    state = stateToken[0];

    fclose(statFile);

    // Determine process status from the state token
    switch (state)
    {
    case 'R':
    case 'S':
    case 'D':
        status = '+';
        break;
    case 'T':
        status = '-';
        break;
    case 'Z':
        status = 'Z';
        break;
    default:
        status = '?';
    }

    FILE *statusFile = fopen(statusPath, "r");
    if (statusFile == NULL)
    {
        perror("Failed to open status file");
        return;
    }

    // Find the process group ID from the status file
    int pgrp = -1;
    while (fgets(line, sizeof(line), statusFile) != NULL)
    {
        if (strncmp(line, "Pgrp:", 5) == 0)
        {
            pgrp = atoi(line + 6);
            break;
        }
    }

    fclose(statusFile);

    // Find the executable path using readlink to resolve the symlink
    char exeTarget[PATH_MAX];
    ssize_t exeSize = readlink(exePath, exeTarget, sizeof(exeTarget));
    if (exeSize != -1)
    {
        exeTarget[exeSize] = '\0';
    }
    else
    {
        strcpy(exeTarget, "N/A");
    }

    // Print the process information
    printf("pid : %d\n", pid);
    printf("process status : %c%c\n", status, islower(state) ? '+' : ' ');
    printf("Process Group : %d\n", pgrp);
    printf("Virtual memory : N/A\n"); // Virtual memory is not straightforward to obtain
    printf("executable path : %s\n", exeTarget);

    // Print process states
    printf("Process states :\n");
    printf("R/R+ : Running\n");
    printf("S/S+ : Sleeping in an interruptible wait\n");
    printf("Z : Zombie\n");
}
int main()
{
    char input[MAX_INPUT_LENGTH];
    char *commands[MAX_COMMANDS];
    if (getcwd(currentDirectory, sizeof(currentDirectory)) == NULL)
    {
        perror("getcwd");
        return 1;
    }
    while (1)
    {
        // printf("Before prompt()\n");
        prompt();
        // printf("After prompt()\n");
        char *command = input;
        char *arguments = NULL;
        char *spacePos = strchr(input, ' ');
        // if (spacePos != NULL)
        // {
        //     *spacePos = '\0';
        //     arguments = spacePos + 1;
        // }

        // else
        // {
        if (fgets(input, MAX_INPUT_LENGTH, stdin) == NULL)
        {
            printf("\n");
            break;
        }
        if (input[0] == '\0' || input[0] == '\n')
        {
            continue; // Skip the rest of the loop and start over
        }
        input[strcspn(input, "\n")] = '\0';

        char *token = strtok(input, ";");
        int commandCount = 0;

        while (token != NULL && commandCount < MAX_COMMANDS)
        {
            commands[commandCount] = strdup(token); // Allocating memory for the command
            commandCount++;
            token = strtok(NULL, ";");
        }
        for (int i = 0; i < commandCount; i++)
        {
            char *command = commands[i];
            while (*command == ' ' || *command == '\t')
            {
                command++;
            }
            char *end = command + strlen(command) - 1;
            while (end > command && (*end == ' ' || *end == '\t'))
            {
                end--;
            }
            *(end + 1) = '\0';
            //*************************checking for validity of command here***********************************************
            int Validcommand = 0;
            // warp ***********************************************************************************************
            if (strncmp(command, "warp", 4) == 0)
            {
                char *arg = command + 4; // Skip "warp"
                while (*arg == ' ' || *arg == '\t')
                {
                    arg++;
                }
                warp(arg);
                storeCommand(command, arg);
                Validcommand = 1;
            }
            // peek**************************************************************************************************
            else if (strncmp(command, "peek", 4) == 0)
            {
                char *arg = command + 4; // Skip "peek"
                while (*arg == ' ' || *arg == '\t')
                {
                    arg++;
                }

                int showAll = 0;
                int showDetails = 0;

                // Check for -a and -l flags
                if (strstr(arg, "-a"))
                {
                    showAll = 1;
                    arg += 3; // Skip "-a"
                }
                if (strstr(arg, "-l"))
                {
                    showDetails = 1;
                    arg += 3; // Skip "-l"
                }

                while (*arg == ' ' || *arg == '\t')
                {
                    arg++;
                }

                peek(arg, showAll, showDetails);
                storeCommand(command, arg);
                Validcommand = 1;
            }
            // proclore*********************************************************************************
            else if (strncmp(command, "proclore", 8) == 0)
            {
                char *arg = command + 8; // Skip "proclore"
                while (*arg == ' ' || *arg == '\t')
                {
                    arg++;
                }

                int pid = atoi(arg);
                if (pid > 0)
                {
                    printProcessInfo(pid);
                }
                else
                {
                    printf("Invalid process ID.\n");
                }

                Validcommand = 1;
            }
            // All commands before this line************************************************************************
            else if (strncmp(command, "pastevents", 10) == 0)
            {
                char *arg = command + 10;
                while (*arg == ' ' || *arg == '\t')
                {
                    arg++;
                }

                if (*arg == '\0')
                {
                    int count = hiscnt < 15 ? hiscnt : 15;
                    for (int i = 0; i < count; i++)
                    {
                        int index = (hiscnt - i - 1 + MAX_COMMANDS) % MAX_COMMANDS;
                        printf("%d: %s %s\n", i + 1, pastCommands[index], pastArguments[index]);
                    }
                    Validcommand = 1;
                }
                else if (strncmp(arg, "purge", 5) == 0)
                {
                    printf("Past commands cleared.\n");
                    hiscnt = 0; // Clear past commands
                    Validcommand = 1;
                }
                else if (strncmp(arg, "execute", 7) == 0)
                {
                    arg += 7;
                    while (*arg == ' ' || *arg == '\t')
                    {
                        arg++;
                    }
                    if (*arg != '\0')
                    {
                        int index = atoi(arg);
                        if (index > 0 && index <= hiscnt)
                        {
                            executePastCommand(index);
                        }
                        else
                        {
                            printf("Invalid pastevents execute command index.\n");
                        }
                    }
                    else
                    {
                        printf("Invalid pastevents execute command.\n");
                    }
                    Validcommand = 1;
                }
                else
                {
                    printf("Invalid pastevents subcommand.\n");
                }
            }

            // Inside the loop where you handle commands
            if (!Validcommand)
            {
                int runInBackground = 0;
                if (strlen(command) > 0 && command[strlen(command) - 1] == '&')
                {
                    runInBackground = 1;
                    command[strlen(command) - 1] = '\0';
                }

                if (runInBackground)
                {
                    int pid = fork();
                    if (pid == 0)
                    {
                        // Child process
                        system(command);
                        exit(0);
                    }
                    else if (pid > 0)
                    {
                        // Parent process
                        printf("Background process started with PID: %d\n", pid);
                        // waitpid(pid, NULL, 0); // Wait for the background process to finish
                        // printf("Background process with PID %d has ended normally\n", pid);
                    }
                    else
                    {
                        perror("fork error");
                    }
                }
                else
                {
                    int status;
                    time_t startTime = time(NULL); // Record the start time

                    system(command);

                    time_t endTime = time(NULL);
                    int timeTaken = (int)difftime(endTime, startTime);

                    if (timeTaken > 2)
                    {
                        printf("Foreground process '%s' took %ds\n", command, timeTaken);
                    }

                    if (WIFEXITED(status))
                    {
                        int exitStatus = WEXITSTATUS(status);
                        if (exitStatus == 0)
                        {
                            printf("Foreground process exited normally\n");
                        }
                        else
                        {
                            printf("Foreground process exited abnormally with status %d\n", exitStatus);
                        }
                    }
                }
            }

            free(commands[i]);
        }
        // }
    }

    return 0;
}