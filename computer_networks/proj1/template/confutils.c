/*--------------------------------------------------------------------*/
/* functions to connect clients and server */

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <netdb.h>
#include <time.h> 
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#define MAXNAMELEN 256
/*--------------------------------------------------------------------*/

/*----------------------------------------------------------------*/
/* prepare server to accept requests
 returns file descriptor of socket
 returns -1 on error
 */
int startserver() {
	int sd; /* socket descriptor */

	char * servhost; /* full name of this host */
	ushort servport; /* port assigned to this server */
	char hostname[MAXNAMELEN];
	struct sockaddr_in servaddr;
	struct hostent * hostentry;
	socklen_t size;
	/*
	 FILL HERE
	 create a TCP socket using socket()
	 */

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket < 0){
		fprintf(stderr, "Error in creating socket %s\n",strerror(errno) );
		return sd;
	}
	
	memset(&servaddr, '0', sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = 0;

	/*
	 FILL HERE
	 bind the socket to some port using bind()
	 let the system choose a port
	 */

	if(bind(sd,(struct sockaddr *)&servaddr, sizeof(servaddr)) != 0){
		fprintf(stderr, "Error in binding socket => %s\n",strerror(errno) );
		return -1;
	}

	/* we are ready to receive connections */
	listen(sd, 5);

	/*
	 FILL HERE
	 figure out the full host name (servhost)
	 use gethostname() and gethostbyname()
	 full host name is remote**.cs.binghamton.edu
	 */

	if(gethostname(hostname, MAXNAMELEN) == -1){
		fprintf(stderr, "Error in getting host name => %s\n",strerror(errno) );
		return -1;
	}
	hostentry = gethostbyname(hostname);
	servhost = hostentry->h_name;
	/*
	 FILL HERE
	 figure out the port assigned to this server (servport)
	 use getsockname()
	 */
	size = sizeof(servaddr);
	if(getsockname(sd, (struct sockaddr *)&servaddr, &size) == -1){
		fprintf(stderr, "Error in getting port number => %s\n",strerror(errno) );
		return -1;
	}

	servport = ntohs(servaddr.sin_port);
	/* ready to accept requests */
	printf("admin: started server on '%s' at '%hu'\n", servhost, servport);
	// free(hostentry);
	return (sd);
}
/*----------------------------------------------------------------*/

/*----------------------------------------------------------------*/
/*
 establishes connection with the server
 returns file descriptor of socket
 returns -1 on error
 */
int hooktoserver(char *servhost, ushort servport) {
	int sd; /* socket descriptor */

	ushort clientport; /* port assigned to this client */

	struct sockaddr_in servaddr, clientaddr;
	struct hostent *hostentry;
	struct in_addr **addr_list;
	int i;
	socklen_t clientaddrsize = sizeof(clientaddr);
	memset(&servaddr, '0', sizeof(servaddr));

	/*
	 FILL HERE
	 create a TCP socket using socket()
	 */

	if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		fprintf(stderr, "Error in creating socket %s\n", strerror(errno));
		return sd;
	}

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(servport);
	
	/*
	 FILL HERE
	 connect to the server on 'servhost' at 'servport'
	 use gethostbyname() and connect()
	 */
	
	if((hostentry = gethostbyname(servhost)) == NULL){
		fprintf(stderr, "Error in creating socket %s\n", strerror(errno));
		return -1;
	}

	addr_list = (struct in_addr **) hostentry->h_addr_list;

	for(i = 0; addr_list[i] != NULL; i++){
		servaddr.sin_addr = *addr_list[i];
		break;
	}

	if(connect(sd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
		printf("Error: Connection failed : %s", strerror(errno));
		return -1;
	}

	/*
	 FILL HERE
	 figure out the port assigned to this client
	 use getsockname()
	 */
	getsockname(sd, (struct sockaddr *)&clientaddr, &clientaddrsize);
	clientport = ntohs(clientaddr.sin_port);
	/* succesful. return socket descriptor */
	printf("admin: connected to server on '%s' at '%hu' thru '%hu'\n", servhost,
			servport, clientport);
	printf(">");
	fflush(stdout);
	return (sd);
}
/*----------------------------------------------------------------*/

/*----------------------------------------------------------------*/
int readn(int sd, char *buf, int n) {
	int toberead;
	char * ptr;

	toberead = n;
	ptr = buf;
	while (toberead > 0) {
		int byteread;

		byteread = read(sd, ptr, toberead);
		if (byteread <= 0) {
			if (byteread == -1)
				perror("read");
			return (0);
		}

		toberead -= byteread;
		ptr += byteread;
	}
	return (1);
}

char *recvtext(int sd) {
	char *msg;
	long len;
	/* read the message length */
	if (!readn(sd, (char *) &len, sizeof(len))) {
		return (NULL);
	}
	len = ntohl(len);

	/* allocate space for message text */
	if (len > 0) {
		msg = (char *) malloc(len);
		if (!msg) {
			fprintf(stderr, "error : unable to malloc\n");
			return (NULL);
		}
		/* read the message text */
		if (!readn(sd, msg, len))	 {
			free(msg);
			return (NULL);
		}
	}

	/* done reading */
	return (msg);
}

int sendtext(int sd, char *msg) {
	long len;

	/* write lent */
	len = (msg ? strlen(msg) + 1 : 0);
	len = htonl(len);
	write(sd, (char *) &len, sizeof(len));

	/* write message text */
	len = ntohl(len);
	if (len > 0)
		write(sd, msg, len);
	return (1);
}
/*----------------------------------------------------------------*/

