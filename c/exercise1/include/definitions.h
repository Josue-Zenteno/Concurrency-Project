/*
====================================================================
Josue Carlos Zenteno and Sergio Silvestre Pavon
Concurrent and Real-Time Programming
Faculty of Computer Science
University of Castilla-La Mancha (Spain)
====================================================================
*/

#define SEM_BARBER_CHAIR   "sem_barber_chair"
#define SEM_NEW_CLIENT     "sem_new_client"
#define SEM_HAIRCUT_DONE   "sem_haircut_done"
#define SEM_MUTEX          "sem_mutex"

#define SHM_CHAIRS         "shm_chairs"

#define BARBER_CLASS       "BARBER"
#define BARBER_PATH        "./exec/barber"

#define CLIENT_CLASS       "CLIENT"
#define CLIENT_PATH        "./exec/client"

#define NUM_BARBERS 1

#define TRUE 1
#define FALSE 0

struct TData_t {
  int chairs;
};

enum ProcessClass_t {BARBER,CLIENT}; 

struct TProcess_t {          
  enum ProcessClass_t class; /* BARBER or CLIENT */
  pid_t pid;                 /* Process ID */
  char *str_process_class;   /* String representation of the process class */
};
