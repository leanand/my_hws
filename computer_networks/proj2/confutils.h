
#define MAX_REQUEST_SIZE 1000
#define MAX_HOST_NAME 64
#define MAX_GET_REQUEST 200
#define MAXNAMELEN 256


typedef struct _node {
  unsigned char * response;
  char request_url[MAX_GET_REQUEST];
  size_t content_length;
  size_t total_size;
  int cache_status;
  struct  _node *  next;
} NODE;

NODE * cache_list;

char * recvtext(int sd);
int sendn(int sd, unsigned char *buf, size_t n);
int parse_request(char * msg, char *host_name, int host_name_size, int *host_port, char * request_url,  int request_url_size);
int startserver();
NODE * get_request(NODE **cache_list, char*host_name, int host_port, char * request_url, char * request);