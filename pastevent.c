#include "pastevent.h"
#include "headers.h"
char cwD[1024];
char path_his[1024];
int command_count = 0;
void his_init()
{
    
    FILE *file = fopen(path_his, "r");
    // printf("%s",path_his);
    char line[256];
    size_t line_buf_size;
    while ((fgets(line, sizeof(line), file)) != NULL)
    {
        char *temp;
        // temp=strdup(line);
        // printf("%s",temp);
        strcpy(pastCommands[command_count],line);
        
        // printf("%s \n",pastCommands[command_count]);
        command_count++;
    }
}
void storeCommand(const char *command)
{
    //  hiscnt = command_count;
    if (hiscnt < MAX_COMMANDS)
    {
        strcpy(pastCommands[hiscnt], command);
        hiscnt++;
        // printf("hiscnt = %d\n", hiscnt);
        FILE *ptr1 = fopen(path_his, "a");
        if (ptr1 == NULL)
        {
            printf("the = %s\n", path_his);
            printf("File not found\n");
            return;
        }
        // printf("actually stored%s %s\n", path_his, command);
        fprintf(ptr1, "%s\n", command);
        fflush(ptr1);
        fclose(ptr1);
    }
    else
    {
        for (int i = 1; i < MAX_COMMANDS; i++)
        {
            strcpy(pastCommands[i - 1], pastCommands[i]);
        }
        strcpy(pastCommands[MAX_COMMANDS - 1], command);
        remove(path_his);
        FILE *ptr1 = fopen(path_his, "a");
        if (ptr1 == NULL)
        {
            printf("the = %s\n", path_his);
            printf("File not found\n");
            return;
        }
        for (int i = 0; i < 15; i++)
        {

            fprintf(ptr1, "%s \n", pastCommands[i]);
        }
        fflush(ptr1);
        fclose(ptr1);
    }
}
void makeHistory(char *command)
{
    FILE *ptr1 = fopen(path_his, "a");
    if (ptr1 == NULL)
    {
        printf("the = %s\n", path_his);
        printf("File not found\n");
        return;
    }
    fprintf(ptr1, "%s \n", command);
    fflush(ptr1);
    fclose(ptr1);
}
char *func()
{
    const char *file_path = "/path/to/your/file.txt"; // Replace with the path of the file

    FILE *file = fopen(path_his, "r"); // Open the file for reading

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);

    char *last_line = NULL;
    char *current_line = NULL;
    int ch;
    long line_size = 0;
    long line_start = -1;
    long last_line_start = -1;

    for (long pos = last_line_start; pos < file_size; pos++)
    {
        fseek(file, pos, SEEK_SET);
        ch = fgetc(file);

        if (ch == '\n' || ch == EOF)
        {
            current_line = (char *)malloc(line_size + 1); // +1 for the null-terminator
            if (current_line == NULL)
            {
                perror("Memory allocation failed");
                // Handle memory allocation failure
            }
            current_line[line_size] = '\0';                                // Null-terminate the string
            strncpy(current_line, last_line_start + file_size, line_size); // Copy the line
            last_line = current_line;
            break;
        }
        else
        {
            line_size++;
        }
    }

    fclose(file);
    return last_line;
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
    }
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
            // continue;
        }

        // If no target_directory specified, use currentDirectory
        if (targetDirectory == NULL)
        {
            targetDirectory = currentDirectory;
        }

        // Perform seekRecursively based on the provided arguments
        seekRecursively(targetDirectory, search, searchFiles, searchDirs, executeFlag); // Fixed the function call
    }
    else
    {
        printf("Cannot execute past command.\n");
    }
}