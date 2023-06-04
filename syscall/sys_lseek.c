#include <types.h>
#include <stat.h>
#include <kern/seek.h>
#include <proc.h>
#include <kern/errno.h>
#include <uio.h>
#include <vnode.h>
#include <current.h>
#include <syscall.h>
#include <copyinout.h>

int  sys_lseek(int fd, off_t pos,uint32_t sp16, int *retval){
    int whence;
    *retval = -1;
    
    // EBADF	fd is not a valid file descriptor
	if (fd>=MAX_FILE_OPEN || fd < 0){
        return EBADF;
    }

	// EBADF	 not opened for writing.
	if (curproc->file_table[fd].filename_buffer==NULL){
		return EBADF;
	}

    // copy content of last user argument from user stack
    int error = copyin((const_userptr_t)sp16, &whence, sizeof(int));
	if(error){
		return error;
	}

    if(whence!= SEEK_SET && whence!= SEEK_CUR && whence!= SEEK_END)
        return EINVAL;

    if(!VOP_ISSEEKABLE(curproc->file_table[fd].node)) {
		return ESPIPE;
	}

    // Seek the file according to the specified 'whence'
    off_t newpos=0;
    if(whence == SEEK_SET){
        if(pos<0)
            return EINVAL;
        curproc->file_table[fd].offset = pos;
        newpos = pos;
    }
    else if(whence == SEEK_CUR){
        if(curproc->file_table[fd].offset+pos < 0)
            return EINVAL;
        curproc->file_table[fd].offset += pos;
        newpos = curproc->file_table[fd].offset; 
    }
    else if (whence == SEEK_END){
        struct stat file_stat;
        int error = VOP_STAT(curproc->file_table[fd].node, &file_stat);
            if (error)
                return error;
        if(file_stat.st_size + pos < 0)
            return EINVAL;
        curproc->file_table[fd].offset = file_stat.st_size + pos;
        newpos =  curproc->file_table[fd].offset;
    }
    //kprintf("newpos= %d\n",(int)newpos);
    *retval = newpos;

    return 0;

}