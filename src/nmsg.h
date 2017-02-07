#ifndef __NMSG_H__
#define __NMSG_H__

#ifdef __cplusplus
extern "C" {
#endif
    
int connect_receiver (char *url);
int connect_sender (char *url);
void shutdown_receiver (int sock);
void shutdown_sender (int sock);
    
char *receive_msg (int sock, int *number_of_bytes);
int send_msg (const char *msg, int sock);
   
    
#ifdef __cplusplus
}
#endif


#endif