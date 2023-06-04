#include<types.h>
#include<current.h>
#include<syscall.h>
#include <proc.h>
#include <thread.h>

int sys_getpid(pid_t *retval){
    //getpid returns the process id of the current process.
    *retval = curproc->pid;
    //kprintf("Process ID: %d\nParent Process ID: %d\n", curproc->pid,curproc->ppid); // Print the process ID
    //getpid does not fail.
    return 0;
}