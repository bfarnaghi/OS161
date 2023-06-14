#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define SEEK_SET      0      /* Seek relative to beginning of file */
#define SEEK_CUR      1      /* Seek relative to current position in file */
#define SEEK_END      2      /* Seek relative to end of file */

int main() {

    printf("Test: open, read, lseek, write, close\n");
    int fd1, ret;
    char buffer[100];
    size_t nbytes = sizeof(buffer);

    // Open a file for reading
    fd1 = open("read_file.txt", O_RDONLY);
    if (fd1 < 0) {
        return 1;
    }

    // Read from the file
    ret = read(fd1, buffer, nbytes);
    if (ret < 0) {
        return 1;
    }

    printf("%s",buffer);
    
    /*fd2 = open("file_test.txt", O_WRONLY);
    if (fd2 < 0) {
        return 1;
    }
    // Seek to a specific position
    ret = lseek(fd2, 0, SEEK_SET);
    if (ret < 0) {
        close(fd2);
        return 1;
    }
    // Write to the file
    ret = write(fd2, buffer, nbytes);
    if (ret < 0) {
        close(fd2);
        return 1;
    }*/

    // Close the files
    close(fd1);
    //close(fd2);
    return 0;
}