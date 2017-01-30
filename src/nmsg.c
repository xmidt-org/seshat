#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <nanomsg/nn.h>
#include <nanomsg/pipeline.h>

#define SOCK_SEND_TIMEOUT_MS 2000

const char *send_id = NULL;
const char *msg = NULL;
unsigned long send_count = 0;
bool set_timeout = true;
char errbuf[100];

int connect_receiver (char *url)
{
	int sock = -1;
        
	if (NULL == url) {
		return sock;
	}

        sock = nn_socket (AF_SP, NN_PULL);
	if (sock < 0) {
		printf ("Unable to create rcv socket: %s\n",
			strerror_r (errno, errbuf, 100));
 		return sock;
	}
        if (nn_bind (sock, url) < 0) {
		printf ("Unable to bind to receive socket %s: %s\n",
			url, strerror_r (errno, errbuf, 100));
                nn_close(sock);
                sock = -1;
	}
        
	return sock;
}

void shutdown_receiver (int sock)
{
	if (sock != -1) {
		nn_shutdown (sock, 0);
		nn_close (sock);
	}
}

int connect_sender (char *url)
{
	int sock = -1;
	int send_timeout = SOCK_SEND_TIMEOUT_MS;

	if (NULL == url) {
		return sock;
	}

        sock = nn_socket (AF_SP, NN_PUSH);
	if (sock < 0) {
		printf ("Unable to create send socket: %s\n",
			strerror_r (errno, errbuf, 100));
 		return sock;
	}
	if (set_timeout) {
		if (nn_setsockopt (sock, NN_SOL_SOCKET, NN_SNDTIMEO, 
					&send_timeout, sizeof (send_timeout)) < 0) {
			printf ("Unable to set socket timeout: %s: %s\n",
				url, strerror_r (errno, errbuf, 100));
                        nn_close(sock);
	 		return -1;
		}
        }
        
        if (nn_connect (sock, url) < 0) {
		printf ("Unable to connect to send socket %s: %s\n",
			url, strerror_r (errno, errbuf, 100));
		nn_close(sock);
                sock = -1;
	}

	return sock;
}

void shutdown_sender (int sock)
{
	nn_shutdown (sock, 0);
	nn_close (sock);
}


// returned msg must be freed with nn_freemsg
char *receive_msg (int sock)
{
	char *buf = NULL;
	int bytes;
  bytes = nn_recv (sock, &buf, NN_MSG, 0);

  if (bytes < 0) { 
		printf ("Error receiving msg: %s\n", strerror_r (errno, errbuf, 100));
		return NULL;
	}
	printf ("RECEIVED \"%s\"\n", buf);
	return buf;
}

int send_msg (const char *msg, int sock)
{
  int sz_msg = strlen (msg) + 1; // '\0' too
  int bytes;
	bytes = nn_send (sock, msg, sz_msg, 0);
  if (bytes < 0) { 
		printf ("Error sending msg: %s\n", strerror_r (errno, errbuf, 100));
		return -1;
	}
  if (bytes != sz_msg) {
		printf ("Not all bytes sent, just %d\n", bytes);
		return -1;
	}
	return 0;
}

