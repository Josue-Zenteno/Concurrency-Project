/*
====================================================================
Josue Carlos Zenteno Yave and Sergio Silvestre Pavon
Concurrent and Real-Time Programming
Faculty of Computer Science
University of Castilla-La Mancha (Spain)
====================================================================
*/

#include <errno.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <definitions.h>
#include <semaphoreI.h>

/* Semaphores and shared memory retrieval */
void get_shm_segments(int *shm_chairs, struct TData_t **data);

void get_sems(sem_t **p_sem_new_client, sem_t **p_sem_haircut_done, sem_t **p_sem_mutex, sem_t **p_sem_barber_chair);

/* Task management */

void clienting(sem_t *sem_new_client, sem_t *sem_haircut_done, sem_t *sem_barber_chair ,sem_t *sem_mutex , struct TData_t *data);

/******************** Main function ********************/

int main(int argc, char *argv[]) {
  struct TData_t *data;
  int shm_chairs;
  sem_t *sem_new_client, *sem_haircut_done, *sem_mutex, *sem_barber_chair;
 
  /* Get shared memory segments and semaphores */
  get_shm_segments(&shm_chairs,&data);
  get_sems(&sem_new_client, &sem_haircut_done, &sem_mutex,&sem_barber_chair);
  
  clienting(sem_new_client,sem_haircut_done,sem_barber_chair,sem_mutex ,data);
  return EXIT_SUCCESS;
}

/******************** Semaphores and shared memory retrieval ********************/

void get_shm_segments(int *shm_chairs, struct TData_t **data) {
  *shm_chairs = shm_open(SHM_CHAIRS, O_RDWR, 0644);
  *data = mmap(NULL, sizeof(struct TData_t), PROT_READ | PROT_WRITE, MAP_SHARED, *shm_chairs, 0);
}

void get_sems(sem_t **p_sem_new_client, sem_t **p_sem_haircut_done,sem_t **p_sem_mutex,sem_t **p_sem_barber_chair) {
  *p_sem_new_client = get_semaphore(SEM_NEW_CLIENT);
  *p_sem_haircut_done = get_semaphore(SEM_HAIRCUT_DONE);
  *p_sem_mutex = get_semaphore(SEM_MUTEX);
  *p_sem_barber_chair = get_semaphore(SEM_BARBER_CHAIR);
}

/******************** Task management ********************/

void clienting(sem_t *sem_new_client, sem_t *sem_haircut_done, sem_t *sem_barber_chair ,sem_t *sem_mutex , struct TData_t *data) {
   
   srand((int)getpid());  //Generating random spawn time
   sleep(1 + (rand()%10));
  
   printf("[Client %d]    Gets in the barbershop.\n",getpid());

   wait_semaphore(sem_mutex);
   if (data->chairs == 0){
      printf("[Client %d]    There is not any free chair. Leaves the barbershop\n",getpid());
      signal_semaphore(sem_mutex); //If we don't free the mutex when this client goes, the other clients will be blocked
    }else{
      printf("[Client %d]    Takes a seat in the barbershop (%d chairs left).\n",getpid(),data->chairs);
      sleep(1);
      data->chairs = data->chairs - 1; //We decremnt the number of chair as this client is using one chair
      printf("[Client %d]    Seats on an empty chair.\n",getpid());
      sleep(1);
      signal_semaphore(sem_mutex); 

      wait_semaphore(sem_barber_chair); 
      
          wait_semaphore(sem_mutex);
              data->chairs = data->chairs + 1;
              printf("[Client %d]    Frees the chair.\n",getpid());
              sleep(1);
          signal_semaphore(sem_mutex);
          printf("[Client %d]    Wakes up the barber.\n",getpid());
          sleep(1);
          signal_semaphore(sem_new_client); //Rendezvous
          wait_semaphore(sem_haircut_done); //Rendezvous
          printf("[Client %d]    Leaves the barbershop (%d chairs left).\n",getpid(),data->chairs);
          sleep(1);
      signal_semaphore(sem_barber_chair);
    }

}
