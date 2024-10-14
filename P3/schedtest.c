#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"
#define NULL 0

static struct pstat pstat;
static int compticks = -1;
static int sleepticks = -1;

/**
 * @brief Get compensation info for a process
 * @param pid Process ID to query
 */
static void get_comp_info(int pid)
{
  int pret = getpinfo(&pstat);
  if (pret != 0)
  {
    printf(1, "XV6_SCHEDULER\t getpinfo(&pstat) failed\n");
    exit();
  }
  for (int i = 0; i < NPROC; ++i)
  {
    if (pstat.inuse[i] == 1 && pstat.pid[i] == pid)
    {
      compticks = pstat.compticks[i];
      sleepticks = pstat.sleepticks[i];
      return;
    }
  }
  printf(1, "XV6_SCHEDULER\t did not find process %d in the fetched pstat\n", pid);
  exit();
}

/**
 * @brief Main function for scheduler test
 * Creates two child processes and reports their compensation ticks
 */
int main(int argc, char *argv[])
{
  if (argc != 6)
  {
    exit();
  }

  int sliceA = atoi(argv[1]);
  // int sleepA = atoi(argv[2]);
  int sliceB = atoi(argv[3]);
  // int sleepB = atoi(argv[4]);
  int sleepParent = atoi(argv[5]);

  // int mypid = getpid();

  // first child
  int pid_a = fork2(sliceA);
  if (pid_a < 0)
  {
    printf(1, "XV6_SCHEDULER\t fork2(sliceA) failed\n");
    exit();
  }
  if (pid_a == 0)
  {
    char *args[3] = {
        "/loop", // string literial is of type "const char*"
        argv[2],
        NULL // it must have a NULL in the end of argv
    };
    exec(args[0], args);
    exit();
  }

  // second child
  int pid_b = fork2(sliceB);
  if (pid_b < 0)
  {
    printf(1, "XV6_SCHEDULER\t fork2(sliceA) failed\n");
    exit();
  }
  if (pid_b == 0)
  {
    char *args[3] = {
        "/loop", // string literial is of type "const char*"
        argv[4],
        NULL // it must have a NULL in the end of argv
    };
    exec(args[0], args);
    exit();
  }

  sleep(sleepParent);

  get_comp_info(pid_a);
  int compticksA = compticks;
  get_comp_info(pid_b);
  int compticksB = compticks;

  printf(1, "%d %d\n", compticksA, compticksB);

  // wait for the two loop processes
  for (int i = 0; i < 2; ++i)
    wait();

  exit();
}