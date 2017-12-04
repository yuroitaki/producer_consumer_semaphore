/******************************************************************
 * The helper file that contains the following helper functions:
 * check_arg - Checks if command line input is a number and returns it
 * sem_create - Create number of sempahores required in a semaphore array
 * sem_init - Initialise particular semaphore in semaphore array
 * sem_wait - Waits on a semaphore (akin to down ()) in the semaphore array
 * sem_signal - Signals a semaphore (akin to up ()) in the semaphore array
 * sem_close - Destroy the semaphore array
 ******************************************************************/

# include "helper.h"

int check_arg (char *buffer)
{
  int i, num = 0, temp = 0;
  if (strlen (buffer) == 0)
    return -1;
  for (i=0; i < (int) strlen (buffer); i++)
  {
    temp = 0 + buffer[i];
    if (temp > 57 || temp < 48)
      return -1;
    num += pow (10, strlen (buffer)-i-1) * (buffer[i] - 48);
  }
  return num;
}

int sem_create (key_t key, int num)
{
  int id;                    //num = number of semaphore in the set
  if ((id = semget (key, num,  0666 | IPC_CREAT | IPC_EXCL)) < 0)
    return -1;
  return id;
}

int sem_init (int id, int num, int value)          //num = index of a specific semaphore
{
  union semun semctl_arg;
  semctl_arg.val = value;               //val = set the value of the semaphore
  if (semctl (id, num, SETVAL, semctl_arg) < 0)
    return -1;
  return 0;
}

void sem_wait (int id, short unsigned int num)
{
  struct sembuf op[] = {             //num = index of semaphore to be manipulated
    {num, -1, SEM_UNDO}
  };
  semop (id, op, 1);
}

void sem_signal (int id, short unsigned int num)
{
  struct sembuf op[] = {
    {num, 1, SEM_UNDO}
  };
  semop (id, op, 1);
}

int sem_close (int id)
{
  if (semctl (id, 0, IPC_RMID, 0) < 0)
    return -1;
  return 0;
}


bool deposit_job(Job* job_ptr, Job** job_arr, int buff_size)
{
  for (int i=0;i<buff_size;i++){
    if(job_arr[i]==NULL){
      job_arr[i] = job_ptr;
      return true;
    }
  }return false;
}

bool take_job(Job** job_arr, int& job_id, int buff_size)
{
  for (int i=0;i<buff_size;i++){
    cout << i << " ";
    if(job_arr[i]!=NULL){
      job_id = job_arr[i]->job_id;
      job_arr[i]=NULL;
      return true;
    }
  }return false;
}
