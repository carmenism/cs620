/**
 * CS 620, Fall 2012
 * Assignment 8
 * 
 * barber.c
 *
 * Simulates the barber shop problem introduce thread synchronization system 
 * calls.
 * 
 * A barber shop consists of a waiting room with n chairs, and the barber room
 * containing the barber chair. If there are no customers to be served, the 
 * barber goes to sleep. If a customer enters the barber shop and all chairs 
 * are taken, then the customers will leave the shop. If the barber is busy, 
 * but chairs are available, then the customer sits in one of the free chairs.
 * If the barber is asleep, the customer wakes up the barber. Write a program
 * using semaphores to coordinate the barber and the customers.
 *
 * @author Carmen St. Jean (crr8)
 * @date November 9, 2012
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>

#define  SPACE      2    // Number of chairs in the barber shop
#define  CUSTOMERS  100  // Maximum number of customers allowed during a 
                         // simulation run.
#define  SECONDS    60
#define  ERROR      -1

void   *customerthread( void *arg );
void   *barberthread( void *arg );
double drand48();
void   srand48(long seedval);
long   get_time();
long   get_current_time();

typedef enum { false, true } bool;

int start_time;
int duration;
int meanServiceTime;
  
// shared variables and semaphore variables
int vacant_chairs;

sem_t mutex;
sem_t barber_chair;
sem_t customer_in;
sem_t hair_cut_done;
   
int main(int argc, char *argv[])
{
   pthread_t  customer[CUSTOMERS];  // Customer thread ids.
   pthread_t  barber;               // Barber thread id
   long       end_time;
   long       current_time;
   double     random, exp_no;
   long       seed_value;  /* Initialization of random generator */
      
   // other variables here
   vacant_chairs = SPACE + 1;
   int customerNum = 0;
   
   int arrival = 5;
   duration = 1;
   meanServiceTime = 10;
   
   /* User enters duration of program execution (in minutes) */
   printf("Enter duration of program execution (in minutes): ");
   scanf("%d", &duration);   
  
   /* User enters the customer inter-arrival time (in seconds) */
   printf("Enter customer mean inter-arrival time (in seconds): ");
   scanf("%d", &arrival);   
  
   /* User enters mean service time for the barber (in seconds) */
   printf("Enter mean service time for the barber (in seconds): ");
   scanf("%d", &meanServiceTime);   

   // Initialize all semaphores.
   if (sem_init(&mutex, 1, 1) == ERROR) {
       perror("Could not initialize mutex sem");
       exit(1);
   }
  
   if (sem_init(&barber_chair, 1, 1) == ERROR) {
       perror("Could not initialize barber_chair sem");
       exit(1);
   }
   
   if (sem_init(&customer_in , 1, 0) == ERROR) {
       perror("Could not initialize customer_in sem");
       exit(1);
   }
  
   if (sem_init(&hair_cut_done , 1, 0) == ERROR) {
       perror("Could not initialize hair_cut_done sem");
       exit(1);
   }  

   printf("***********************************************************\n");
   printf("TOTAL WAITING SPACE IN BARBER SHOP = %d\n\n", SPACE);

   /************* MODEL SIMULATION ************************************/
   /* Initialize random sequence */
   seed_value = get_current_time();  /* seconds since Jan. 1, 1970 */
   srand48(seed_value);  /* Initialization entry pt. for drand48 */

   /* Start of main simulation */
   current_time = get_current_time();
   start_time = current_time;
   duration = SECONDS * duration;
   end_time = current_time + duration;
   printf("Simulation start time = %d\n", get_time());
      
   // Create barber thread here.
   if (pthread_create(&barber, NULL, barberthread, NULL) != 0) {
       perror("Barber pthread_create error");
       exit(1);
   }
  
   while (get_current_time() <= end_time)
   {
       // Customers arrive randomly.  The interarrival time distribution
       // is exponential with mean equal to arrival.
       /* random number generated must not be a 0 */
       while ((random = drand48()) == 0);
       /* Generated an exponentially distributed random variable *
        * with rate as 1/arrival                                 */
       exp_no = (-arrival) * log(random);
       // Calling program sleeps during the interarrival time.
       sleep((int)exp_no);

       // Create customer thread and other stuff here
       customerNum = customerNum + 1;
     
       if (pthread_create(&customer[customerNum], NULL, 
                          customerthread, (void *)customerNum) != 0) {
           perror("Customer pthread_create error");
           exit(1);        
       }
   }
   
   printf("***********************************************************\n");
   
   // Kill all customer threads.
   int i;
   for (i = 1; i <= customerNum; i++)
   {
       pthread_kill(&customer[i]);
   }
   
   // Kill the barber thread.
   pthread_kill(&barber);
   
   printf("End of simulation run.\n");
   printf("***********************************************************\n");
   
   return 0;
}

//  CUSTOMER THREAD
void *customerthread( void *arg )
{
  int customerNo = (int) arg;

  bool there_are_no_vacant_chairs = false;  
  
  if (get_time() > duration) {
     pthread_exit(NULL);
  }
  
  printf("Customer %d arrives at time %d.\n", customerNo, get_time());
  
  // Entering customer checks number of vacant chairs
  // Since vacant_chairs is a shared variable, it should be accessed only
  // within a critical section.
  if (sem_wait(&mutex) == ERROR) { // Enter CS.
      perror("Invalid mutex wait");
      pthread_exit(NULL); 
  }
  if (vacant_chairs == 0) {
      there_are_no_vacant_chairs = true;
      
      if (get_time() > duration) {
         pthread_exit(NULL);
      }
      
      printf("Customer %d leaves since no vacant seats at time %d.\n", 
             customerNo, get_time());
  } else {
      // Customer takes the seat.
      vacant_chairs = vacant_chairs - 1;
  }
  if (sem_post(&mutex) == ERROR) { // Exit CS.
      perror("Invalid mutex post");
      pthread_exit(NULL);
  }  
  
  if (there_are_no_vacant_chairs == true) {
      pthread_exit(NULL);
  }
  
  // The customer now tries to get the barber chair.
  if (sem_wait(&barber_chair) == ERROR) { // Block if barber chair is occupied.
      perror("Invalid barber_chair wait"); 
      pthread_exit(NULL); 
  }
      // Now you have the chair.  Wake the barber.
      if (sem_post(&customer_in) == ERROR) {
          perror("Invalid customer_in post");
      }
            
      if (get_time() > duration) {
         pthread_exit(NULL);
      }
      
      printf("Customer %d gets barber at time %d.\n", customerNo, get_time());
      // Wait for barber to finish your haircut.
      if (sem_wait(&hair_cut_done) == ERROR) {
          perror("Invalid hair_cut_done wait");
          pthread_exit(NULL); 
      }
  if (sem_post(&barber_chair) == ERROR) {  // Vacate barber's chair, wake a
      perror("Invalid barber_chair post"); // client waiting for the barber.
      pthread_exit(NULL); 
  }
  
  // Exit the barber shop.  Remember to increment vacant chairs.
  if (sem_wait(&mutex) == ERROR) { // Enter CS.
      perror("Invalid mutex wait");
      pthread_exit(NULL); 
  }
      vacant_chairs = vacant_chairs + 1; // Customer vacates the seat.
  if (sem_post(&mutex) == ERROR) { // Exit CS.
      perror("Invalid mutex post");
      pthread_exit(NULL); 
  } 
  
  if (get_time() > duration) {
      pthread_exit(NULL);
  }
  
  printf("Customer %d leaves the barber shop at time %d.\n", 
         customerNo, get_time());
}

// BARBER THREAD
void *barberthread( void *arg )
{
   double random;

   printf("Barber opens shop at time %d\n", get_time());

   for (;;)
   {
      // Wait for a customer to arrive
      if (sem_wait(&customer_in) == ERROR) {
          perror("Invalid customer_in wait");
          pthread_exit(NULL); 
      }

      // Time taken for the haircut is a random number
      // The mean service time is input from the user.
      // This code is similar to customer inter-arrival time generator.
      while ((random = drand48()) == 0);
      /* Generated an exponentially distributed random variable *
       * with rate as 1/arrival                                 */
      double exp_time = (-meanServiceTime) * log(random);
      
      sleep((int) exp_time);
      
      // Notify the customer that hair-cut done.
      if (sem_post(&hair_cut_done) == ERROR) {
          perror("Invalid hair_cut_done post");
          pthread_exit(NULL);
      }
   }
}

/**
 * Returns the current time minus the start time.
 */
long get_time() {
    return get_current_time() - start_time;
}

/**
 * Returns the current time as seconds since Jan 1, 1970.
 */
long get_current_time()
{
  int    flag;
  long   current_time;
  struct timeval tp;
  struct timezone tzp;

  flag = 0;
  if ((flag = gettimeofday(&tp, &tzp)) == ERROR)
  {
    fprintf(stderr, "\ngettimeofday: error returned.\n");
    exit(ERROR);
  }
  current_time = (long)tp.tv_sec;  /* seconds since Jan. 1, 1970 */
  return(current_time);
}



