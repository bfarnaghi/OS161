#include <types.h>
#include <kern/errno.h>
#include <kern/fcntl.h>
#include <kern/unistd.h>
#include <vfs.h>
#include <current.h>
#include <copyinout.h>
#include <proc.h>
#include <syscall.h>

int sys_chdir(userptr_t path)
{
    char kpath[buffer_size];
    size_t pathlen;
    int result;

    // Copy the path from user space to kernel space
    result = copyinstr(path, kpath, buffer_size, &pathlen);
    if (result != 0)
    {
        return result;
    }

    result = vfs_chdir(kpath);
    if (result){
        kfree(kpath);
        return result;
    }

    return 0;
}
