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
    (void ) data;
    while (1) {
        sleep(10);
        printf("listener running\n");
    }
    
    return NULL;
}