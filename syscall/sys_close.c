#include <types.h>
#include <lib.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <kern/syscall.h>
#include <current.h>
#include <synch.h>
#include <kern/iovec.h>
#include <uio.h>
#include <copyinout.h>
#include <syscall.h>
#include <proc.h>
#include <vfs.h>
#include <vnode.h>


int
sys_close(int fd, int *retval)
{   
    char empty[buffer_size];

    // EBADF	fd is not a valid file descriptor
	if (fd>=MAX_FILE_OPEN || fd < 0){
        return EBADF;
    }

	// EBADF	 not opened for writing.
	if (curproc->file_table[fd].filename_buffer==NULL){
		return EBADF;
	}

    strcpy(curproc->file_table[fd].filename_buffer,empty);

    *retval = 0;

    return 0;
}