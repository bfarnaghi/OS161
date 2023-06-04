#include <types.h>
#include <kern/errno.h>
#include <kern/wait.h>
#include <proc.h>
#include <thread.h>
#include <wchan.h>
#include <synch.h>
#include <current.h>
#include <syscall.h>

int
sys_exit (int status)
{
    KASSERT (curproc != NULL);
    curproc->exit = status;
    thread_exit();
    return 0;
}