#include <types.h>
#include <proc.h>
#include <current.h>
#include <vnode.h>
#include <vfs.h>
#include <uio.h>
#include <synch.h>
#include <kern/errno.h>
#include <kern/fcntl.h>
#include <copyinout.h>
#include <syscall.h>
#include <limits.h>
#include <kern/unistd.h>
#include <stat.h>
#include <lib.h>


int sys_read(int fd, userptr_t buffer, size_t nbytes, int *retval){

    // EBADF	fd is not a valid file descriptor
	if (fd>=MAX_FILE_OPEN || fd < 0){
        return EBADF;
    }

	// EBADF	 not opened for writing.
	if (curproc->file_table[fd].flags == O_WRONLY || curproc->file_table[fd].filename_buffer==NULL){
		return EBADF;
	}

	// Part or all of the address space pointed to by buf is invalid
	if (buffer == NULL) {
		return EFAULT;
	}

	// create the temporal variables and structures
    char *kbuffer = (char *)kmalloc(sizeof(*buffer)*nbytes);
	struct iovec iov;
	struct uio kuio;
	uio_kinit(&iov, &kuio, kbuffer, nbytes, curproc->file_table[fd].offset, UIO_READ);

    int error = VOP_READ(curproc->file_table[fd].node, &kuio);
	if (error){
		kfree(kbuffer);
		return error;
	}

	// calculate the number of bytes read, pass it as return value and update the offset
    off_t bytes = kuio.uio_offset - curproc->file_table[fd].offset;
	*retval = (int)bytes;
    curproc->file_table[fd].offset = kuio.uio_offset;

	// copy the data read from kernel space to user space
    if(bytes != 0){
		error = copyout(kbuffer, buffer, *retval);
		if(error) {
			kfree(kbuffer);
			return error;
		}
	}

	kfree(kbuffer);
	return 0;

}