#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"


uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);

  backtrace();
  return 0;
}


#ifdef LAB_PGTBL
int sys_pgaccess(void) {
    uint64 va_start = 0;
    int n = 0;
    uint64 buffer_addr = 0;
    uint64 buffer = 0;

    // retrieve arguments
    if (argaddr(0, &va_start) < 0 || argint(1, &n) < 0 || argaddr(2, &buffer_addr) < 0) {
        return -1;
    }

    // check whether n is in valid arange
    if (n < 0 || n > 64) {
        printf("error: the maximum pages to check is 64, i.e. 0 < n < 64. You are requesting to check %d pages", n);
    }

    uint64 va = va_start;
    struct proc *p = myproc();

    for (int i = 0; i < n; ++i) {
        pte_t *pte = walk(p->pagetable, va, 0);
        if ((PTE_A & (*pte)) != 0) {
            buffer |= (1L << i);
        }
        // clear PTE_A after checking it
        *pte &= (~PTE_A);
        va += PGSIZE;
    }

    copyout(p->pagetable, buffer_addr, (char *)&buffer, 8);

    return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64 sys_trace(void) {
    int mask = 0;

    if (argint(0, &mask) < 0) {
        return -1;
    }

    myproc()->traced_syscall = mask;
    return 0;
}

uint64 sys_sysinfo(void) {
    struct sysinfo sinfo;
    uint64 info_t = 0; // user pointer to struct sysinfo
    struct proc *p = myproc();

    if (argaddr(0, &info_t) < 0) {
        return -1;
    }

    // fill out sysinfo struct with collected information
    sinfo.freemem = free_mem_size();
    sinfo.nproc = proc_num();

    if (copyout(p->pagetable, info_t, (char *)&sinfo, sizeof(sinfo)) < 0) {
        return -1;
    }

    return 0;
}

uint64 sys_sigalarm(void) {
    int interval = 0;
    void (*handler)() = 0;
    struct proc *p = myproc();

    if (argint(0, &interval) < 0) {
        return -1;
    }
    p->alarm_interval = interval;

    if (argaddr(1, (uint64 *)&handler) < 0) {
        return -1;
    }
    p->alarm_handler = handler;

    return 0;
}

uint64 sys_sigreturn(void) {
    return 0;
}

