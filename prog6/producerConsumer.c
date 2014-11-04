/* * * * * * * * * * * * * *
 * Wes Cratty
 * OPSys 460
 * Prog6 producerConsumer.c
 * ---------------
 * Practice using mutex and semiphores
 * * * * * * * * * * * * * */


#include <sys/stat.h>
#include <fcntl.h>
#include <sys/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>


#define TRUE 1
#define BUFFER_SIZE 5

/* Declare fancy type */
typedef int buffer_item;                                

/* prototypes for insertion and removal functions  */
void insert_item(buffer_item item);
void remove_item(buffer_item *item);

/* declare buffer and three semaphores */
buffer_item buffer[BUFFER_SIZE];
pthread_mutex_t mutex;
sem_t *empty2;
sem_t *full2;


/*  Pointers for keeping track of where left off when inserting and removing */
int insertPointer = 0, removePointer = 0,cons = 0,prod = 0;

/*  prototypes for thread functions. */
void *producer(void *param);
void *consumer(void *param);

/* Declare thread ids and attributes */
pthread_t ptid[sizeof(int)];
pthread_t ctid[sizeof(int)];
pthread_attr_t pattr[sizeof(int)];
pthread_attr_t cattr[sizeof(int)];
int tid[sizeof(int)];

/* Prototype for function to destroy threads */
void cleanUp(int);

/* ---------------------------------------------------------------*/
int main(int argc, char *argv[]){
    
    int runTime = 0;                    /* Variables for use */
    int newProducers = 0;
    int newConsumers = 0;
    int i = 0;
    int err;
    
    
    
    /* Signal if contro+c to call cleanup */
    signal(SIGINT, cleanUp);            

    /* Check that there are the correct amount of sent args */
    if (argc!=4) {
        printf("Inorrect Parameters argc\n");
        exit(0);
    }
    
    /* Get integer value's from argv */
    runTime=atoi (argv[1]);
    newProducers=atoi (argv[2]);
    newConsumers=atoi (argv[3]);
    
    /* Initialize the three locks and check for errors */

    err = pthread_mutex_init(&mutex, NULL);                                         /* store result in err */
    if (err != 0){                                                                  /* Check for success */
        printf("\ncan't create mutex :[%d]", errno);                                /* Client message fail */
    }else{
        printf("\nsuccessfully initialized the mutex");                             /* Client message success */
    }
    
    empty2 = sem_open("/empty2", O_CREAT, 0644, BUFFER_SIZE);                       /* Open semiphore */
    if ((empty2==SEM_FAILED)){                                                      /* Check for success */
        perror("sem_open1");                                                        /* Client message fail */
    }else{
        printf("\nsuccessfully initialized the empty");                             /* Client message success */
    }
    
    full2 = sem_open("/full2", O_CREAT, 0644, 0);                                   /* Open semiphore */
    if ((full2==SEM_FAILED)){                                                       /* Check for success */
        perror("sem_open2");                                                        /* Client message fail */
    }else{
        printf("\nsuccessfully initialized the full");                              /* Client message success */
    }
    
    
    srand(time(0));                                                                 /* seed random generator with time */
    
    
    /* Create the producer threads*/
    
    while (i<newProducers) {                                                        /* loop for sent number of times */
        i++;
        
        pthread_attr_init(&pattr[i]);                                               /* instantiate attribute at pointer location */
        err =pthread_create(&(ptid[i]), &pattr[i],producer, NULL);                  /* Create the threead */
        if (err != 0){
            printf("\ncan't create producer :[%d]", err);                           /* Client message fail */
        }else{
            printf("\ncreated producer thread id:[%p]",(void*)pthread_self());      /* Client message success */
        }
    }
    i=0;                                                                            /* Reset i */
    
    
    /* Create the consumer threads */
    
    while (i<newConsumers) {                                                        /* loop for sent number of times */
        i++;
       
        pthread_attr_init(&cattr[i]);                                               /* instantiate attribute at pointer location */
        err = pthread_create(&(ctid[i]), &cattr[i],consumer, NULL);                 /* Create the threead */
        if (err != 0){
            printf("\ncan't create consumer :[%d]", err);                           /* Client message fail */
        }else{
            printf("\ncreated consumer thread id:[%p]",(void*)pthread_self());      /* Client message success */
        }
    }
    i=0;                                                                            /* Reset i */
    
    sleep(runTime);                                                                 /* Put main thread to sleap */
    cleanUp(0);                                                                     /* Call to close and unlink threads*/
    return 0;                                                                       /* end program */
}

/* ---------------------------------------------------------------*/
void *producer(void *param){                                                        /* Recieve pointer to thread */
    
    int randBuf =0;                                                                 /* make a few variables */
    int sleepTime =0;
    
    while(TRUE){                                                                    /* infinite loop */
        sleepTime = (rand()%5);                                                     /* generate random num under 6 */
        sleep(sleepTime);                                                           /* sleep that long */
        randBuf= (rand()%5);                                                        /* generate random num under 6 */
        insert_item(randBuf);                                                       /* call to insert the rand */
    }
}

/* ---------------------------------------------------------------*/
void *consumer(void *param){

    int randBuf =0;
    int sleepTime =0;
    
    while(TRUE){                                                                    /* infinite loop */
        sleepTime = (rand()%5);                                                     /* generate random num under 6 */
        sleep(sleepTime);                                                           /* sleep that long */
        randBuf= (rand()%5);                                                        /* generate random num under 6 */
        remove_item(&randBuf);                                                      /* call to insert the rand */
    }
}

/* ---------------------------------------------------------------*/
void insert_item(buffer_item item){

    sem_wait(empty2);                                                               /* Decrement empty, if no slots producer waits. */
    pthread_mutex_lock(&mutex);                                                     /* prevent other threads from entering code */
    /* ========== Protected area =========== */

    buffer[insertPointer] = item;                                                   /* insert item to a location in the buffer*/
    printf("\n insert %d\n",buffer[insertPointer]);                                 /* tell what number is being inserted */
    insertPointer++;
    insertPointer=insertPointer%BUFFER_SIZE;                                        /* incase incremented passed buffer size; mod to wrap */
    
    /* ========== Protected area =========== */

    pthread_mutex_unlock(&mutex);                                                   /* unlock mutex to allow others in */
    sem_post(full2);                                                                /* add one to full */
}

/* ---------------------------------------------------------------*/
void remove_item(buffer_item *item){
    
    sem_wait(full2);                                                                /* decrement full; consumer waits if nothing to consume. */
    pthread_mutex_lock(&mutex);                                                     /* prevent other threads from entering code */
    /* ========== Protected area =========== */

    printf("\n remove %d\n",buffer[removePointer]);                                 /* tell what number is being removed; if -1 then it is an item that was inserted by consumer */
    buffer[removePointer] = -1;                                                     /* store -1. "I was here" */
    removePointer++;
    removePointer=removePointer%BUFFER_SIZE;                                        /* incase incremented passed buffer size; mod to wrap */
    
    /* ========== Protected area =========== */

    pthread_mutex_unlock(&mutex);                                                   /* unlock mutex to allow others in */
    sem_post(empty2);                                                               /* add one to empty */
}

/* ---------------------------------------------------------------*/
void cleanUp(int dummy) {
   
    sem_close(empty2);                                                              /* close semiphore */
    sem_close(full2);                                                               /* close semiphore */
    sem_unlink("/empty2");                                                          /* unlink semiphore */
    sem_unlink("/full2");                                                           /* unlink semiphore */
    /*fprintf(stderr,"\n Clean up compleate");*/
    /*pthread_exit(NULL);*/
    exit(0);
    
}

