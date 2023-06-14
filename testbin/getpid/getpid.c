#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int pid = getpid(); // Call the getpid() system call
    
    return pid;
}
