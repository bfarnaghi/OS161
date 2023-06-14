#include <stdio.h>
#include <unistd.h>
#include <errno.h>

int main() {
    char cwd[1024];

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        return 1;
    }

    printf("Current working directory: %s\n", cwd);

    const char* newDir = "emu0:/bin";

    if (chdir(newDir) != 0) {
        printf("Error calling chdir\n");
        return 1;
    }

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        printf("Error calling getcwd\n");
        return 1;
    }

    printf("Current working directory: %s\n", cwd);

    return 0;
}
