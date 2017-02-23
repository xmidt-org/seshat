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
    char *buf = NULL;
    
    while (1) {
        int bytes = nn_recv (in_data.socket, &buf, NN_MSG, 0);
        if (bytes > 0) {
            wrp_msg_t response.msg_type = WRP_MSG_TYPE__AUTH;
            size_t resp_size = 0;
             printf("listener got %d bytes\n", bytes);
             response.u.auth.status = create_response_to_message(buf, bytes);
             nn_send(in_data.socket, &response, sizeof(wrp_msg_t));
             free(buf);
        } else {
            // do we need to check for socket error other than timed out ?
            printf("listener timed out or socket error?\n");
            continue;
        }
        sleep(5);
        printf("listener running\n");
    }
    
    return NULL;
}