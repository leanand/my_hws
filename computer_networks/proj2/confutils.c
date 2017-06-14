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
#include "confutils.h"
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
	char *server_ip;
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
	// printf("server address: %s\n", inet_ntoa(servaddr.sin_addr));
	if(connect(sd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
		printf("Error: Connection failed : %s", strerror(errno));
		return -1;
	}
	// getsockname(sd, (struct sockaddr *)&clientaddr, &clientaddrsize);
	// clientport = ntohs(clientaddr.sin_port);
	/* succesful. return socket descriptor */
	// printf("admin: connected to server on '%s' at '%hu' thru '%hu'\n", servhost,servport, clientport);
	fflush(stdout);
	return (sd);
}
/*----------------------------------------------------------------*/

/*----------------------------------------------------------------*/
int readn(int sd, unsigned char *buf, size_t n) {
	size_t toberead;
	unsigned char * ptr;

	toberead = n;
	ptr = buf;
	while (toberead > 0) {
		size_t byteread;
		// printf("Byte to be read ==> %d byte read ===>%d\n", toberead, byteread)
		byteread = read(sd, ptr, toberead);
		if (byteread <= 0) {
			if (byteread == -1){
				perror("read");
				return (0);
			}
			return (1);
		}

		toberead -= byteread;
		ptr += byteread;
	}
	return (1);
}

int sendn(int sd, unsigned char *buf, size_t n){
	size_t tobesent;
	unsigned char * ptr;

	tobesent = n;
	ptr = buf;
	while (tobesent > 0) {
		size_t bytesent;
		// printf("Byte to be read ==> %d byte read ===>%d\n", toberead, byteread)
		bytesent = write(sd, ptr, tobesent);
		if (bytesent <= 0) {
			if (bytesent == -1){
				perror("write");
				return (0);
			}
			return (1);
		}
		tobesent -= bytesent;
		ptr += bytesent;
	}
	return (1);
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


int parse_request(char * request, char * host_name, int host_name_size, int * host_port,  char * request_url,int request_url_size){
	char dummy_req[MAX_REQUEST_SIZE];
	char temp[request_url_size];
	char temp1[host_name_size];
	char *token;
	int found = 0;
	strncpy(dummy_req, request, MAX_REQUEST_SIZE);
	token = strtok(dummy_req, "\n");
	while(token != NULL){
		if(sscanf(token,"GET%sHTTP/1.1", temp) != 0){
			strncpy(request_url, temp, request_url_size);
			found ++;
		}
		if(sscanf(token,"Host: %s", temp) != 0){
			strncpy(temp1, temp, host_name_size);	
			found ++;
		}
		token = strtok(NULL, "\n");
	}
	if(found == 2){
		token = strtok(temp1, ":");
		strncpy(host_name, token, host_name_size);

		if((token = strtok(NULL, ":")) != NULL){
			*host_port = atoi(token);
		}else{
			*host_port = 80;
		}
		return 1;
	}else{
		return 0;
	}
}

size_t get_content_size(char * response, size_t response_size, size_t * content_size){
	char dummy_resp[MAX_REQUEST_SIZE];
	char *token;
	char *remaining;
	strncpy(dummy_resp, response, response_size);
	size_t remaining_bytes;
	size_t temp;
	token = strtok(dummy_resp, "\n");
	while(token != NULL){
		if(sscanf(token, "Content-Length: %zd", &temp) != 0){
			*content_size = temp;
		}
		token = strtok(NULL, "\n");
	}
	if((remaining = strstr(response, "\r\n\r\n")) != NULL){
		remaining_bytes = *content_size - (response + response_size - remaining) + 4;
		return remaining_bytes;
	}
	return -1;
}
NODE *get_from_cache(NODE ** cache_list, char * request_url){
	NODE * target_node;
	target_node = *cache_list;
	while(target_node != NULL){
		if(strcmp(target_node->request_url, request_url) == 0){
			target_node->cache_status = 1;
			return target_node;
		}
		target_node = target_node->next;
	}
	return NULL;
}

void add_to_cache(NODE ** cache_list, NODE * target_node){
	NODE * temp_node;
	temp_node = * cache_list;
	if(temp_node == NULL){
		*cache_list = target_node;
		return;
	}
	while(temp_node -> next != NULL){
		temp_node = temp_node->next;
	}
	temp_node->next = target_node;
}

NODE * init_node(){
	NODE * temp;
	temp = malloc(sizeof(NODE));
	temp->response = NULL;
	memset(temp->request_url, '\0', MAX_GET_REQUEST);
	temp->next = NULL;
	temp->cache_status = 0;
	return temp;
}

NODE * make_request(NODE ** cache_list, char * host_name, int host_port, char * request_url, char * request){
	int server_socket;
	server_socket = hooktoserver(host_name, host_port);
	unsigned char * temp_response;
	size_t total_size;
	size_t content_length;
	size_t init_size;
	unsigned char * response;
	size_t size, remaining_bytes;
	NODE *target_node;
	if(!server_socket){
		printf("admin: Error in connecting to host %s\n", host_name);
		return NULL;
	}
	if(!send(server_socket, request, strlen(request), 0)){
		perror("admin: Error in sending request to host %s");
		close(server_socket);
		return NULL;
	}
	temp_response = malloc(MAX_REQUEST_SIZE * sizeof(char));
	if(!(total_size = recv(server_socket, temp_response, MAX_REQUEST_SIZE, 0))){
		perror("admin: Error in sending request to host %s");
		close(server_socket);
		return NULL;
	}
	// total_size = strlen((char*)temp_response);
	response = malloc(total_size * sizeof( unsigned char));
	memcpy(response, temp_response, total_size);
	// strcpy((char *)response, (char *)temp_response);

	if((remaining_bytes = get_content_size((char *)temp_response, total_size, &content_length )) == -1){
		printf("admin: Error in getting content size");
		close(server_socket);
		return NULL;
	}
	free(temp_response);
	if(remaining_bytes > 0){
		init_size = total_size;
		total_size += remaining_bytes;
		temp_response = malloc(remaining_bytes * sizeof(unsigned char));
		if((size = readn(server_socket, temp_response, remaining_bytes)) == 0){
			perror("admin: Error in getting response body from server");
			close(server_socket);
			return NULL;
		}

		response = realloc(response, total_size * sizeof(unsigned char));
		memcpy(response + init_size, temp_response, remaining_bytes);
		free(temp_response);
	}
	close(server_socket);
	target_node = init_node();
	target_node->response = response;
	target_node->total_size = total_size;
	target_node->content_length = content_length;
	strcpy(target_node->request_url, request_url);
	return target_node;
}

NODE * get_request(NODE **cache_list, char*host_name, int  host_port, char * request_url, char * request){
	NODE * target_node;
	target_node = get_from_cache(cache_list, request_url);
	if(target_node != NULL){
		return target_node;
	}
	target_node = make_request(cache_list, host_name, host_port, request_url, request);
	if(target_node != NULL){
		add_to_cache(cache_list, target_node);
	}
	return target_node;
}
