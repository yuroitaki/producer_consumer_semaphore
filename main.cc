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
  
  Producer prod[producer_vol];
  Consumer cons[consumer_vol];
  Job* jobs[queue_size];  
  Job** job_ptr = jobs;
  
  for(int i=0;i<queue_size;i++){
    jobs[i] = NULL;
  }
  int sem_idd = 0;
  /*int sem_num = 3;
  int sem_idd = sem_create(SEM_KEY,sem_num);
  if(sem_idd < 0){
    cerr << "Semaphore creation failed." << endl;
    return -2;
  }
  int init_result[sem_num];
  init_result[0] = sem_init(sem_idd,MUTEX,1);                //1st = mutex
  init_result[1] = sem_init(sem_idd,SPACE,queue_size);       //2nd = space
  init_result[2] = sem_init(sem_idd,ITEM,0);                //3rd = item
  
  for(int i=0;i<sem_num;i++){
    if(init_result[i] < 0){
      cerr << "Semaphore iniation failed." << endl;
      return -3;
    }
  }*/
  pthread_t producerid[producer_vol], consumerid[consumer_vol];

  for (int i=0;i<producer_vol;i++){
    Producer produc(i+1,job_vol,queue_size,sem_idd,job_ptr);
    prod[i] = produc;
    
    // cout << "Creating producer " << prod[i].prod_id << endl;
    pthread_create(&producerid[i], NULL, producer, (void *) &prod[i]);
  }
  for (int i=0;i<consumer_vol;i++){
    Consumer consum(i+1,queue_size,sem_idd,job_ptr);
    cons[i]= consum;
    
    // cout << "Creating consumer " << cons[i].cons_id << endl;
    pthread_create(&consumerid[i], NULL, consumer, (void *) &cons[i]);
  }
  
  for (int i=0;i<producer_vol;i++){
    pthread_join(producerid[i], NULL);
    // cout << "Producer " << prod[i].prod_id << " has finished " << prod[i].job_count << " jobs."<<endl;
  }
  for (int i=0;i<consumer_vol;i++){
    pthread_join(consumerid[i], NULL);
    // cout << "Consumer " << cons[i].cons_id << " is done."<<endl;
  }
  for(int i=0;i<queue_size;i++){
    cout << jobs[i]->job_id << " "<<jobs[i]->duration<< endl;
    // delete jobs[i];
  }
  /*int close_result = sem_close(sem_idd);
  if(close_result < 0){
    cerr << "Semaphore destruction failed." << endl;
    return -4;
    }*/
  pthread_exit(NULL);
  return 0;
}

void *producer(void* prod) 
{
  Producer* prod_param = (Producer*) prod;
  int job_vol = prod_param->job_count;
  int buff_size = prod_param->que_size;
  int sema_id = prod_param->sem_id;
  Job** job_arr_pt = prod_param->job_arr_ptr;
  // cout << "Producer = " << prod_param->prod_id << " " << job_vol << endl;
  
  for(int i=0;i<job_vol;i++){

    int t = rand() % 10+1;
    Job* job_ptr = new Job(i,t);
    
    /*sem_wait(sema_id,SPACE);
      sem_wait(sema_id,MUTEX);*/

    if(deposit_job(job_ptr,job_arr_pt,buff_size)){
      cout << "Producer " << prod_param->prod_id << " deposited item " << job_ptr->job_id << " with duration " << job_ptr->duration <<endl;
      }
    /*sem_signal(sema_id,MUTEX);
      sem_signal(sema_id,ITEM);*/
      // cout << job_arr_pt[j]->job_id << " "<<job_arr_pt[j]->duration<< endl;
  }    
  pthread_exit(NULL);
}

void *consumer (void* cons) 
{
  Consumer* cons_param = (Consumer*) cons;
  int buff_size = cons_param->que_size;
  int sema_id = cons_param->sem_id;
  int job_idd;
  Job** job_arr_pt = cons_param->job_arr_ptr;
  
  //while(take_job(job_arr_pt,job_idd,buff_size)){
  //cout << "Consumer " << cons_param->cons_id << " consumed item " << job_idd <<endl;
  //}
  // cout << "Consumer = " << cons_param->cons_id << endl;
  pthread_exit(NULL);
}
