#include <kern/errno.h>
#include <kern/unistd.h>
#include <syscall.h>
#include <types.h>  
#include <vnode.h> 
#include <lib.h>  
#include <current.h> 
#include <vfs.h>
#include <proc.h>
#include <kern/fcntl.h> //File operation flags
#include <copyinout.h> 
#include <uio.h>

int sys_write(int fd,const_userptr_t buffer,size_t buff_size,int *retval){

	int _fd = fd;

	// EBADF	fd is not a valid file descriptor
	if (fd>=MAX_FILE_OPEN || fd < 0){
        return EBADF;
    }

	if(fd == STDOUT_FILENO || fd == STDERR_FILENO || fd==STDIN_FILENO){
		if(fd == STDERR_FILENO)
			_fd = curproc -> std_err;
		else if(fd==STDIN_FILENO)
			_fd = curproc -> std_in;
		else if(fd == STDOUT_FILENO)
			_fd = curproc -> std_out;

		struct vnode * node = NULL;
		char filename_buffer[5]; 
		strcpy(filename_buffer,curproc->file_table[_fd].filename_buffer);

		int result = vfs_open(filename_buffer, O_RDWR,0, &node);
		if (result != 0)
			return result;
	    curproc->file_table[_fd].node = node;
		curproc->file_table[_fd].flags = O_RDWR;
		//strcpy(curproc->file_table[fd].filename_buffer,"con:");

		//curproc->file_table[fd].offset=0;
	}

	// EBADF	 not opened for writing.
	if (curproc->file_table[_fd].flags == O_RDONLY || curproc->file_table[_fd].filename_buffer==NULL){
		return EBADF;
	}

	// Part or all of the address space pointed to by buf is invalid
	if (buffer == NULL) {
		return EFAULT;
	}

    // Copy the file name to the buffer
	size_t act;
    char write_string[buffer_size]; 
	int ret_error = copyinstr(buffer, write_string, buffer_size, &act);

	if (ret_error != 0) {
		return ret_error;
	}

    //kprintf("Pos to write: %d\n",curproc->file_table[fd].offset);
	// use the functions to write 
	struct uio u;
    struct iovec iov;
    int result;
    // Initialize the uio structure
    uio_kinit(&iov, &u, (void *)buffer, buff_size, curproc->file_table[_fd].offset,UIO_WRITE);

    // Perform the write operation
    result = VOP_WRITE(curproc->file_table[_fd].node, &u);
    if (result)
        return result;
	
    // Update the file offset
     curproc->file_table[_fd].offset = u.uio_offset;

    // Set the number of bytes written as the return value
    *retval = buff_size - u.uio_resid;

	return 0;

}