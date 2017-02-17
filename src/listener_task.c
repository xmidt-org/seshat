#include "listener_task.h"
#include <pthread.h>



static pthread_t thread_id;

static void *listener(void *data);
static listener_data_t the_data;
int listener_thread_start(listener_data_t *data)
{
    the_data = *data;
    return pthread_create(&thread_id, NULL, listener, (void *) &the_data);
}


void *listener(void *data)
{
    listener_data_t in_data = * (listener_data_t *) data;
    char buf[NN_MSG+1];
    
    while (1) {
        int bytes = nn_recv (in_data.socket, &buf, NN_MSG, 0);
        if (bytes > 0) {
             printf("listener got %d bytes\n", bytes);
           //Parse message and send response
           // nn_send(socket?, response_code, size);
        } else {
            printf("listener timed out\n");
            
        }
        sleep(5);
        printf("listener running\n");
    }
    
    return NULL;
}