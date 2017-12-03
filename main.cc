/******************************************************************
 * The Main program with the two functions. A simple
 * example of creating and using a thread is provided.
 ******************************************************************/

#include "helper.h"

struct Job{
  int job_id;
  int duration;
};

struct Producer{
  int prod_id;
  int job_count;
  Job** job_arr;

  Producer(){}
  Producer(int id,int count,Job** arr) : prod_id(id),job_count(count),job_arr(arr){}
};

struct Consumer{
  int cons_id;
  Job** job_arr;

  Consumer(){}
  Consumer(int id, Job** arr) : cons_id(id),job_arr(arr){}
};
  
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
  Job jobs[queue_size];
  Job* job_ptr = jobs;
  
  pthread_t producerid[producer_vol], consumerid[consumer_vol];

  for (int i=0;i<producer_vol;i++){
    Producer produc(i+1,job_vol,&job_ptr);
    prod[i] = produc;
    
    cout << "Creating producer " << prod[i].prod_id << endl;
    pthread_create(&producerid[i], NULL, producer, (void *) &prod[i]);
  }
  for (int i=0;i<consumer_vol;i++){
    Consumer consum(i+1,&job_ptr);
    cons[i]= consum;
    
    cout << "Creating consumer " << cons[i].cons_id << endl;
    pthread_create(&consumerid[i], NULL, consumer, (void *) &cons[i]);
  }
  
  for (int i=0;i<producer_vol;i++){
    pthread_join(producerid[i], NULL);
    cout << "Producer " << prod[i].prod_id << " has finished " << prod[i].job_count << " jobs."<<endl;
  }
  for (int i=0;i<consumer_vol;i++){
    pthread_join(consumerid[i], NULL);
    cout << "Consumer " << cons[i].cons_id << " is done."<<endl;
  }
  pthread_exit(NULL);
  return 0;
}

void *producer(void* prod) 
{
  Producer* prod_param = (Producer*) prod;
  int job_vol = prod_param->job_count;
  Job** job_array = prod_param->job_arr;
  cout << "Producer = " << prod_param->prod_id << " " << job_vol << endl;
  
  //for(int i=0;i<job_vol;i++){
  //   int t = rand() % 10+1;
  //   Job job_x = {i,t};
  
  pthread_exit(NULL);
}

void *consumer (void* cons) 
{
  Consumer* cons_param = (Consumer*) cons;
  cout << "Consumer = " << cons_param->cons_id << endl;
  pthread_exit (NULL);
}
