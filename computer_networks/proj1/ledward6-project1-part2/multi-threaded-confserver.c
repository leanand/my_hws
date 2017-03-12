/*--------------------------------------------------------------------*/
/* conference server */

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <netdb.h>
#include <time.h> 
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <sys/select.h>
#include <pthread.h>
#include <signal.h>
extern char * recvtext(int sd);
extern int sendtext(int sd, char *msg);

extern int startserver();
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/* main routine */

int open_sockets[50];
int open_sockets_count = 0;
pthread_mutex_t lock;

void add_socket(int socket){
	pthread_mutex_lock(&lock);
	open_sockets[open_sockets_count] = socket;
	open_sockets_count ++;
	pthread_mutex_unlock(&lock);
}

void remove_socket(int socket){
	int i;
	pthread_mutex_lock(&lock);
	for(i =0; i < open_sockets_count; i ++){
		if(socket == open_sockets[i]){
			open_sockets[i] = open_sockets[open_sockets_count - 1];
			open_sockets_count --;
			break;
		}
	}
	pthread_mutex_unlock(&lock);
}

void * process_single_client(void * client_sock){
	int client_socket, i;
	pthread_detach(pthread_self());
	client_socket = * (int *)client_sock;
	char * clienthost; /* host name of the client */
	ushort clientport; /* port number of the client */
	struct hostent * cliententry;
	struct in_addr * clientaddress;
	struct sockaddr_in clientaddr;
	socklen_t clientaddrlen = sizeof(clientaddr);
	memset(&clientaddr, '0', sizeof(clientaddr));
	char *msg;
	
	if(getpeername(client_socket,(struct sockaddr *) &clientaddr, &clientaddrlen) == -1){
		perror("Error in get peer name");
	}

	clientaddress = &clientaddr.sin_addr;
	cliententry = gethostbyaddr((char * )clientaddress, sizeof(clientaddress), AF_INET);
	if(cliententry == NULL){
		clienthost = inet_ntoa(*clientaddress);
	}else{
		clienthost = cliententry->h_name;
	}
	clientport = ntohs(clientaddr.sin_port);
	while((msg = recvtext(client_socket))){
		pthread_mutex_lock(&lock);
		for(i = 0; i < open_sockets_count; i ++){
			if(open_sockets[i] == client_socket){
				continue;
			}
			sendtext(open_sockets[i], msg);
		}
		pthread_mutex_unlock(&lock);
		printf("%s(%hu): %s", clienthost, clientport, msg);
		free(msg);
	}

	printf("admin: disconnect from '%s(%hu)'\n", clienthost,clientport);
	remove_socket(client_socket);
	close(client_socket);
	return NULL;
}


void * accept_new_client(void * anc_args){
	int servsock, clientsock;
	struct sockaddr_in clientaddr;
	socklen_t clientaddrlen = sizeof(clientaddr);
	servsock = startserver();
	pthread_t client_thread;
	if (servsock == -1) {
		perror("Error on starting server: ");
		exit(1);
	}
	while(1){
		memset(&clientaddr, '0', sizeof(clientaddr));
		char * clienthost; /* host name of the client */
		ushort clientport; /* port number of the client */
		struct hostent * cliententry;
		struct in_addr * clientaddress;
		if((clientsock = accept(servsock, (struct sockaddr *)&clientaddr, (socklen_t *) &clientaddrlen)) < 0){
			fprintf(stderr, "Error in accepting new socket \n");
			continue;
		}
		add_socket(clientsock);

		clientaddress = &clientaddr.sin_addr;
		cliententry = gethostbyaddr((char * )clientaddress, sizeof(clientaddress), AF_INET);
		if(cliententry == NULL){
			clienthost = inet_ntoa(*clientaddress);
		}else{
			clienthost = cliententry->h_name;
		}
		clientport = ntohs(clientaddr.sin_port);
		printf("admin: connect from '%s' at '%hu'\n", clienthost, clientport);

		if(pthread_create(&client_thread, NULL, process_single_client, &clientsock) != 0){
			perror("Cannot create worker thread");
			remove_socket(clientsock);
			close(clientsock);
			close(servsock);
			pthread_exit(NULL);
		}
	}
	pthread_exit(NULL);
}


int main(int argc, char *argv[]) {
	int servsock; /* server socket descriptor */
	/* check usage */
	if (argc != 1) {
		fprintf(stderr, "usage : %s\n", argv[0]);
		exit(1);
	}
	pthread_t server_thread;
	sigset_t sigmask;
	sigemptyset (&sigmask);
	sigaddset(&sigmask, SIGPIPE);

	if(pthread_sigmask(SIG_BLOCK, &sigmask, NULL) != 0){
		perror("Error in Masking Signal");
		exit(-1);
	}

	if (pthread_mutex_init(&lock, NULL) != 0){
		perror("Error in creating lock");
    exit(-1);
	}

	/* get ready to receive requests */
	if(pthread_create(&server_thread, NULL, accept_new_client, NULL) < 0){
		perror("Error in creating server thread");
		exit(-1);
	}
	pthread_join(server_thread, NULL);
	pthread_mutex_destroy(&lock);
	return 1;
}
/*--------------------------------------------------------------------*/


