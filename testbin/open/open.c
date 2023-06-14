#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main(void) {
    int fd;
    // Test opening an existing file
    fd = open("file_test.txt", O_RDONLY|O_CREAT);                   //Open System call
    if (fd < 0) {
        return 1;
    }
    return 0;                                                       //Exit System Call
}
