/*
 * For some reason '///' is required otherwise I get a bind error ;-(/!
 ./fske_server -f foo.txt -u ipc:///tmp/foo.ipc
 */
#include "fake_server.h"
#include <pthread.h>



static pthread_t fake_thread_id;

static void *fake_server(void *data);
static listener_data_t fake_data;

int server_listener_thread_start(listener_data_t *data);
static void *server_listener(void *data);
static listener_data_t server_listener_data;
static pthread_t server_listener_thread_id;
static void __sig_handler(int sig);
static void init_signal_handler(void);
static void _exit_process_(int signum);

int fake_server_socket;
listener_data_t fake_server_data;
int fake_server_thread_start(listener_data_t *data);    
static char *url = NULL;

int main(int arc, char **argv);


#define OPTIONS_STRING_FORMAT "u:"

/* Notes: getopts is case-sensitive */
static struct option command_line_options[] = {
    {"url", required_argument, 0, 'u'},
    {0,0,0,0}
};


int main(int argc, char **argv)
{
    (void) argc; (void) argv; // suppress compiler warning/error
 
    int item;
    int options_index = 0;
    int str_len = 0;
    //int timeout_val;
    
    init_signal_handler();
    
    while (-1 != (item = getopt_long(argc, argv, OPTIONS_STRING_FORMAT,
                         command_line_options, &options_index)))
    {
        switch (item) {
            case 'u':
                str_len = strlen(optarg);
                url = (char *) malloc(str_len + 1);
                memset(url, 0, str_len);
                strncpy(url, optarg, str_len);
                break;
        }
        
    }
    
    if (NULL == url) {
     printf("%s:url is a required parameter!\n", argv[0]);
     exit(-1);
    } 
   
    
    sleep(1);
    printf("About to start fake server\n");
    fake_server_socket = nn_socket(AF_SP, NN_REQ);
    if (fake_server_socket < 0) {
        printf("Fake Server Socket Failed!\n");
        exit(-11);
    } 

#if 0   
    timeout_val = 5000; // ms    
    if (0 != nn_setsockopt (fake_server_socket, NN_SOL_SOCKET, NN_RCVTIMEO,
            &timeout_val, sizeof(timeout_val))) {
        printf("fake_server_socket: Failed to set wait time out!\n");
    }    
#endif
    
    if (nn_connect(fake_server_socket, url) < 0) {
        printf("Fake Server connect failed!\n");
        nn_shutdown(fake_server_socket, 0);
        free(url);
        exit(-12);
    }
    
    fake_server_data.socket = fake_server_socket;
    fake_server_data.url = url;
    
    fake_server_thread_start(&fake_server_data);
    
    while(1) {
        printf("Fake server thread started...waiting for kill command\n");
        sleep(7000);
    }
    
    free(url);
    
    return 0;
}

int fake_server_thread_start(listener_data_t *data)
{
    fake_data = *data;
    return pthread_create(&fake_thread_id, NULL, fake_server, (void *) &fake_data);
}


void *fake_server(void *data)
{
    listener_data_t in_data = * (listener_data_t *) data;
    
    server_listener_thread_start((listener_data_t *) data);   
    
    printf("Fake Server starting\n");

    while (1) {
        wrp_msg_t *response = (wrp_msg_t *) malloc(sizeof(wrp_msg_t));
        int bytes_sent;
        response->msg_type = WRP_MSG_TYPE__AUTH;
        response->u.auth.status = 200;
        printf("fake_server: About to send message\n");
        if ((bytes_sent =  nn_send(in_data.socket, response, sizeof(wrp_msg_t), 0)) > 0) {
            printf("Sent %d bytes (size of struct %d)\n", bytes_sent, (int ) sizeof(wrp_msg_t));
        }
        else {
            printf("Sender failed to send message\n");
        }

        sleep(5);
        printf("Fake Server running...\n");
    }
    
    return NULL;
}


int server_listener_thread_start(listener_data_t *data)
{
    server_listener_data = *data;
    return pthread_create(&server_listener_thread_id, NULL, server_listener, (void *) &server_listener_data);
}

void *server_listener(void *data)
{
    listener_data_t in_data = * (listener_data_t *) data;
    char *buf = NULL;
    
    sleep(10);
    
    while (1) {
        int bytes = nn_recv (in_data.socket, &buf, NN_MSG, 0);
        if (bytes > 0) {
            wrp_msg_t response = *(wrp_msg_t *) buf;
            printf("Server Listener Got reply: %d bytes, response %d\n",
                    bytes, response.msg_type);
            nn_freemsg(buf);
        } else {
            // do we need to check for socket error other than timed out ?
            printf("server_listener timed out or socket error?\n");
        }
        sleep(3);
        printf("server_listener running\n");
    }
    
    return NULL;
}

void init_signal_handler(void)
{
      signal(SIGTERM, __sig_handler);
      signal(SIGINT, __sig_handler);
      signal(SIGUSR1, __sig_handler);
      signal(SIGUSR2, __sig_handler);
      signal(SIGSEGV, __sig_handler);
      signal(SIGBUS, __sig_handler);
      signal(SIGKILL, __sig_handler);
      signal(SIGFPE, __sig_handler);
      signal(SIGILL, __sig_handler);
      signal(SIGQUIT, __sig_handler);
      signal(SIGHUP, __sig_handler);
      signal(SIGALRM, __sig_handler);
}


void __sig_handler(int sig)
{

    if ( sig == SIGINT ) {
        signal(SIGINT, __sig_handler); /* reset it to this function */
        printf("fake_server SIGINT received!\n");
        _exit_process_(sig);
    }
    else if ( sig == SIGUSR1 ) {
        signal(SIGUSR1, __sig_handler); /* reset it to this function */
        printf("fake_server SIGUSR1 received!\n");
    }
    else if ( sig == SIGUSR2 ) {
        printf("fake_server SIGUSR2 received!\n");
    }
    else if ( sig == SIGCHLD ) {
        signal(SIGCHLD, __sig_handler); /* reset it to this function */
        printf("fake_server SIGHLD received!\n");
    }
    else if ( sig == SIGPIPE ) {
        signal(SIGPIPE, __sig_handler); /* reset it to this function */
        printf("fake_server SIGPIPE received!\n");
    }
    else if ( sig == SIGALRM ) {
        signal(SIGALRM, __sig_handler); /* reset it to this function */
        printf("fake_server SIGALRM received!\n");
    }
    else if( sig == SIGTERM ) {
      //  signal(SIGTERM, __terminate_listener);
        printf("fake_server SIGTERM received!\n");
        _exit_process_(sig);
    }
    else {
        printf("seshat Signal %d received!\n", sig);
        _exit_process_(sig);
    }
}

void _exit_process_(int signum)
{
  printf("fake_server ready to exit!\n");
  signal(signum, SIG_DFL);
  kill(getpid(), signum);
}
