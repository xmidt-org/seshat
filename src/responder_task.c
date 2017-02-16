#include "responder_task.h"
#include <pthread.h>



static pthread_t thread_id;

static void responder(void *data);
static int the_socket;

int responder_thread_start(int socket)
{
    the_socket = socket;
    return pthread_create(&thread_id, NULL, responder, (void *) &the_socket);
}


void responder(void *data)
{
    (void ) data;
    while (1) {
        sleep(10);
        printf("responder running\n");
    }
    
}