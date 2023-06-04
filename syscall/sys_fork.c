#include <proc.h>
#include <types.h> /* for various types to be accessed */
#include <thread.h> /* for the thread_fork function */
#include <current.h> /* for the value of curthread */
#include <addrspace.h> /* to get the as_copy, as_activate functions */
#include <lib.h> /* for the memcpy function */
#include <kern/errno.h> /* for error values */
#include <mips/trapframe.h>
#include <syscall.h>
#include <spl.h>
#include <vnode.h>

void
enter_fork_process(void *data1, unsigned long data2);

int sys_fork(struct trapframe *tf, int32_t *retval) {
    
    //int spl = splhigh();
    //kprintf("P th:%s\n",curthread->t_name);
    *retval = -1;

    // 1. Create a new process structure (child process).
    struct proc *parent_proc;
    struct proc *child_proc=NULL;
    struct trapframe *child_tf;

    child_tf = kmalloc(sizeof(*child_tf));
    if (child_tf == NULL) {
        return ENOMEM; // Return error code if unable to allocate trapframe
    }
    memcpy(child_tf, tf, sizeof(*child_tf));

    parent_proc = curproc ;
    child_proc = proc_create_runprogram(parent_proc->p_name);
    if (child_proc == NULL)
        return ENOMEM;

    // 2. Allocate and initialize a new address space for the child process.
    struct addrspace *parent_addrspace = parent_proc->p_addrspace;
    struct addrspace *child_addrspace;
    int result = as_copy(parent_addrspace, &child_addrspace);
    if (result != 0) {
        proc_destroy(child_proc);
        return result; // Return error code if unable to copy address space
    }
    spinlock_acquire(&parent_proc->p_lock);
    child_proc->p_addrspace = child_addrspace; //ch:8003 3B00      p:8003 3AE0
    if (parent_proc->p_cwd != NULL) {
		VOP_INCREF(parent_proc->p_cwd);
		child_proc->p_cwd = parent_proc->p_cwd;
	}
    spinlock_release(&parent_proc->p_lock);
    //kprintf("ch add:%d  p add:%d \n",(unsigned int)child_addrspace,(unsigned int) parent_addrspace);

    // 3. Copy the parent process's file table to the child process.
    // Iterate over the file descriptors in the parent process's file table
    for (int i = 0; i < MAX_FILE_OPEN; i++) {
        strcpy(child_proc->file_table[i].filename_buffer,parent_proc->file_table[i].filename_buffer);
        child_proc->file_table[i].flags = parent_proc->file_table[i].flags;
        child_proc->file_table[i].offset = parent_proc->file_table[i].offset;
        //memcpy(child_proc->file_table[i].node,parent_proc->file_table[i].node,sizeof(struct vnode));
    }

    //kprintf("cur add th stack:%d\n",(unsigned int)curthread->t_stack); //8003D000     8006 F000

    *retval = child_proc->pid;

    result = thread_fork(curthread->t_name,child_proc,enter_fork_process,(void *) child_tf, 0);
    if (result) {
        as_destroy(child_addrspace);
        proc_destroy(child_proc);
        return result;
    }
    //splx(spl);//Re-Enable interrupts after thread_fork

    return 0;
}

void
enter_fork_process(void *data1, unsigned long data2)
{
    //kprintf("Ch th:%s\n",curthread->t_name);
	(void) data2;
    struct trapframe *ntf = (struct trapframe *) data1;
    KASSERT(curthread != NULL);
	KASSERT(curthread->t_curspl == 0);
	KASSERT(curthread->t_iplhigh_count == 0);
    as_activate();
    /* return 0 and signal no error */
	ntf->tf_v0 = 0;
	ntf->tf_a3 = 0;
	ntf->tf_epc += 4;

    mips_usermode(ntf);
    panic("enter_forked_process returned\n");
}
