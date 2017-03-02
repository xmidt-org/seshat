#include "listener_task.h"
#include "wrp_interface.h"
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
    
    sleep(10);
    
    while (1) {
        int bytes = nn_recv (in_data.socket, &buf, NN_MSG, 0);
        if (bytes > 0) {
            wrp_msg_t response;
#ifndef FAKE_SERVER_CODE
            response.msg_type = WRP_MSG_TYPE__AUTH;
#else
            response.msg_type = 911;            
#endif
             printf("listener got %d bytes\n", bytes);
             response.u.auth.status = create_response_to_message(buf, bytes);
             nn_send(in_data.socket, &response, sizeof(wrp_msg_t), 0);
             nn_freemsg(buf);
        } else {
            // do we need to check for socket error other than timed out ?
            printf("listener timed out or socket error?\n");
            continue;
        }
        sleep(1);
        printf("listener running\n");
    }
    
    return NULL;
}