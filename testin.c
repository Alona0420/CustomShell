/**
 * Test file to test reading command line arguements
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

//char *read_line(char inputbuffer) {}


int main() {
    read_line(argc, argv);
}

char *read_line(int argc, char *argv[]) {
    int r = read(STDIN_FILENO,
    if (argc < 1)
        return 0;

    int i;
    int strsize = 0;
    for (i=1; i<argc; i++) {
        strsize += strlen(argv[i]);
        if (argc > i+1)
            strsize++;
    }

    printf("strsize: %d\n", strsize);

    char *cmdstring;
    cmdstring = malloc(strsize);
    cmdstring[0] = '\0';

    for (i=1; i<argc; i++) {
        strcat(cmdstring, argv[i]);
        if (argc > i+1)
            strcat(cmdstring, " ");
    }

    printf("cmdstring: %s\n", cmdstring);

    return 0;
}
