/*
====================================================================
Josue Carlos Zenteno Yave and Sergio Silvestre Pavon
Concurrent and Real-Time Programming
Faculty of Computer Science
University of Castilla-La Mancha (Spain)
====================================================================
*/

#define _POSIX_SOURCE
#define _BSD_SOURCE

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
#include <sys/wait.h>

#include <definitions.h>
#include <semaphoreI.h>

/* Total number of processes */
int g_nProcesses;
/* 'Process table' (child processes) */
struct TProcess_t *g_process_table;

/* Process management */
void create_processes_by_class(enum ProcessClass_t class, int n_processes, int index_process_table);
pid_t create_single_process(const char *class, const char *path, const char *argv);
void get_str_process_info(enum ProcessClass_t class, char **path, char **str_process_class);
void init_process_table(int n_clients, int n_barbers);
void terminate_processes();
void wait_processes(int n_clients);

/* Semaphores and shared memory management */
void create_shm_segments(int *shm_chairs, struct TData_t **p_data, int *n_chairs);

void create_sems();
void close_shared_memory_segments(int shm_chairs);




/* Auxiliar functions */
void free_resources();
void install_signal_handler();
void parse_argv(int argc, char *argv[], int *n_clients, int *n_chairs);
void signal_handler(int signo);

/******************** Main function ********************/

int main(int argc, char *argv[]) {
  struct TData_t *data;
  int shm_chairs;
  int n_clients, n_chairs;

  /* Install signal handler and parse arguments*/
  install_signal_handler();
  parse_argv(argc, argv, &n_clients, &n_chairs);

  /* Init the process table*/
  init_process_table(n_clients, NUM_BARBERS);

  /* Create shared memory segments and semaphores */
  create_shm_segments(&shm_chairs,&data,&n_chairs);
  create_sems();

  /* Create processes */
  create_processes_by_class(BARBER, NUM_BARBERS, 0);
  create_processes_by_class(CLIENT, n_clients, NUM_BARBERS);
  

  /* Wait proccesess termination */
  wait_processes(n_clients);

  /* Free resources and terminate */
  close_shared_memory_segments(shm_chairs);
  terminate_processes();
  free_resources();

  return EXIT_SUCCESS;
}

/******************** Process Management ********************/

void create_processes_by_class(enum ProcessClass_t class, int n_processes, int index_process_table) {
  char *path = NULL, *str_process_class = NULL;
  int i;
  pid_t pid;

  get_str_process_info(class, &path, &str_process_class);

  for (i = index_process_table; i < (index_process_table + n_processes); i++) {
    pid = create_single_process(path, str_process_class, NULL);

    g_process_table[i].class = class;
    g_process_table[i].pid = pid;
    g_process_table[i].str_process_class = str_process_class;
  }

  //printf("[MANAGER] %d %s processes created.\n", n_processes, str_process_class);
  sleep(1);
}

pid_t create_single_process(const char *path, const char *class, const char *argv) {
  pid_t pid;

  switch (pid = fork()) {
  case -1 :
    fprintf(stderr, "[MANAGER] Error creating %s process: %s.\n", 
	    class, strerror(errno));
    terminate_processes();
    free_resources();
    exit(EXIT_FAILURE);
    /* Child process */
  case 0 : 
    if (execl(path, class, argv, NULL) == -1) {
      fprintf(stderr, "[MANAGER] Error using execl() in %s process: %s.\n", 
	      class, strerror(errno));
      exit(EXIT_FAILURE);
    }
  }

  /* Child PID */
  return pid;
}

void get_str_process_info(enum ProcessClass_t class, char **path, char **str_process_class) {
  switch (class) {
  case CLIENT:
    *path = CLIENT_PATH;
    *str_process_class = CLIENT_CLASS;
    break;
  case BARBER:
    *path = BARBER_PATH;
    *str_process_class = BARBER_CLASS;
    break;
  }
}

void init_process_table(int n_clients, int n_barbers) {
  int i;

  /* Number of processes to be created */
  g_nProcesses = n_clients + n_barbers; 
  /* Allocate memory for the 'process table' */
  g_process_table = malloc(g_nProcesses * sizeof(struct TProcess_t)); 

  /* Init the 'process table' */
  for (i = 0; i < g_nProcesses; i++) {
    g_process_table[i].pid = 0;
  }
}

void terminate_processes() {
  int i;
  
  printf("\n----- [MANAGER] Terminating running child processes ----- \n\n");
  for (i = 0; i < g_nProcesses; i++) {
    /* Child process alive */
    if (g_process_table[i].pid != 0) { 
      printf("[MANAGER] Terminating %s process [%d]...\n", 
	     g_process_table[i].str_process_class, g_process_table[i].pid);
      if (kill(g_process_table[i].pid, SIGINT) == -1) {
	fprintf(stderr, "[MANAGER] Error using kill() on process %d: %s.\n", 
		g_process_table[i].pid, strerror(errno));
      }
    }
  }
}

/******************** Process Termination ********************/
void wait_processes(int n_clients){
  int i;
  pid_t pid;

  while(n_clients > 0 ){
    pid = wait(NULL);
    for(i=0 ;i<g_nProcesses;i++){
      if(pid == g_process_table[i].pid){
        g_process_table[i].pid = 0;
        if(g_process_table[i].class == CLIENT){
          n_clients--;
        }
        break;
      }
    }
  }
}
/******************** Semaphores and shared memory management ********************/

void create_shm_segments(int *shm_chairs, struct TData_t **p_data, int *n_chairs) {
 
  /* Create and initialize shared memory segments */
  *shm_chairs = shm_open(SHM_CHAIRS, O_CREAT | O_RDWR, 0644); 
  ftruncate(*shm_chairs, sizeof(struct TData_t));          
  *p_data = mmap(NULL, sizeof(struct TData_t), PROT_READ | PROT_WRITE, MAP_SHARED, *shm_chairs, 0);

  /* Load encoded data */
  (*p_data)->chairs = *n_chairs;
}

void create_sems() {  
  /* The manager process only initializes the rest, but it does not use them */
  create_semaphore(SEM_BARBER_CHAIR, 1); 
  create_semaphore(SEM_NEW_CLIENT, 0);
  create_semaphore(SEM_HAIRCUT_DONE, 0);
  create_semaphore(SEM_MUTEX, 1);
}

void close_shared_memory_segments(int shm_chairs) {
  close(shm_chairs);
}

/******************** Auxiliar functions ********************/

void free_resources() {
  printf("\n----- [MANAGER] Freeing resources ----- \n");

  /* Free the 'process table' memory */
  free(g_process_table); 

  /* Semaphores */ 
  remove_semaphore(SEM_BARBER_CHAIR);
  remove_semaphore(SEM_NEW_CLIENT);
  remove_semaphore(SEM_HAIRCUT_DONE);
  remove_semaphore(SEM_MUTEX);


  /* Shared memory segments*/
  shm_unlink(SHM_CHAIRS);
}

void install_signal_handler() {
  if (signal(SIGINT, signal_handler) == SIG_ERR) {
    fprintf(stderr, "[MANAGER] Error installing signal handler: %s.\n", strerror(errno));    
    exit(EXIT_FAILURE);
  }
}

void parse_argv(int argc, char *argv[], int *n_clients, int *n_chairs) {
  if (argc != 3) {
    fprintf(stderr, "Synopsis: ./exec/manager <n_clients> <n_chairs>.\n");    
    exit(EXIT_FAILURE); 
  }
  *n_clients = atoi(argv[1]);  
  *n_chairs = atoi(argv[2]);
}

void signal_handler(int signo) {
  printf("\n[MANAGER] Program termination (Ctrl + C).\n");
  terminate_processes();
  free_resources();
  exit(EXIT_SUCCESS);
}
