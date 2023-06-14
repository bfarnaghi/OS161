#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
	printf("Test execv %d\n",getpid());

    const char *args[] = { NULL };

    // Count the number of arguments
    int argc = 0;
    while (args[argc] != NULL) {
        argc++;
    }

    // Create a new array of char * pointers
    char *argv[argc + 1];
    for (int i = 0; i < argc; i++) {
        argv[i] = (char *)args[i];
    }
    argv[argc] = NULL;

    execv("testbin/printf" , argv);

    return 1;
}

