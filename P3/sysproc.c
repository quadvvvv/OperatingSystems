#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "pstat.h"

int sys_fork(void)
{
  return fork();
}

int sys_exit(void)
{
  exit();
  return 0; // not reached
}

int sys_wait(void)
{
  return wait();
}

int sys_kill(void)
{
  int pid;

  if (argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int sys_getpid(void)
{
  return myproc()->pid;
}

int sys_sbrk(void)
{
  int addr;
  int n;

  if (argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

int sys_sleep(void)
{
  int n;
  uint ticks0;

  if (argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;

  myproc()->comp_slice = 0; // clear the comp_slice if it's put to sleep
  myproc()->ticks0_sleep = ticks0;
  myproc()->sleep_slice = n;

  while (ticks - ticks0 < n)
  { // wake up for each tick
    if (myproc()->killed)
    {
      release(&tickslock);
      return -1;
    }
    // if ticks change, than its waken up, but we don't want to do this.
    // we want it to sleep for the entire time duration.
    sleep(&ticks, &tickslock); // sleep on the ticks' value
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

/**
 * @brief System call to set process time slice
 * @return 0 on success, -1 on failure
 */
int sys_setslice(void)
{
  int pid;
  int slice;

  // check pid and slice
  if ((argint(0, &pid) < 0) || argint(1, &slice) < 0)
    return -1; // failed to retrieve

  if (slice <= 0 || pid <= 0)
    return -1; // not postive, not valid

  return setslice(pid, slice);
}

/**
 * @brief System call to get process time slice
 * @return Time slice value, or -1 on failure
 */
int sys_getslice(void)
{
  int pid;

  // check pid
  if (argint(0, &pid) < 0)
    return -1; // failed to retrieve

  if (pid <= 0)
    return -1; // not postive, not valid

  return getslice(pid);
}

/**
 * @brief System call to fork with specified time slice
 * @return PID of new process, or -1 on failure
 */
int sys_fork2(void)
{
  int slice;

  // check slice
  if (argint(0, &slice) < 0)
    return -1; // failed to retrieve

  if (slice <= 0)
    return -1; // not postive, not valid

  return fork2(slice);
}

/**
 * @brief System call to get process information
 * @return 0 on success, -1 on failure
 */
int sys_getpinfo(void)
{
  struct pstat *pstat;

  if (argptr(0, (void *)&pstat, sizeof(*pstat)) < 0)
    return -1;
  return getpinfo(pstat);
}