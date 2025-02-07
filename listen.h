#ifndef LISTEN_H
#define LISTEN_H

#define PORT 9000
#define BUFFER_SIZE 1024

#define MAX_PATH 256
#define MAX_TYPE 64
#define MAX_BODY 1024

void *reply_to_request(void *client_socket);

typedef struct {
    char request_type[8];     // GET, POST, etc.
    char path[MAX_PATH];      // Requested path
    char content_type[MAX_TYPE];   // Content-Type header
    int content_length;       // Content-Length header
    char body[MAX_BODY];      // Request body (if any)
} req_info;

void parse_http_request(const char *request, req_info *info);

void listen_for_rq();

void *reply_to_request(void *client_socket);


#endif