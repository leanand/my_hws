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
extern char * recvtext(int sd);
extern int sendtext(int sd, char *msg);

extern int startserver();
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
int fd_isset(int fd, fd_set *fsp) {
	return FD_ISSET(fd, fsp);
}
/* main routine */
int main(int argc, char *argv[]) {
	int servsock; /* server socket descriptor */
	struct sockaddr_in clientaddr;
	fd_set livesdset; /* set of live client sockets */
	fd_set masterset;
	int livesdmax; /* largest live client socket descriptor */
	int new_socket;
	int connfd;
	int selectstatus;
	char sendBuff[1025];
	socklen_t clientaddrlen = sizeof(clientaddr);
	/* check usage */
	if (argc != 1) {
		fprintf(stderr, "usage : %s\n", argv[0]);
		exit(1);
	}

	/* get ready to receive requests */
	servsock = startserver();
	if (servsock == -1) {
		perror("Error on starting server: ");
		exit(1);
	}


	/*while(1){
		connfd = accept(servsock, (struct sockaddr *)NULL, NULL);

		time_t ticks = time(NULL);
		snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
		
		for(int i =0;i < 10 ; i ++){
			write(connfd, sendBuff, strlen(sendBuff));
		}
		close(connfd);
		sleep(1);
	}
*/
	/*
	 FILL HERE:
	 init the set of live clients
	 */

	FD_ZERO(&livesdset);
	// FD_SET(servsock, &livesdset);
	FD_SET(servsock, &masterset);
	livesdmax = servsock;
	/* receive requests and process them */
	while (1) {
		int frsock; /* loop variable */
		livesdset = masterset;
		/*
		 FILL HERE
		 wait using select() for
		 messages from existing clients and
		 connect requests from new clients
		 */
	
		selectstatus = select(livesdmax + 1, &livesdset, NULL, NULL, NULL);
		if(selectstatus < 0){
			fprintf(stderr, "Error in select \n");
		}

		/* look for messages from live clients */
		for (frsock = 3; frsock <= livesdmax; frsock++) {
			/* skip the listen socket */
			/* this case is covered separately */
			if (frsock == servsock){
				continue;
			}

			if (FD_ISSET(frsock, &livesdset)) {
				char * clienthost; /* host name of the client */
				ushort clientport; /* port number of the client */
				struct hostent * cliententry;
				struct in_addr * clientaddress;
				memset(&clientaddr, '0', sizeof(clientaddr));
				char *msg;
				/*
				 FILL HERE:
				 figure out client's host name and port
				 using getpeername() and gethostbyaddr()
				 */
				if(getpeername(frsock,(struct sockaddr *) &clientaddr, &clientaddrlen) == -1){
					perror("Error in get peer name");
				}

				clientaddress = &clientaddr.sin_addr;
				cliententry = gethostbyaddr((char * )clientaddress, sizeof(clientaddress), AF_INET);
				if(cliententry == NULL){
					// herror("Error in gethostbyaddr, receiving data : ");
					clienthost = inet_ntoa(*clientaddress);
				}else{
					clienthost = cliententry->h_name;
				}
				clientport = ntohs(clientaddr.sin_port);

				/* read the message */
				msg = recvtext(frsock);
				if (!msg) {
					/* disconnect from client */
					printf("admin: disconnect from '%s(%hu)'\n", clienthost,
							clientport);

					/*
					 FILL HERE:
					 remove this guy from the set of live clients
					 */
					FD_CLR(frsock, &masterset);

					// close the socket 
					close(frsock);
				} else {
					/*
					 FILL HERE
					 send the message to all live clients
					 except the one that sent the message
					 */
					int temp_fsock;
					for(temp_fsock = 3; temp_fsock<=livesdmax; temp_fsock++){
						if(temp_fsock != frsock && temp_fsock !=servsock && FD_ISSET(temp_fsock, &masterset)){
							sendtext(temp_fsock, msg);
						}
					}

					/* display the message */
					printf("%s(%hu): %s", clienthost, clientport, msg);

					/* free the message */
					free(msg);
				}
			}
		}

		/* look for connect requests */
		if (FD_ISSET(servsock, &livesdset)) {
			/*
			 FILL HERE:
			 accept a new connection request
			 */
			memset(&clientaddr, '0', sizeof(clientaddr));
			if((new_socket = accept(servsock, (struct sockaddr *)&clientaddr, (socklen_t *) &clientaddrlen)) < 0){
				fprintf(stderr, "Error in accepting new socket \n");
				exit(1);
			}

			/* if accept is fine? */
			if (new_socket != -1) {
				char * clienthost; /* host name of the client */
				ushort clientport; /* port number of the client */
				struct in_addr * clientaddress = &clientaddr.sin_addr;
				struct hostent * cliententry;
				/*
				 FILL HERE:
				 figure out client's host name and port
				 using gethostbyaddr() and without using getpeername().
				 */
				cliententry = gethostbyaddr((char * )clientaddress, sizeof(clientaddress), AF_INET);
				
				if(cliententry == NULL){
					clienthost = inet_ntoa(*clientaddress);
				}else{
					clienthost = cliententry->h_name;
				}
				clientport = ntohs(clientaddr.sin_port);
				printf("admin: connect from '%s' at '%hu'\n", clienthost,
						clientport);

				/*
				 FILL HERE:
				 add this guy to set of live clients
				 */

				FD_SET(new_socket, &masterset);
				if(livesdmax < new_socket){
					livesdmax = new_socket;
				}
			} else {
				perror("accept");
				exit(0);
			}
		}
	}
	return 0;
}
/*--------------------------------------------------------------------*/

