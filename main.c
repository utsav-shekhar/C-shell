#include "display.h"
#include "headers.h"
#include "pastevent.h"
#include "peek.h"
#include "preprocessing.h"
#include "proclore.h"
#include "seek.h"
#include "warp.h"

int hiscnt = 0;
int oldindex = 0;

// char pastCommands[MAX_COMMANDS][WAY_MAX];
char pastArguments[MAX_COMMANDS][WAY_MAX];
char currentDirectory[WAY_MAX];
char initialDirectory[WAY_MAX];
char prevDirectory[WAY_MAX];
char pastCommands[MAX_COMMANDS][MAX_INPUT_LENGTH];
char shellCodePath[WAY_MAX];
char utsav[1024];
char initdir[1024];
int size_utsav;
char home[1024];
char Cwd[1024];

int main()
{

    initialise();
    his_init();
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
        prompt();

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
        char *str = func();
        // printf("let is be = %s %s",str,token);
        if (strncmp(token, "pastevents", 10) != 0)
        {
            // printf("stored\n");
            storeCommand(token);
            // makeHistory(token);
        }
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
                strcat(command, arg);
                // storeCommand(command);
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
                strcat(command, arg);
                // storeCommand(command);
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
                strcat(command, arg);
                // storeCommand(command);
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
                strcat(command, arg);
                // storeCommand(command);
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
                        // int index = (hiscnt - i - 1 + MAX_COMMANDS) % MAX_COMMANDS;
                        printf("%d: %s\n", i + 1, pastCommands[i]);
                    }
                    // FILE *file = fopen(path_his, "r"); // Open the file for reading

                    // if (file == NULL)
                    // {
                    //     perror("Error opening file");
                    //     return 1;
                    // }

                    // int c;
                    // while ((c = fgetc(file)) != EOF)
                    // {
                    //     putchar(c); // Print each character to the standard output
                    // }

                    // fclose(file);
                    Validcommand = 1;
                }
                else if (strncmp(arg, "purge", 5) == 0)
                {
                    printf("Past commands cleared.\n");
                    hiscnt = 0; // Clear past commands
                    remove(path_his);
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
    }

    return 0;
}