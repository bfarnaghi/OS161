#include <types.h>
#include <kern/errno.h>
#include <proc.h>
#include <current.h>
#include <thread.h>
#include <synch.h>
#include <copyinout.h>
#include <syscall.h>

int sys_waitpid(pid_t pid, const struct __userptr * status, int options, int *retval) {

    // Check if the provided PID is valid
    if (pid<=0 || proc_table[pid]==NULL) {
        *retval = -1;
        return EINVAL;
    }
    
    // Check if the options are valid
    if (options != 0) {
        *retval = -1;
        return EINVAL;
    }
    
    // Check if the status pointer is valid
    if (status == NULL) {
        *retval = -1;
        return EFAULT;
    }
    

    // Acquire the process lock to access the process table
    //spinlock_acquire(&proc_table[proc_table[pid]->ppid]->p_lock);

    while (proc_table[pid]->exit==-1)
        thread_yield();
    
    //spinlock_release(&proc_table[proc_table[pid]->ppid]->p_lock);

    // Copy the exit status to the user's buffer
    int copy_status = copyout(&proc_table[pid]->exit, (userptr_t) status, sizeof(int));
    if (copy_status != 0) {
        *retval = -1;
        return copy_status;
    }
    
    // Return the PID of the terminated child process
    *retval = pid;
    return 0;
}
