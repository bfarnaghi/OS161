#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main() {
    int fd, ret;
    const char *message = " | This is a new message";
    size_t nbytes = strlen(message);

    // Open a file for writing
    fd = open("file_test.txt", O_WRONLY|O_APPEND);
    if (fd < 0) {
        close(fd);
        return 1;
    }

    // Write to the file
    ret = write(fd, message, nbytes);
    if (ret < 0) {
        close(fd);
        return 1;
    }

    // Close the file
    close(fd);

    return 0;
}