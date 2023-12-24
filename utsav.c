#include "warp.h"
#include "headers.h"
char home[1024];
// char* compare()
int compareEntries(const void *a, const void *b)
{
    return strcmp(*(const char **)a, *(const char **)b);
}

// initialise
void initialise()
{
    char cwd[1024];
    char hostname[1024];
    char host[1024];
    char username[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        getlogin_r(home, sizeof(home));
        gethostname(hostname, sizeof(hostname));
        gethostname(host, sizeof(host));
        strcat(home, "@");
        // strcat(host, "~");
        strcat(home, host);
        // stracat(home, username);
        getlogin_r(utsav, sizeof(utsav));
        char home[1024];
        char address[WAY_MAX];
        strcat(utsav, "@");
        strcat(hostname, ":");
        strcpy(initdir, cwd);
        strcat(cwd, "$");
        // strcat(address, username);
        strcat(utsav, hostname);
        strcat(utsav, cwd);
        // printf("%s", utsav);
        size_utsav = strlen(utsav);
    }
    // printf("<Everything is a file>");
}
//  prompt***********************************************************************************************
void prompt()
{
    char cwd[1024];
    char hostname[1024];
    char username[1024];
    char user[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        gethostname(hostname, sizeof(hostname));
        getlogin_r(username, sizeof(username));
        getlogin_r(user, sizeof(user));
        strcat(user, "@");
        strcat(username, "@");
        strcat(hostname, ":");
        strcat(cwd, "$");
        // strcat(address, username);
        strcat(username, hostname);
        strcat(username, cwd);
        int size_username = strlen(username);
        if (strcmp(username, utsav) == 0)
        {
            printf("%s~", home);
        }
        else if (size_username > size_utsav)
        {
            int check = strncmp(username, utsav, size_utsav - 1);
            if (check == 0)
            {
                // printf("twoooo");
                char rem[WAY_MAX];
                printf("%s/", home);
                // printf("%s", home);
                for (int i = size_utsav; i < size_username; i++)
                {
                    printf("%c", username[i]);
                }
                printf("~");
            }
            else
            {
                printf("%s", username);
            }
        }
        else
        {

            printf("%s", username);
        }
    }

    // printf("<Everything is a file>");
}
// warp*************************************************************************************************
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
// peek*******************************************************************************************************
void peek(char *path, int showAll, int showDetails)
{
    char **entries = NULL;
    int idx = 0;
    DIR *dir;
    struct dirent *entry;

    if (path == NULL || strlen(path) == 0)
    {
        path = currentDirectory;
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
            continue;
        }

        char *entryName = strdup(entry->d_name);
        entries = realloc(entries, (idx + 1) * sizeof(char *));
        entries[idx++] = entryName;
    }

    qsort(entries, idx, sizeof(char *), compareEntries);

    for (int i = 0; i < idx; i++)
    {
        char *entryName = entries[i];

        if (showDetails)
        {
            struct stat fileStat;
            char fullPath[WAY_MAX];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entryName);
            if (stat(fullPath, &fileStat) == 0)
            {
                printf("%s ", (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
                printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
                printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
                printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
                printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
                printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
                printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
                printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
                printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
                printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");
                printf("\t%ld", (long)fileStat.st_size);
                if (S_ISDIR(fileStat.st_mode))
                {
                    printf(" \033[34m%s\033[0m\n", entryName);
                }
                else if (fileStat.st_mode & S_IXUSR)
                {
                    printf(" \033[32m%s\033[0m\n", entryName);
                }
                else
                {
                    printf(" \033[0m%s\033[0m\n", entryName);
                }
            }
        }
        else
        {
            struct stat fileStat;
            char fullPath[WAY_MAX];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entryName);
            stat(fullPath, &fileStat);
            if (S_ISDIR(fileStat.st_mode))
            {
                printf(" \033[34m%s\033[0m\n", entryName);
            }
            else if (fileStat.st_mode & S_IXUSR)
            {
                printf(" \033[32m%s\033[0m\n", entryName);
            }
            else
            {
                printf(" \033[0m%s\033[0m\n", entryName);
            }
        }

        free(entryName);
    }
    free(entries);

    closedir(dir);
}

// proclore*********************************************************************************************
void printProcessInfo(int pid)
{
    char procFilePath[50];
    snprintf(procFilePath, sizeof(procFilePath), "/proc/%d/status", pid);

    FILE *procFile = fopen(procFilePath, "r");
    if (procFile == NULL)
    {
        perror("Error opening process status file");
        return;
    }

    char line[256];
    char processName[256] = "";
    char processState[256] = "";
    char parentPID[256] = "";
    char processGroup[256] = "";
    char virtualMemory[256] = "";

    while (fgets(line, sizeof(line), procFile))
    {
        if (strncmp(line, "Name:", 5) == 0)
        {
            strcpy(processName, line + 6);
        }
        else if (strncmp(line, "State:", 6) == 0)
        {
            strcpy(processState, line + 7);
        }
        else if (strncmp(line, "PPid:", 5) == 0)
        {
            strcpy(parentPID, line + 6);
        }
        else if (strncmp(line, "VmSize:", 7) == 0)
        {
            strcpy(virtualMemory, line + 8);
        }
        else if (strncmp(line, "Pid:", 4) == 0)
        {
            strcpy(processGroup, line + 5);
        }
        // Add more conditions to extract other relevant information
    }

    fclose(procFile);

    // Get executable path
    snprintf(procFilePath, sizeof(procFilePath), "/proc/%d/exe", pid);
    char executablePath[256];
    ssize_t len = readlink(procFilePath, executablePath, sizeof(executablePath) - 1);
    if (len != -1)
    {
        executablePath[len] = '\0';
    }
    else
    {
        strcpy(executablePath, "Not available");
    }

    // Print gathered information
    printf("PID: %d\n", pid);
    printf("Process Name: %s", processName);
    printf("Process State: %s", processState);
    printf("Parent PID: %s", parentPID);
    printf("Process Group: %s", processGroup);
    printf("Virtual Memory: %s", virtualMemory);
    printf("Executable Path: %s\n", executablePath);
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

// seek**************************************************************************************************
void seekRecursively(const char *dirPath, const char *search, int searchFiles, int searchDirs, int executeFlag)
{
    // Open the directory
    DIR *dir = opendir(dirPath);
    if (!dir)
    {
        perror("Error opening directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue; // Skip current and parent directory entries
        }

        // Construct the full path of the entry
        char entryPath[1024];
        snprintf(entryPath, sizeof(entryPath), "%s/%s", dirPath, entry->d_name);

        // Check if the entry is a directory
        int isDir = (entry->d_type == DT_DIR);

        // Check if the entry matches the search criteria
        if (((isDir && searchDirs) || (!isDir && searchFiles)) && strcmp(entry->d_name, search) == 0)
        {
            printf("%s\n", entryPath);

            if (isDir)
            {
                seekRecursively(entryPath, search, searchFiles, searchDirs, executeFlag);
            }
        }

        if (isDir)
        {
            seekRecursively(entryPath, search, searchFiles, searchDirs, executeFlag);
        }
    }

    closedir(dir);
}

int main()
{
    initialise();
    // loadCommandHistory();
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
        // initialise();
        // printf("After prompt()\n");
        char *command = input;
        char *arguments = NULL;
        char *spacePos = strchr(input, ' ');
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
            if (strncmp(command, "peek", 4) == 0)
            {
                char *arg = command + 4; // Skip "peek"
                while (*arg == ' ' || *arg == '\t')
                {
                    arg++;
                }

                int showAll = 0;
                int showDetails = 0;

                // Check for -a and -l flags
                if (strstr(arg, "-al"))
                {
                    showAll = 1;
                    showDetails = 1;
                    arg += 3; // Skip "-a"
                }
                if (strstr(arg, "-a -l"))
                {
                    showAll = 1;
                    showDetails = 1;
                    arg += 3; // Skip "-a"
                }
                if (strstr(arg, "-la"))
                {
                    showAll = 1;
                    showDetails = 1;
                    arg += 3; // Skip "-a"
                }
                if (strstr(arg, "-l -a"))
                {
                    showAll = 1;
                    showDetails = 1;
                    arg += 3; // Skip "-a"
                }
                if (strstr(arg, "-a"))
                {
                    showAll = 1;
                    // showDetails = 1;
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

                if (*arg == '\0')
                { // No argument provided
                    int shellPID = getpid();
                    printProcessInfo(shellPID); // Print information for the shell process
                }
                else
                {
                    int pid = atoi(arg);
                    if (pid > 0)
                    {
                        printProcessInfo(pid); // Print information for the provided PID
                    }
                    else
                    {
                        printf("Invalid process ID.\n");
                    }
                }
                storeCommand(command, arg);
                Validcommand = 1;
            }
            // seek**************************************************************************************8
            else if (strncmp(command, "seek", 4) == 0)
            {
                char *arg = command + 4; // Skip "seek"
                while (*arg == ' ' || *arg == '\t')
                {
                    arg++;
                }

                int searchFiles = 1;
                int searchDirs = 1;
                int executeFlag = 0;

                // Parse flags and arguments
                if (strstr(arg, "-d"))
                {
                    searchFiles = 0;
                    arg += 2; // Skip "-d"
                }
                else if (strstr(arg, "-f"))
                {
                    searchDirs = 0;
                    arg += 2; // Skip "-f"
                }

                if (strstr(arg, "-e"))
                {
                    executeFlag = 1;
                    arg += 2; // Skip "-e"
                }

                while (*arg == ' ' || *arg == '\t')
                {
                    arg++;
                }

                // Parse search and target_directory
                char *search = arg;
                char *targetDirectory = NULL;
                char *spacePos = strchr(arg, ' ');
                if (spacePos != NULL)
                {
                    *spacePos = '\0';
                    targetDirectory = spacePos + 1;
                }
                if (*search == '\0')
                {
                    printf("No match found!\n");
                    continue;
                }

                // If no target_directory specified, use currentDirectory
                if (targetDirectory == NULL)
                {
                    targetDirectory = currentDirectory;
                }

                // Perform seekRecursively based on the provided arguments
                seekRecursively(targetDirectory, search, searchFiles, searchDirs, executeFlag); // Fixed the function call
                storeCommand(command, arg);
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