#include "types.h"
#include "stat.h"
#include "user.h"

/**
 * @brief Get compensation info for a process
 * @param pid Process ID to query
 */
static void get_comp_info(int pid)
{
  int i = 0, j = 0;
  while (i < iters)
  {
    j += i * j + 1;
    i++;
  }
  return j;
}

/**
 * @brief Main function for scheduler test
 * Creates two child processes and reports their compensation ticks
 */
int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    exit();
  }

  int sleepT = atoi(argv[1]);

  sleep(sleepT);
  // here we should have timeslice == 1
  int w1 = workload(800000000);
  kill(-w1); // an unelegant way of "using" the workload value to avoid optimized out
  exit();
}