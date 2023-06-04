#include <kern/errno.h>
#include <syscall.h>
#include <types.h>  
#include <vnode.h> 
#include <lib.h>  
#include <current.h> 
#include <vfs.h>
#include <proc.h>
#include <kern/fcntl.h> //File operation flags
#include <copyinout.h> 
#include <stat.h>

int sys_open(const_userptr_t filename, int flags, int *retval) {

    // Check if filename is a valid pointer
    if (filename == NULL)
        return EFAULT;

    // Error Handling -> EINVAL	flags contained invalid values.
	switch(flags) {
		
		//The flags argument should consist of one of

		//O_RDONLY	Open for reading only.
		//O_WRONLY	Open for writing only.
		//O_RDWR	Open for reading and writing.
	
		case O_RDONLY: break;
		case O_WRONLY: break;
		case O_RDWR: break;
		
		//It may also have any of the following flags OR'd in:
		//O_CREAT	Create the file if it doesn't exist.
		//O_EXCL	Fail if the file already exists.
		//O_TRUNC	Truncate the file to length 0 upon open.
		//O_APPEND	Open the file in append mode.

		case O_RDONLY|O_CREAT: break;
		case O_WRONLY|O_CREAT: break;
		case O_RDWR|O_CREAT: break;
		
		case O_RDWR|O_CREAT|O_TRUNC: break;
		
		case O_WRONLY|O_APPEND: break;
		case O_RDWR|O_APPEND: break;
		
		
		//O_EXCL is only meaningful if O_CREAT is also used.
		
		default: 
			return EINVAL;
	}

    // Copy the file name to the buffer
	size_t act;
    char filename_buffer[buffer_size]; 
	int ret_error = copyinstr(filename, filename_buffer, buffer_size, &act);
	if (ret_error != 0) {
		return ret_error;
	}

    // Perform the open operation
    struct vnode * node = NULL;
    int result = vfs_open(filename_buffer, flags,0, &node);
    if (result != 0)
        return result;

    //Find a free slot in file_table array
    int slot_index=0;
    for(int i=3; i<MAX_FILE_OPEN;i++){
		if(curproc->file_table[i].filename_buffer==NULL){
            slot_index=i;
            break;
        }
    }

    curproc->file_table[slot_index].node = node;
    curproc->file_table[slot_index].flags = flags;
    curproc->file_table[slot_index].offset = 0;
    strcpy(curproc->file_table[slot_index].filename_buffer,filename_buffer);

    //Handle the Append flag
    if((flags==(O_WRONLY|O_APPEND)) || (flags==(O_RDWR|O_APPEND))){
        struct stat open_file_status;
        VOP_STAT(node,&open_file_status);
        int file_size = open_file_status.st_size;
        curproc->file_table[slot_index].offset = file_size;
    }

    // Set the file descriptor as the return value
    *retval = slot_index;

    //kprintf("retval=%d  filename=%s flags:%d \n",slot_index,filename_buffer,flags);

    /* on success, return 0 */
    return 0;
}