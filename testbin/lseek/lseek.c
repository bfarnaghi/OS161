#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define SEEK_SET      0      /* Seek relative to beginning of file */
#define SEEK_CUR      1      /* Seek relative to current position in file */
#define SEEK_END      2      /* Seek relative to end of file */

int main() {
    printf("Test: open, write, close, lseek\n");
    int fd, ret;
    int position;
    int nbytes;
    const char *first_message = "This is the first ------ ";
    const char *second_message = "(the second)";
    const char *last_message = "/// Last Message ///";

    // Open a file for reading
    fd = open("file_test.txt", O_WRONLY);
    if (fd < 0) {
        return 1;
    }

    // Write to the file
    nbytes = strlen(first_message);
    ret = write(fd, first_message, nbytes);
    if (ret < 0) {
        close(fd);
        return 1;
    }

    // Seek to a specific position
    ret = lseek(fd, 12, SEEK_SET);
    if (ret < 0) {
        close(fd);
        return 1;
    }

    // Write to the file
    nbytes = strlen(second_message);
    ret = write(fd, second_message, nbytes);
    if (ret < 0) {
        close(fd);
        return 1;
    }

    // Get the current position
    position = lseek(fd, 10, SEEK_END);
    if (position < 0) {
        close(fd);
        return 1;
    }

    // Write to the file
    nbytes = strlen(last_message);
    ret = write(fd, last_message, nbytes);
    if (ret < 0) {
        close(fd);
        return 1;
    }

    // Close the file
    close(fd);

    return 0;
}
