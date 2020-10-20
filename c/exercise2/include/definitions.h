/*
====================================================================
Josue Carlos Zenteno and Sergio Silvestre Pavon
Concurrent and Real-Time Programming
Faculty of Computer Science
University of Castilla-La Mancha (Spain)
====================================================================
*/

#define MQ_NEW_CLIENT         "/mq_new_client"
#define MQ_HAIRCUT_DONE       "/mq_haircut_done"
#define MQ_BARBER_CHAIR       "/mq_barber_chair"
#define MQ_CHAIRS             "/mq_chairs"
#define MQ_MUTEX              "/mq_mutex"

#define BARBER_CLASS      "BARBER"
#define BARBER_PATH       "./exec/barber"
#define CLIENT_CLASS      "CLIENT"
#define CLIENT_PATH       "./exec/client"


#define NUM_BARBERS 1

#define TRUE 1
#define FALSE 0

/* Used in MQ_RAW_DATA */
struct MsgBarbershop_t {
  int chairs;
};
struct MsgInteger_t{
  int value;
};

enum ProcessClass_t {BARBER,CLIENT}; 

struct TProcess_t {          
  enum ProcessClass_t class; /* BARBER or CLIENT */
  pid_t pid;                 /* Process ID */
  char *str_process_class;   /* String representation of the process class */
};
