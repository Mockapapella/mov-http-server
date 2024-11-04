// Original server implementation from https://bruinsslot.jp/post/simple-http-webserver-in-c/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>         // For close()
#include <arpa/inet.h>      // For inet_ntoa()
#include <netinet/in.h>     // For sockaddr_in
#include <sys/socket.h>     // For socket functions

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket
    memset(&address, 0, sizeof(address)); // Zero out structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Any IP address
    address.sin_port = htons(PORT);       // Port number

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_fd, SOMAXCONN) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d\n", PORT);

    // Accept and handle connections
    while (1) {
        int new_socket;
        struct sockaddr_in client_address;
        int client_addrlen = sizeof(client_address);

        // Accept connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&client_address, (socklen_t *)&client_addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }

        printf("Connection accepted from %s:%d\n",
               inet_ntoa(client_address.sin_addr),
               ntohs(client_address.sin_port));

        // Read request
        char buffer[BUFFER_SIZE] = {0};
        int bytes_read = read(new_socket, buffer, BUFFER_SIZE - 1);
        if (bytes_read < 0) {
            perror("Read failed");
            close(new_socket);
            continue;
        }

        printf("Received request:\n%s\n", buffer);

        // Send response
        const char *response =
            "HTTP/1.1 200 OK\r\n"
            "Server: SimpleCServer\r\n"
            "Content-Type: text/html; charset=UTF-8\r\n\r\n"
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head><title>Hello</title></head>\n"
            "<body><h1>Hello, World!</h1></body>\n"
            "</html>\n";

        if (write(new_socket, response, strlen(response)) < 0) {
            perror("Write failed");
        }

        // Close connection
        close(new_socket);
    }

    // Close server socket (unreachable code in this example)
    close(server_fd);

    return 0;
}
