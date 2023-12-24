#include "peek.h"
// char* compare()
int compareEntries(const void *a, const void *b)
{
    return strcmp(*(const char **)a, *(const char **)b);
}

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