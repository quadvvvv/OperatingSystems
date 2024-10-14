#include "helper.h"
#include "request.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "slot.h"
#include <signal.h>
#include <pthread.h>
#include <string.h>

// Global Variables:
// Part A: Thread synchronization and buffer management
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buf_not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t buf_not_empty = PTHREAD_COND_INITIALIZER;
pthread_t *workers; // array of worker threads
int threads;        // size of the workers
int thread_i;       // index of the worker thread
int fill_ptr;       // Reference: Textbook
int use_ptr;        // Reference: Textbook

// Part A: Buffer for producer-consumer pattern
int *buffer; // fixed-size buffer for connection descriptors
int buffers; // size of the buffer
int count;   // number of connection descriptors in the buffer

// Part B: Shared memory
char shm_name[100]; // name of the shared memory object
slot_t *shm_ptr;    // array of shared slots
int pagesize;

// Signal handler for graceful termination (Part B)
static void my_handler(int sig)
{
  // Unmap shared memory
  int pagesize = getpagesize();
  int ret = munmap(shm_ptr, pagesize);
  if (ret != 0)
  {
    perror("munmap");
    exit(1);
  }

  // Delete the shared memory region
  ret = shm_unlink(shm_name);
  if (ret != 0)
  {
    perror("shm_unlink");
    exit(1);
  }

  // Free allocated resources
  free(buffer);
  free(workers);

  // Destroy synchronization primitives
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&buf_not_empty);
  pthread_cond_destroy(&buf_not_full);
  exit(0);
}

// Parse command line arguments
void getargs(int *port_num, int *threads, int *buffers, char *shm_name, int argc, char *argv[])
{
  if (argc != 5)
  {
    fprintf(stderr, "Usage: %s <port_num> <threads> <buffers> <shm_name>\n", argv[0]);
    exit(1);
  }
  *port_num = atoi(argv[1]);
  *threads = atoi(argv[2]);
  *buffers = atoi(argv[3]);
  strcpy(shm_name, argv[4]);
  if (*threads < 1 || *port_num < 1 || *buffers < 1 || *port_num == 22)
  {
    fprintf(stderr, "invalid argument(s)\n");
    exit(1);
  }
}

// Worker thread function (Part A)
static void *
worker_func(void *arg)
{
  // Initialize the relevant fields in the slot_t (Part B)
  pthread_mutex_lock(&mutex);
  int slot_i = thread_i;
  shm_ptr[slot_i].thread_id = pthread_self();
  shm_ptr[slot_i].dynamic_req = 0;
  shm_ptr[slot_i].static_req = 0;
  shm_ptr[slot_i].total = 0;
  shm_ptr[slot_i].use = 1;
  thread_i++;
  pthread_mutex_unlock(&mutex);

  // Worker thread main loop
  while (1)
  {
    // Wait for a connection in the buffer (consumer part of producer-consumer)
    pthread_mutex_lock(&mutex);
    while (count == 0)
    {
      pthread_cond_wait(&buf_not_empty, &mutex);
    }
    int connfd = buffer[use_ptr];
    int is_static = requestHandle(connfd);
    Close(connfd);

    // Update statistics in shared memory (Part B)
    if (is_static == 1)
    {
      shm_ptr[slot_i].static_req = shm_ptr[slot_i].static_req + 1;
      shm_ptr[slot_i].total = shm_ptr[slot_i].total + 1;
    }
    else if (is_static == 0)
    {
      shm_ptr[slot_i].dynamic_req = shm_ptr[slot_i].dynamic_req + 1;
      shm_ptr[slot_i].total = shm_ptr[slot_i].total + 1;
    }
    use_ptr = (use_ptr + 1) % (buffers);
    count--;

    pthread_cond_signal(&buf_not_full);
    pthread_mutex_unlock(&mutex);
  }
  return NULL;
}

int main(int argc, char *argv[])
{
  int listenfd, connfd, port_num, clientlen;
  struct sockaddr_in clientaddr;

  // Initialize the locks and relevant fields
  threads = 0;
  thread_i = 0;
  fill_ptr = 0;
  use_ptr = 0;
  buffers = 0;
  count = 0;
  pagesize = 0;
  memset(shm_name, '\0', sizeof(shm_name));

  getargs(&port_num, &threads, &buffers, shm_name, argc, argv);

  // Part B: Create & initialize the shared memory region
  pagesize = getpagesize();
  int shmfd = shm_open(shm_name, O_RDWR | O_CREAT, 0660);
  if (shmfd < 0)
  {
    perror("shm_open");
    exit(1);
  }
  int ret = ftruncate(shmfd, pagesize);
  if (ret != 0)
  {
    perror("ftruncate");
    exit(1);
  }
  shm_ptr = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
  if (shm_ptr == MAP_FAILED)
  {
    perror("mmap");
    exit(1);
  }

  // Part A: Create thread pool and buffer
  buffer = (int *)calloc(buffers, sizeof(int));
  workers = (pthread_t *)calloc(threads, sizeof(pthread_t));
  for (int i = 0; i < threads; i++)
  {
    pthread_create(&workers[i], NULL, worker_func, NULL);
  }

  // Set up signal handler for graceful termination
  signal(SIGINT, my_handler);

  // Main server loop (producer part of producer-consumer)
  listenfd = Open_listenfd(port_num);
  while (1)
  {
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *)&clientlen);

    // Add connection to buffer
    pthread_mutex_lock(&mutex);
    while (count == buffers)
    {
      pthread_cond_wait(&buf_not_full, &mutex);
    }

    buffer[fill_ptr] = connfd;
    fill_ptr = (fill_ptr + 1) % buffers;
    count++;

    pthread_cond_signal(&buf_not_empty);
    pthread_mutex_unlock(&mutex);
  }
}