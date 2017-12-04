/******************************************************************
 * Header file for the helper functions. This file includes the
 * required header files, as well as the function signatures and
 * the semaphore values (which are to be changed as needed).
 ******************************************************************/
#ifndef HELPER_H
#define HELPER_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/ipc.h>
# include <sys/shm.h>
# include <sys/sem.h>
# include <sys/time.h>
# include <math.h>
# include <errno.h>
# include <string.h>
# include <pthread.h>
# include <ctype.h>
# include <iostream>
using namespace std;

# define SEM_KEY 0x99 // Change this number as needed
# define MAX_JOB 1000000
# define MUTEX 0
# define SPACE 1
# define ITEM 2


union semun {
  int val;               /* used for SETVAL only */
  struct semid_ds *buf;  /* used for IPC_STAT and IPC_SET */ //for preventing race condition
  ushort *array;         /* used for GETALL and SETALL */ //to set or get the values of all sempaphores in the array
};

struct Job{
  int job_id;
  int duration;

  Job(){}
  Job(int id,int dur):job_id(id),duration(dur){};
};

struct Producer{
  int prod_id;
  int job_count;
  int que_size;
  int sem_id;
  Job** job_arr_ptr;

  Producer(){}
  Producer(int id,int count,int size,int sid,Job** arr) : prod_id(id),job_count(count),que_size(size),sem_id(sid),job_arr_ptr(arr){}
};

struct Consumer{
  int cons_id;
  int que_size;
  int sem_id;
  Job** job_arr_ptr;
  
  Consumer(){}
  Consumer(int id, int size,int sid,Job** arr) : cons_id(id),que_size(size),sem_id(sid),job_arr_ptr(arr){}
};

int check_arg (char *);
int sem_create (key_t, int);
int sem_init (int, int, int);
void sem_wait (int, short unsigned int);
void sem_signal (int, short unsigned int);
int sem_close (int);

bool deposit_job(Job*,Job**,int);
bool take_job(Job**,int&, int);

#endif
