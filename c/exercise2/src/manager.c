/*
====================================================================
Josue Carlos Zenteno and Sergio Silvestre Pavon
Concurrent and Real-Time Programming
Faculty of Computer Science
University of Castilla-La Mancha (Spain)
====================================================================
*/

#define _POSIX_SOURCE
#define _BSD_SOURCE

#include <errno.h>
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <definitions.h>

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

/* Message queue management */
void create_message_queue(const char *mq_name, mode_t mode, long mq_maxmsg, long mq_msgsize,mqd_t *q_handler);

void close_message_queues(mqd_t q_handler_new_client, mqd_t q_handler_haircut_done,
mqd_t q_handler_barber_chair, mqd_t q_handler_chairs, mqd_t q_handler_mutex);

/* Task management */

void send_number_of_chairs(struct MsgBarbershop_t *msg_chairs, mqd_t q_handler_chairs, mqd_t q_handler_barber_chair , mqd_t q_handler_mutex);

/* Auxiliar functions */
void free_resources();
void initiate_number_of_chairs(struct MsgBarbershop_t **msg_chairs, int *n_chairs);
void install_signal_handler();
void parse_argv(int argc, char *argv[], int *n_clients, int *n_chairs);
void signal_handler(int signo);

/******************** Main function ********************/

int main(int argc, char *argv[]) {
  mqd_t q_handler_new_client, q_handler_haircut_done;
  mqd_t q_handler_barber_chair, q_handler_chairs, q_handler_mutex;

  mode_t mode_creat_read_write = (O_RDWR | O_CREAT);
  
  struct MsgBarbershop_t *msg_chairs;
  int  n_clients, n_chairs;

  /* Install signal handler and parse arguments*/
  install_signal_handler();
  parse_argv(argc, argv, &n_clients, &n_chairs);
  /* Init the process table*/
   init_process_table(n_clients, NUM_BARBERS);

  /* Create message queues */
  create_message_queue(MQ_NEW_CLIENT, mode_creat_read_write,1,sizeof(struct MsgInteger_t), &q_handler_new_client);
  create_message_queue(MQ_HAIRCUT_DONE, mode_creat_read_write,1,sizeof(struct MsgInteger_t), &q_handler_haircut_done);
  create_message_queue(MQ_BARBER_CHAIR, mode_creat_read_write,1,sizeof(struct MsgInteger_t), &q_handler_barber_chair);
  create_message_queue(MQ_CHAIRS, mode_creat_read_write,1,sizeof(struct MsgBarbershop_t), &q_handler_chairs);
  create_message_queue(MQ_MUTEX, mode_creat_read_write,1,sizeof(struct MsgInteger_t), &q_handler_mutex);

  /* Generate a message with the input data */
  initiate_number_of_chairs(&msg_chairs, &n_chairs);
  send_number_of_chairs(msg_chairs,q_handler_chairs,q_handler_barber_chair,q_handler_mutex);

  /* Create processes */
  create_processes_by_class(BARBER, NUM_BARBERS, 0);
  create_processes_by_class(CLIENT, n_clients, NUM_BARBERS);

  /* Wait for the decoder process */
  wait_processes(n_clients);

  /* Free resources and terminate */
  close_message_queues(q_handler_new_client,q_handler_haircut_done,q_handler_barber_chair,q_handler_chairs,q_handler_mutex);
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

/******************** Message queue management ********************/

void create_message_queue(const char *mq_name, mode_t mode, long mq_maxmsg, long mq_msgsize,mqd_t *q_handler) {
  struct mq_attr attr;

  attr.mq_maxmsg = mq_maxmsg;
  attr.mq_msgsize = mq_msgsize;
  *q_handler = mq_open(mq_name, mode, S_IWUSR | S_IRUSR, &attr);
}

void close_message_queues(mqd_t q_handler_new_client, mqd_t q_handler_haircut_done,mqd_t q_handler_barber_chair, mqd_t q_handler_chairs, mqd_t q_handler_mutex) {
  mq_close(q_handler_new_client);
  mq_close(q_handler_haircut_done);
  mq_close(q_handler_barber_chair);
  mq_close(q_handler_chairs);
  mq_close(q_handler_mutex);
}

/******************** Task management ********************/

void send_number_of_chairs(struct MsgBarbershop_t *msg_chairs, mqd_t q_handler_chairs, mqd_t q_handler_barber_chair , mqd_t q_handler_mutex){
  
  struct MsgInteger_t msg_init_value;
  
  mq_send(q_handler_chairs, (const char *)msg_chairs, sizeof(struct MsgBarbershop_t), 0);
  mq_send(q_handler_barber_chair, (const char *)&msg_init_value, sizeof(struct MsgInteger_t), 0);
  mq_send(q_handler_mutex, (const char *)&msg_init_value, sizeof(struct MsgInteger_t), 0);
  
}


/******************** Auxiliar functions ********************/

void free_resources() {
  printf("\n----- [MANAGER] Freeing resources ----- \n");

  /* Free the 'process table' memory */
  free(g_process_table); 

  /* Remove message queues */
  mq_unlink(MQ_NEW_CLIENT);
  mq_unlink(MQ_HAIRCUT_DONE);
  mq_unlink(MQ_BARBER_CHAIR);
  mq_unlink(MQ_CHAIRS);
  mq_unlink(MQ_MUTEX);
}

void initiate_number_of_chairs(struct MsgBarbershop_t **msg_chairs, int *n_chairs) {
  (*msg_chairs)->chairs = *n_chairs;
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
