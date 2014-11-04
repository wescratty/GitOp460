/* * * * * * * * * * * * * *
 * Wes Cratty
 * OPSys 460
 * Prog6 producerConsumer.c
 * ---------------
 * Create a shell
 * * * * * * * * * * * * * */

#include <sys/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>


#define TRUE 1

/* Let's assign a fancy name to our items to be
 produced and consumed. Now we can declare of variable
 of type buffer_item when we want to produce or consume
 an item*/
typedef int buffer_item;
#define BUFFER_SIZE 5

/* prototypes for insertion and removal functions  */
void insert_item(buffer_item item);
void remove_item(buffer_item *item);

/* declare buffer and three semaphores */
buffer_item buffer[BUFFER_SIZE];
pthread_mutex_t mutex;
sem_t empty;
sem_t full;

/*  Pointers for keeping track of where left off
 when inserting and removing */

int insertPointer = 0, removePointer = 0;

/*  prototypes for thread functions. When create a thread you
 pass the creation function a pointer to the function
 that you want the  thread to execute. These are them
 and they are definded below.*/
void *producer(void *param);
void *consumer(void *param);

pthread_t ptid[sizeof(int)];
pthread_t ctid[sizeof(int)];
pthread_attr_t pattr[sizeof(int)];
pthread_attr_t cattr[sizeof(int)];


/*void checkError(int err, char name);*/

int main(int argc, char *argv[]){
    
    int runTime = 0;
    int newProducers = 0;
    int newConsumers = 0;
    int i = 0;
    int err;
    
    if (argc!=4) {
        printf("Inorrect Parameters argc\n");
        exit(0);
    }
        
    runTime=atoi (argv[1]);
    newProducers=atoi (argv[2]);
    newConsumers=atoi (argv[3]);
    
    printf("\nrunTime %d",runTime);
    printf("\nnewProducers %d",newProducers);
    printf("\nnewConsumers %d\n",newConsumers);
    
    /* Initialize the three locks */
    
    err = pthread_mutex_init(&mutex, NULL);
    /*checkError(err, 'mutex')*/
    if (err != 0){
        printf("\ncan't create mutex :[%d]", errno);
    }else{
        printf("\nsuccessfully initialized the mutex");
        
    }

    err = sem_init(&empty, 0, BUFFER_SIZE);
    if ((err!=0)){
       
        printf("\ncan't create empty :[%d]",err );
    }else{
        printf("\nsuccessfully initialized the empty");
        
    }
    err = sem_init(&full, 0, 0);
    if ((err!=0)){
        printf("\ncan't create full :[%d]", err);
    }else{
        printf("\nsuccessfully initialized the full");
        
    }
        
 
    /* seed the random number generator with the time */
    srand(time(0));
    
    /* Create the producer threads*/
    /* loop from 0 while less than number of producers */
    while (i<newProducers) {
        i++;
        pthread_attr_init(&pattr[i]);
        err = pthread_create(&(ptid[i]), &pattr[i],producer, NULL);
        if (err != 0){
            printf("\ncan't create producer :[%d]", err);
        }else{
            printf("\ncreated producer thread id:[%p]", (void *)pthread_self());
        }
    }
    i=0;
    /* Create the consumer threads */
    /* loop from 0 while less than number of producers */
    
    while (i<newConsumers) {
        i++;
       
        
        pthread_attr_init(&cattr[i]);
        err = pthread_create(&(ctid[i]), &cattr[i],consumer, NULL);
        if (err != 0){
            printf("\ncan't create consumer :[%d]", err);
        }else{
            printf("\ncreated consumer thread id:[%p]",(void *)pthread_self());
        }
    }
    i=0;
        /* Put this (the main) thread to sleap for a period of
     time equal to the user passed argument for run time*/
    Threed.sleep(runTime);
    
    return 0;
}

void *producer(void *param)
{
    /* create a variable for a randomly generated buffer item */
    int randBuf =0;
    
    /*  create a variable for randomly generated period of time
     that producer will sleep just prior to producing each
     item */
    int *sleepTime =0;
    
    /* infinite loop */
    while(TRUE){
        /* generate a random number between 0 and 5 */
        sleepTime = (rand()%5);
        pthread_self().sleep(sleepTime);
        
        /* sleep for that period of time */
        
        /*  generate a random number and store in variable
         created above */
        randBuf= (rand()%5);
        /*  Insert the item into the buffer using the insert_item
         method (that you have to create, see below)*/
        insert_item(randBuf);
        printf("\n producer\n");

    }
}

void *consumer(void *param){
    /* create a variable for a randomly generated buffer item */
    int randBuf =0;
    /*  create a variable for randomly generated period of
     time that producer will sleep just prior to
     producing each item */
    int *sleepTime =0;
    
    /* infinite loop */
    while(TRUE){
        /* generate a random number between 0 and 5 */
        
        /* sleep for that period of time */
        sleepTime = (rand()%5);
        sleep(sleepTime);
                /* remove an item from the buffer utilizing the remove_item
         method (that you have to create, see below) */
        randBuf= (rand()%5);

        remove_item(&randBuf);
        printf("\n consumer\n");

    }
}


void insert_item(buffer_item item){
    /* Acquire (wait on) Empty Semaphore */
    sem_wait(&empty);
  
    
    /* Acquire mutex lock to protect buffer */
    pthread_mutex_lock(&mutex);
    /*  Set the buffer at location "insertPointer" equal to the
     passed item */
    
    buffer[insertPointer] = item;
    /* Increment the insertPointer */
    insertPointer++;
    /*  Mod the insertPointer with the buffer size in case it has
     to wrap around to beginning of buffer */
    insertPointer=insertPointer%BUFFER_SIZE;
    /* Release mutex lock and full semaphore */
    pthread_mutex_unlock(&mutex);

    sem_post(&full);
    printf("\n insert\n");
}


void remove_item(buffer_item *item){
    /* Acquire (wait on) Full Semaphore */
    sem_wait(&full);
   
    /* Acquire mutex lock to protect buffer */
    pthread_mutex_lock(&mutex);
    /* Acquire item at location "removePointer" from buffer */
     buffer[insertPointer] = -1;
    /* Set value at that location in buffer to -1 */
    
    /* Increment "removePointer" */
    removePointer++;
    /*  Mod removePointer with the size of of the buffer in case
     have wrapped in the buffer */
    removePointer=removePointer%BUFFER_SIZE;
    /* Release mutex lock and empty semaphore */
    pthread_mutex_unlock(&mutex);

    sem_post(&empty);
    printf("\n remove\n");
    
}
/*void checkError(int err, char name)
{
    if (err != 0){
        printf("\ncan't create %s :[%d]",name, err);
    }else{
        printf("\ncreated %s thread id:[%p]"name,(void *)pthread_self());
    }
}*/