/******************************************************************
 * The Main program with the two functions. A simple
 * example of creating and using a thread is provided.
 ******************************************************************/

#include "helper.h"
  
void *producer (void* prod);
void *consumer (void* cons);

int main (int argc, char **argv)
{
  int num_arg[argc];

  if(argc!=5){
    cerr << "Insufficient command line arguments." << endl;
    return -5;
  }
  for(int i=1;i<argc;i++){
    num_arg[i] = check_arg(argv[i]);
    if (num_arg[i] < 0){
      cerr << "Invalid command line arguments." << endl;
      return -1;
    }
  }
  if(num_arg[1]==0){
    cerr << "Queue size should not be 0." << endl;
    return -10;
  }
  int queue_size = num_arg[1];
  int job_vol = num_arg[2];
  int producer_vol = num_arg[3];
  int consumer_vol = num_arg[4];
  int total_job_count = producer_vol*job_vol;
  
  Producer prod[producer_vol];
  Consumer cons[consumer_vol]; 
  Job** job_ptr = new Job* [queue_size];
  int* prod_ptr = new int;
  *prod_ptr = 0;
  int* cons_ptr = new int;
  *cons_ptr = 0;
  int* job_done = new int;
  *job_done = 0;
  
  for(int i=0;i<queue_size;i++){
    job_ptr[i] = NULL;
  }
  int sem_num = 3;
  int sem_idd = sem_create(SEM_KEY,sem_num);
  if(sem_idd < 0){
    cerr << "Semaphore creation failed." << endl;
    return -2;
  }
  int init_result[sem_num];
  init_result[0] = sem_init(sem_idd,MUTEX,1);                
  init_result[1] = sem_init(sem_idd,SPACE,queue_size);       
  init_result[2] = sem_init(sem_idd,ITEM,0);
  
  for(int i=0;i<sem_num;i++){
    if(init_result[i] < 0){
      cerr << "Semaphore " << i << " iniation failed." << endl;
      return -3;
    }
  }
  pthread_t producerid[producer_vol], consumerid[consumer_vol];

  for (int i=0;i<producer_vol;i++){
    Producer produc(i+1,job_vol,queue_size,prod_ptr,sem_idd,job_done,job_ptr);
    prod[i] = produc;
    int prod_thread_result = pthread_create(&producerid[i], NULL, producer, (void *) &prod[i]);

    if(prod_thread_result<0){
      cerr << "Producer thread " << i << " creation failed." << endl;
      return -6;
    }
  }
  for (int i=0;i<consumer_vol;i++){
    Consumer consum(i+1,queue_size,cons_ptr,sem_idd,job_done,total_job_count,job_ptr);
    cons[i]= consum;  
    int cons_thread_result = pthread_create(&consumerid[i], NULL, consumer, (void *) &cons[i]);
 
    if(cons_thread_result<0){
      cerr << "Consumer thread " << i << " creation failed." << endl;
      return -7;
    }
  }
  for (int i=0;i<producer_vol;i++){
    int prod_join_result = pthread_join(producerid[i], NULL);

    if(prod_join_result<0){
      cerr << "Producer thread " << i << " joining failed." << endl;
      return -8;
    }
  }
  for (int i=0;i<consumer_vol;i++){
    int cons_join_result = pthread_join(consumerid[i], NULL);

    if(cons_join_result<0){
      cerr << "Consumer thread " << i << " joining failed." << endl;
      return -9;
    }
  }
  for(int i=0;i<queue_size;i++){
    if(job_ptr[i]!=NULL)
      cout << job_ptr[i]->job_id << " "<<job_ptr[i]->duration << endl;
  }
  delete [] job_ptr;
  delete prod_ptr;
  delete cons_ptr;
  delete job_done;
  
  int close_result = sem_close(sem_idd);
  if(close_result < 0){
    cerr << "Semaphore destruction failed." << endl;
    return -4;
  }
  pthread_exit(NULL);
  return 0;
}

void *producer(void* prod) 
{
  Producer* prod_param = (Producer*) prod;
  int job_vol = prod_param->job_count;
  int buff_size = prod_param->que_size;
  int sema_id = prod_param->sem_id;
  int* prod_ptr = prod_param->prod_pointer;
  int* job_done = prod_param->job_done;
  Job** job_arr_pt = prod_param->job_arr_ptr;
  
  for (int i=0;i<job_vol;i++){

    int sleep_dur = rand() % 5+1;
    sleep(sleep_dur);
    int t = rand() % 10+1;
    
    int wait_code = sem_wait(sema_id,SPACE);    
    if(wait_code < 0){
      printf("Producer(%d): No slot available after 20 seconds.\n",prod_param->prod_id);
      pthread_exit(NULL);
    }
    sem_wait(sema_id,MUTEX);

    int index = *prod_ptr % buff_size;
    job_arr_pt[index] = new Job(index+1,t);
    (*prod_ptr)++;
    (*job_done)++;
    
    printf("Producer(%d): Job id %d duration %d \n",prod_param->prod_id,job_arr_pt[index]->job_id,job_arr_pt[index]->duration);
  
    sem_signal(sema_id,MUTEX);
    sem_signal(sema_id,ITEM);
  }
  printf("Producer(%d): No more jobs to generate.\n",prod_param->prod_id);
  
  pthread_exit(NULL);
}

void *consumer (void* cons) 
{
  Consumer* cons_param = (Consumer*) cons;
  int sema_id = cons_param->sem_id;
  int buff_size = cons_param->buffer_size;
  int tot_job = cons_param->total_job;
  int* cons_ptr = cons_param->cons_pointer;
  int* job_done = cons_param->job_done;
  Job** job_arr_pt = cons_param->job_arr_ptr;

  while(true){
    
    int wait_code = sem_wait(sema_id,ITEM);
    if(wait_code<0){
      if(*job_done == tot_job){
	printf("Consumer(%d): No more jobs left.\n",cons_param->cons_id);
	pthread_exit(NULL);
      }
      printf("Consumer(%d): No jobs arrived after 20 seconds.\n",cons_param->cons_id);
      pthread_exit(NULL);
    }
    
    sem_wait(sema_id,MUTEX);
    
    int index = (*cons_ptr) % buff_size;
    int sleep_dur = job_arr_pt[index]->duration;
    int j_idd = job_arr_pt[index]->job_id;
    delete job_arr_pt[index];
    job_arr_pt[index] = NULL;
    (*cons_ptr)++;
    
    printf("Consumer(%d): Job id %d executing sleep duration %d \n",cons_param->cons_id,j_idd,sleep_dur);
    
    sem_signal(sema_id,MUTEX);
    sem_signal(sema_id,SPACE);
    
    sleep(sleep_dur);
    printf("Consumer(%d): Job id %d completed \n",cons_param->cons_id,j_idd);
  }
  pthread_exit(NULL);
}
