/*
====================================================================
Josue Carlos Zenteno and Sergio Silvestre Pavon
Concurrent and Real-Time Programming
Faculty of Computer Science
University of Castilla-La Mancha (Spain)
====================================================================
*/

#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <definitions.h>

/* Message queue management */
void open_message_queue(const char *mq_name, mode_t mode, mqd_t *q_handler);

/* Task management */

void work(mqd_t q_handler_new_client, mqd_t q_handler_haircut_done);

/******************** Main function ********************/

int main(int argc, char *argv[]) {
  mqd_t q_handler_new_client, q_handler_haircut_done;
  mode_t mode_read_write = O_RDWR;
  
  /* Open message queues */
  open_message_queue(MQ_NEW_CLIENT, mode_read_write, &q_handler_new_client);
  open_message_queue(MQ_HAIRCUT_DONE, mode_read_write, &q_handler_haircut_done);

  /* Task management */
   while(TRUE){
        work(q_handler_new_client,q_handler_haircut_done);
   } 
    

  return EXIT_SUCCESS;
}

/******************** Message queue management ********************/

void open_message_queue(const char *mq_name, mode_t mode, mqd_t *q_handler) {
  *q_handler = mq_open(mq_name, mode);
}

/******************** Task management ********************/

void work(mqd_t q_handler_new_client, mqd_t q_handler_haircut_done) {
  struct MsgInteger_t msg_value;
  
  printf("[Barber]          Barber sleeping zZzZzZz\n");
  mq_receive(q_handler_new_client, (char *)&msg_value, sizeof(struct MsgInteger_t), NULL);
    printf("[Barber]          Barber cutting hair\n");
    sleep(3);   //Simulating complexity
  mq_send(q_handler_haircut_done, (const char *)&msg_value, sizeof(struct MsgInteger_t), 0);
  printf("[Barber]          Barber has finished the haircut\n");

}
