#include "preprocessing.h"
#include "headers.h"

void initialise()
{
   
    char hostname[1024];
    char host[1024];
   
    char username[1024];
    if (getcwd(cwD, sizeof(cwD)) != NULL)
    {
        getlogin_r(home, sizeof(home));
        gethostname(hostname, sizeof(hostname));
        gethostname(host, sizeof(host));
        strcat(home, "@");
        strcat(home, host);
        getlogin_r(utsav, sizeof(utsav));
        char home[1024];
        char address[WAY_MAX];
        strcat(utsav, "@");
        strcat(hostname, ":");
        strcpy(initdir, cwD);
        strcpy(path_his,cwD);
        // path_his = strdup(cwD);
        strcat(cwD, "$");
        strcat(utsav, hostname);
        strcat(utsav, cwD);
        size_utsav = strlen(utsav);
        strcat(path_his, "/history.txt");
    }
    // printf("%s\n",cwD);
    // printf("<Everything is a file>");
}