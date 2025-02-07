#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "common_functions.h"
#include <dlfcn.h>

#define PORT 9000
#define BUFFER_SIZE 1024

void *reply_to_request(void *client_socket);

#define MAX_PATH 256
#define MAX_TYPE 64
#define MAX_BODY 1024

// Structure to hold client socket and route data
typedef struct {
    int client_fd;
    const char **route_data;
} thread_args;


typedef struct {
    char request_type[8];     // GET, POST, etc.
    char path[MAX_PATH];      // Requested path
    char content_type[MAX_TYPE];   // Content-Type header
    int content_length;       // Content-Length header
    char body[MAX_BODY];      // Request body (if any)
} req_info;

void parse_http_request(const char *request, req_info *info) {
    // Initialize the structure
    memset(info, 0, sizeof(req_info));
    info->content_length = 0; // Default to zero

    // Extract request type and path
    sscanf(request, "%s %s", info->request_type, info->path);

    // Find headers and body
    const char *content_type_ptr = strstr(request, "Content-Type: ");
    if (content_type_ptr) {
        sscanf(content_type_ptr, "Content-Type: %s", info->content_type);
    }

    const char *content_length_ptr = strstr(request, "Content-Length: ");
    if (content_length_ptr) {
        sscanf(content_length_ptr, "Content-Length: %d", &info->content_length);
    }

    // Extract body (find \r\n\r\n which separates headers from body)
    const char *body_ptr = strstr(request, "\r\n\r\n");
    if (body_ptr) {
        body_ptr += 4; // Move past \r\n\r\n
        strncpy(info->body, body_ptr, MAX_BODY - 1);
        info->body[MAX_BODY - 1] = '\0'; // Ensure null termination
    }
}


void listen_for_rq(const char **route_data) {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    
    // Create server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket to port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Start listening for connections
    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        // Accept incoming connection
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }

        // Allocate memory for thread arguments
        thread_args *args = malloc(sizeof(thread_args));
        if (!args) {
            perror("Memory allocation failed");
            close(client_fd);
            continue;
        }

        args->client_fd = client_fd;
        args->route_data = route_data;  // Pass route_data

        // Create a new thread to handle the request
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, reply_to_request, args) != 0) {
            perror("Thread creation failed");
            free(args);
            close(client_fd);
        }
        
        // Detach thread to allow automatic cleanup
        pthread_detach(thread_id);
    }

    // Close server socket (not reached in this loop)
    close(server_fd);
}
char * execute_function(const char * path_to_cpp_func);

char* get_substring_after_equal(const char* input) {
    char *equal_sign = strchr(input, '=');
    if (equal_sign != NULL && *(equal_sign + 1) != '\0') {
        return equal_sign + 1; // Return pointer to the substring after '='
    }
    return NULL; // Return NULL if '=' is not found or nothing follows it
}

// Function to handle a client request in a separate thread
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void *reply_to_request(void *args) {
    thread_args *data = (thread_args *)args;
    int client_fd = data->client_fd;
    const char **route_data = data->route_data; // Get route_data
    free(data); // Free allocated memory for thread arguments

    char buffer[BUFFER_SIZE];

    // Read the HTTP request
    ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0'; // Null-terminate the received data

        req_info request_data;
        parse_http_request(buffer, &request_data);
        int is_there_a_match = 0;
        for (int i = 0; route_data[i] != NULL; i++) {
            if (matches_at_equal_signia(route_data[i], request_data.path) == 1) {
                const char *result = get_substring_after_equal(route_data[i]);
                printf("Matched route: %s -> %s\n", request_data.path, route_data[i]);
                is_there_a_match = 1;

                // Get the HTML content from the shared library
                char *file_content = execute_function(result);
                if (file_content) {
                    // Construct the HTTP response
                    char response[BUFFER_SIZE];
                    snprintf(response, sizeof(response),
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html\r\n"
                        "Content-Length: %lu\r\n"
                        "\r\n"
                        "%s",
                        strlen(file_content), file_content);

                    // Send response to the client
                    write(client_fd, response, strlen(response));

                    // Free allocated memory
                    free(file_content);
                } else {
                    // Handle file not found error
                    const char *error_response =
                        "HTTP/1.1 404 Not Found\r\n"
                        "Content-Type: text/plain\r\n"
                        "Content-Length: 13\r\n"
                        "\r\n"
                        "<p>404 Not Found</p>";
                    write(client_fd, error_response, strlen(error_response));
                }
            }
        }
        if (!is_there_a_match){
            // Handle file not found error
            const char *error_response =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 13\r\n"
            "\r\n"
            "404 Not Found";
            write(client_fd, error_response, strlen(error_response));
        }
    }

    // Close the client socket
    close(client_fd);
    return NULL;
}


char * execute_function(const char *path_to_cpp_func) {
    void *handle;
    char *(*func_cpp)();

    // Load the shared library
    handle = dlopen("./libmylib.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Error loading library: %s\n", dlerror());
        return NULL;
    }

    // Get function pointer
    func_cpp = (char *(*)()) dlsym(handle, path_to_cpp_func);
    if (!func_cpp) {
        fprintf(stderr, "Error loading function: %s\n", dlerror());
        dlclose(handle);
        return NULL;
    }

    // Call the function and get HTML content
    char *html_content = func_cpp();

    // Close the shared library
    dlclose(handle);

    return html_content; // Caller must free this
}

