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
void clienting(mqd_t q_handler_chairs,mqd_t q_handler_mutex, mqd_t q_handler_barber_chair, mqd_t q_handler_new_client, mqd_t q_handler_haircut_done);

/******************** Main function ********************/

int main(int argc, char *argv[]) {
  mqd_t q_handler_mutex, q_handler_barber_chair;
  mqd_t q_handler_new_client, q_handler_haircut_done;
  mqd_t q_handler_chairs;
  mode_t mode_read_write = O_RDWR;
  
  /* Open message queues */

  open_message_queue(MQ_NEW_CLIENT, mode_read_write, &q_handler_new_client);
  open_message_queue(MQ_HAIRCUT_DONE, mode_read_write, &q_handler_haircut_done);
  open_message_queue(MQ_BARBER_CHAIR, mode_read_write, &q_handler_barber_chair);
  open_message_queue(MQ_CHAIRS, mode_read_write, &q_handler_chairs);
  open_message_queue(MQ_MUTEX, mode_read_write, &q_handler_mutex);

  /* Task management */
  
  clienting(q_handler_chairs,q_handler_mutex,q_handler_barber_chair,q_handler_new_client,q_handler_haircut_done);

  return EXIT_SUCCESS;
}

/******************** Message queue management ********************/

void open_message_queue(const char *mq_name, mode_t mode, mqd_t *q_handler) {
  *q_handler = mq_open(mq_name, mode);
}

/******************** Task management ********************/

void clienting(mqd_t q_handler_chairs,mqd_t q_handler_mutex, mqd_t q_handler_barber_chair, mqd_t q_handler_new_client, mqd_t q_handler_haircut_done) {
   
   struct MsgInteger_t msg_value;
   struct MsgBarbershop_t msg_chairs;
   int aux;
   
   srand((int)getpid());  //Generating random spawn time
   sleep(1 + (rand()%10));
  
   printf("[Client %d]    Gets in the barbershop.\n",getpid());

   mq_receive(q_handler_mutex, (char *)&msg_value, sizeof(struct MsgInteger_t), NULL);
   mq_receive(q_handler_chairs, (char *)&msg_chairs, sizeof(struct MsgBarbershop_t), NULL); 
        if (msg_chairs.chairs == 0){
            printf("[Client %d]    There is not any free chair. Leaves the barbershop\n",getpid());
            mq_send(q_handler_mutex, (const char *)&msg_value, sizeof(struct MsgInteger_t), 0);
            mq_send(q_handler_chairs, (const char *)&msg_chairs, sizeof(struct MsgBarbershop_t), 0);
        }else{
                printf("[Client %d]    Takes a seat in the barbershop (%d chairs left).\n",getpid(),msg_chairs.chairs);
                sleep(1);

                msg_chairs.chairs = msg_chairs.chairs - 1;

                printf("[Client %d]    Seats on an empty chair.\n",getpid());
                sleep(1);

                mq_send(q_handler_chairs, (const char *)&msg_chairs, sizeof(struct MsgBarbershop_t), 0);
            mq_send(q_handler_mutex, (const char *)&msg_value, sizeof(struct MsgInteger_t), 0);

            mq_receive(q_handler_barber_chair, (char *)&msg_value, sizeof(struct MsgInteger_t), NULL);

                mq_receive(q_handler_mutex, (char *)&msg_value, sizeof(struct MsgInteger_t), NULL);

                    mq_receive(q_handler_chairs, (char *)&msg_chairs, sizeof(struct MsgBarbershop_t), NULL);

                    aux = msg_chairs.chairs + 1;
                    msg_chairs.chairs = aux;

                    printf("[Client %d]    Frees the chair.\n",getpid());
                    sleep(1);

                    mq_send(q_handler_chairs, (const char *)&msg_chairs, sizeof(struct MsgBarbershop_t), 0);

                mq_send(q_handler_mutex, (const char *)&msg_value, sizeof(struct MsgInteger_t), 0);
                
                printf("[Client %d]    Wakes up the barber.\n",getpid());
                sleep(1);
                
                mq_send(q_handler_new_client, (const char *)&msg_value, sizeof(struct MsgInteger_t), 0); //Rendezvous pattern
                mq_receive(q_handler_haircut_done, (char *)&msg_chairs, sizeof(struct MsgBarbershop_t), NULL); //Rendezvous pattern
                
                printf("[Client %d]    Leaves the barbershop (%d chairs left).\n",getpid(),aux);
                sleep(1);

            mq_send(q_handler_barber_chair, (const char *)&msg_value, sizeof(struct MsgInteger_t), 0);
        }
}