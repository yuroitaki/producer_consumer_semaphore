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
  
  for(int i=1;i<argc;i++){
    num_arg[i] = check_arg(argv[i]);
    if (num_arg[i] < 0){
      cerr << "Invalid command line arguments." << endl;
      return -1;
    }
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
      cerr << "Semaphore iniation failed." << endl;
      return -3;
    }
  }
  pthread_t producerid[producer_vol], consumerid[consumer_vol];

  for (int i=0;i<producer_vol;i++){
    Producer produc(i+1,job_vol,queue_size,prod_ptr,sem_idd,job_done,job_ptr);
    prod[i] = produc;
    pthread_create(&producerid[i], NULL, producer, (void *) &prod[i]);
  }
  for (int i=0;i<consumer_vol;i++){
    Consumer consum(i+1,queue_size,cons_ptr,sem_idd,job_done,total_job_count,job_ptr);
    cons[i]= consum;  
    pthread_create(&consumerid[i], NULL, consumer, (void *) &cons[i]);
  }
  
  for (int i=0;i<producer_vol;i++){
    pthread_join(producerid[i], NULL);
  }
  for (int i=0;i<consumer_vol;i++){
    pthread_join(consumerid[i], NULL);
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
    
    int wait_code = sem_wait(sema_id,SPACE);
    if(wait_code<0){
      cout << "Producer(" << prod_param->prod_id << "): No slot available after 20 seconds." << endl;
      pthread_exit(NULL);
    }
    sem_wait(sema_id,MUTEX);

    int sleep_dur = rand() % 5+1;
    sleep(sleep_dur);
    
    int index = *prod_ptr % buff_size;
    int t = rand() % 10+1;  
    job_arr_pt[index] = new Job(index+1,t);
    (*prod_ptr)++;
    (*job_done)++;
    
    cout << "Producer(" << prod_param->prod_id << "): Job id " << job_arr_pt[index]->job_id << " duration " << job_arr_pt[index]->duration <<endl;
  
    sem_signal(sema_id,MUTEX);
    sem_signal(sema_id,ITEM);
  }
  cout << "Producer(" << prod_param->prod_id << "): No more jobs to generate." << endl;
  pthread_exit(NULL);
}

void *consumer (void* cons) 
{
  Consumer* cons_param = (Consumer*) cons;
  int sema_id = cons_param->sem_id;
  int buff_size = cons_param->buffer_size;
  Job** job_arr_pt = cons_param->job_arr_ptr;
  int* cons_ptr = cons_param->cons_pointer;
  int* job_done = cons_param->job_done;
  int tot_job = cons_param->total_job;

  while(true){
    
    int wait_code = sem_wait(sema_id,ITEM);
    if(wait_code<0){
      if(*job_done == tot_job){
	cout << "Consumer(" << cons_param->cons_id << "): No more jobs left." << endl;
	pthread_exit(NULL);
      }
      cout << "Consumer(" << cons_param->cons_id << "): No jobs arrived after 20 seconds." <<endl;
      pthread_exit(NULL);
    }
    
    sem_wait(sema_id,MUTEX);
    
    int index = (*cons_ptr) % buff_size;
    int sleep_dur = job_arr_pt[index]->duration;
    int j_idd = job_arr_pt[index]->job_id;
    job_arr_pt[index] = NULL; 
    (*cons_ptr)++;
    
    cout << "Consumer(" << cons_param->cons_id << "): Job id " << j_idd << " executing sleep duration "  << sleep_dur << endl;
    
    sem_signal(sema_id,MUTEX);
    sem_signal(sema_id,SPACE);
    
    sleep(sleep_dur);
    cout << "Consumer(" << cons_param->cons_id << "): Job id " << j_idd << " completed" << endl;
  }
  pthread_exit(NULL);
}
