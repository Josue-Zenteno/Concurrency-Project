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

void get_sems(sem_t **p_sem_new_client, sem_t **p_sem_haircut_done);

/* Task management */

void work(sem_t *sem_new_client, sem_t *sem_haircut_done);

/******************** Main function ********************/

int main(int argc, char *argv[]) {

  sem_t *sem_new_client, *sem_haircut_done;

  /* Get shared memory segments and semaphores */
  get_sems(&sem_new_client, &sem_haircut_done);
  
  /* Will work until killed by the manager */
  while (TRUE) {
    work(sem_new_client,sem_haircut_done);
  }

  return EXIT_SUCCESS;
}

/******************** Semaphores and shared memory retrieval ********************/

void get_sems(sem_t **p_sem_new_client, sem_t **p_sem_haircut_done) {

  *p_sem_new_client = get_semaphore(SEM_NEW_CLIENT);
  *p_sem_haircut_done = get_semaphore(SEM_HAIRCUT_DONE);
 
}

/******************** Task management ********************/

void work(sem_t *sem_new_client, sem_t *sem_haircut_done) {
    printf("[Barber]          Barber sleeping zZzZzZz\n");
    wait_semaphore(sem_new_client);
        //Simulating the haircut
        printf("[Barber]          Barber cutting hair\n");
        sleep(3);   //Simulating complexity
    signal_semaphore(sem_haircut_done);
    printf("[Barber]          Barber has finished the haircut\n");
}

