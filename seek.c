#include "seek.h"

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