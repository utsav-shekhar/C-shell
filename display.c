#include "display.h"
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