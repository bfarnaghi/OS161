#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <copyinout.h>
#include <proc.h>
#include <current.h>
#include <addrspace.h>
#include <vfs.h>
#include <syscall.h>
#include <kern/fcntl.h> //File operation flags

int sys_execv(const_userptr_t program, const_userptr_t *args, int *retval) {
    
    *retval=-1;

    // Check if program and args are valid user-space pointers
    if (program == NULL || args == NULL)
        return EFAULT;

    // Copy the program path from user space
    size_t pathlen;
    char program_buffer[buffer_size];
    int result = copyinstr((const_userptr_t)program, program_buffer, buffer_size, &pathlen);
    if (result)
        return result;

    // Copy the arguments from user space
    int argc = 0;
    while (args[argc] != NULL)
        argc++;
    char **kargs = kmalloc((argc + 1) * sizeof(char *));
    if (kargs == NULL)
        return ENOMEM;
    for (int i = 0; i < argc; i++) {
        char arg[10];
        size_t arglen;
        result = copyinstr((const_userptr_t)args[i], arg, buffer_size, &arglen);
        if (result) {
            kfree(kargs);
            return result;
        }
        kargs[i] = kstrdup((const char *)args[i]);
        if (kargs[i] == NULL) {
            kfree(kargs);
            return ENOMEM;
        }
    }
    kargs[argc] = NULL;

    // Load the program
    struct addrspace *old_as = proc_getas();
    struct addrspace *as = as_create();
    if (as == NULL)
    {
        proc_setas(old_as);
        return ENOMEM;
    }

    proc_setas(as);
    as_activate();

    struct vnode *v;
    result = vfs_open(program_buffer, O_RDONLY, 0, &v);
    if (result)
    {
        proc_setas(old_as);
        return result;
    }

    vaddr_t entrypoint;
    result = load_elf(v, &entrypoint);
    if (result)
    {
        proc_setas(old_as);
        vfs_close(v);
        return result;
    }

    vfs_close(v);

    // Copy the arguments onto the user stack
    // Calculate the total size needed for the arguments on the stack
    size_t arg_size = 0;
    for (int i = 0; i < argc; i++) {
        arg_size += strlen(kargs[i]) + 1; // Include null terminator
    }

    // Allocate space on the user stack for the arguments
    vaddr_t stackptr = USERSTACK - arg_size;
    char *user_stack = kmalloc(arg_size);
    if (user_stack == NULL) {
        proc_setas(old_as);
        as_destroy(as);
        kfree(kargs);
        return ENOMEM;
    }

    // Copy the arguments to the user stack
    vaddr_t arg_ptr = stackptr;
    for (int i = 0; i < argc; i++) {

        size_t arg_len = strlen(kargs[i]) + 1; // Include null terminator

        // Copy the argument to the user stack
        result = copyoutstr(kargs[i], (userptr_t)arg_ptr, arg_len, NULL);
        if (result) {
            proc_setas(old_as);
            as_destroy(as);
            kfree(kargs);
            kfree(user_stack);
            return result;
        }

        // Update the argument pointer
        arg_ptr += arg_len;

        // Free the kernel argument
        kfree(kargs[i]);
    }

    // Copy the argument pointers onto the user stack
    /*vaddr_t arg_ptrs_ptr = stackptr - (argc + 1) * sizeof(char *);
    result = copyout(kargs, (userptr_t)arg_ptrs_ptr, (argc + 1) * sizeof(char *));
    if (result) {
        proc_setas(old_as);
        kfree(user_stack);
        return result;
    }*/

    // Free the kernel argument array
    kfree(kargs);

    // Set the user stack pointer in the process's address space
    as_define_stack(as, &stackptr);

    // Free the old address space
    //as_destroy(old_as);

    *retval=0;

    // Warp to user mode
    enter_new_process(argc,(userptr_t)stackptr,NULL, stackptr, entrypoint);

    // enter_new_process does not return
kprintf("\n12");
    return EINVAL; // Should never reach this point
}
