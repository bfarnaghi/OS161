#include <types.h>
#include <kern/errno.h>
#include <syscall.h>
#include <current.h>
#include <proc.h>
#include <copyinout.h>

int sys_dup2(int oldfd, int newfd, int *retval) {
    
    *retval = -1;
    if (oldfd < 0 || oldfd >= MAX_FILE_OPEN || newfd < 0 || newfd >= MAX_FILE_OPEN) {
        return EBADF; // Invalid file descriptor
    }
    
    // Check if newfd is open, if so close it
    if (curproc->file_table[newfd].filename_buffer != NULL) {
        char empty[buffer_size];
        strcpy(curproc->file_table[newfd].filename_buffer,empty);
    }
    
    // Place the file at newfd in the file table
    memcpy(&curproc->file_table[newfd],&curproc->file_table[oldfd],sizeof(struct filehandle));
    
    if(oldfd == STDOUT_FILENO || oldfd == STDERR_FILENO || oldfd==STDIN_FILENO){
        if(oldfd == STDERR_FILENO)
			curproc -> std_err = newfd;
		else if(oldfd==STDIN_FILENO)
			curproc -> std_in = newfd;
		else if(oldfd == STDOUT_FILENO)
			curproc -> std_out = newfd;
    }

    *retval = newfd;
    return 0; // Success
}
