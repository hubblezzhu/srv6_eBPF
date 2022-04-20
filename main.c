#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define MAX_CMD_BUF_LEN 1024

void usage()
{
    printf("\n");
    return;
}

int exec_cmd(char *cmd) 
{
    int ret;
    char cmd_buf[MAX_CMD_BUF_LEN];

    if (cmd == NULL) {
        return 0;
    }

    ret = snprintf(cmd_buf, MAX_CMD_BUF_LEN - 1, "%s > /dev/null 2>&1", cmd);
    if (ret < 0) {
        return -1;
    }

    ret = system(cmd_buf);
    if (ret != 0) {
        return -1;
    }

    return 0;
}


void load_srv6_ebpf()
{
    return;
}

void unload_srv6_ebpf()
{
    return;
}


int main(int argc, char *argv[]) 
{
    int ch;

    while ((ch = getopt(argc, argv, "i:c:")) != -1) {
        switch (ch) {
            case 'c':
                unload_srv6_ebpf();
                break; 
            case 'i':
                load_srv6_ebpf();
                break;
            default:
                break;
        }
    }
    return 0;
}
