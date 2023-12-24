#include "proclore.h"
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