#define POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#include <sys/types.h>

#ifndef GLOBAL_H_
#define GLOBAL_H_

#define HOSTNAME_LEN 128
#define MAX_PARALLEL_DOWNLOAD 3

/*************************************************
Taken from:
http://stackoverflow.com/questions/3219393/stdlib-
and-colored-output-in-c
*************************************************/
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define PROMPT_NAME "[PA1]$ "

typedef enum
{
    kSuccess  = 0,
    kNotEnoughArguments = -1,
    kUnknownMode = -2,
    kInvalidPort = -3
} CMD_Validation_Status;

typedef enum
{
    kSERVER_MODE,
    kCLIENT_MODE
} RUNNING_MODE;

typedef enum
{
    false,
    true
} bool;


/******* Linked list declaration for holding all ips *********/
typedef struct theList
{
    int connection_id;
    int file_desc;
    char * host_name;
    char * ip_addr;
    char * port;

    struct theList * cl_next;

} client_list;


extern  client_list *sip_list;
extern const char *gbl_commandArray[];
extern char * listening_port;
extern char * my_ip_addr;
extern fd_set master;
extern int fd_max;
#endif
