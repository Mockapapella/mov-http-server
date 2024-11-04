// A movcc compatible implementation of a basic http server

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Define necessary structures and constants
struct in_addr {
    unsigned long s_addr;
};

struct sockaddr_in {
    short sin_family;
    unsigned short sin_port;
    struct in_addr sin_addr;
    unsigned char sin_zero[8];
};

#define AF_INET 2
#define SOCK_STREAM 1
#define INADDR_ANY 0

// Function to convert host to network byte order
unsigned short my_htons(unsigned short hostshort) {
    return ((hostshort >> 8) & 0xFF) | ((hostshort & 0xFF) << 8);
}

// Replacements for inet_ntoa and ntohs
char *my_inet_ntoa(struct in_addr in) {
    static char buf[16];
    unsigned char *bytes = (unsigned char *)&in.s_addr;
    snprintf(buf, sizeof(buf), "%u.%u.%u.%u",
             bytes[0], bytes[1], bytes[2], bytes[3]);
    return buf;
}

#define my_ntohs(x) my_htons(x)  // Since our `my_htons` function is symmetric

int main() {
    // Variable declarations
    int sockfd;
    int newsockfd;
    int valread;
    int valwrite;
    char buffer[BUFFER_SIZE];
    char resp[] = "HTTP/1.0 200 OK\r\n"
                  "Server: webserver-c\r\n"
                  "Content-type: text/html\r\n\r\n"
                  "<html>hello, world</html>\r\n";
    struct sockaddr_in host_addr;
    struct sockaddr_in client_addr;
    unsigned int client_addrlen;
    char method[16], uri[256], version[16];
    char *ptr;
    char *method_ptr;
    char *uri_ptr;
    char *version_ptr;

    // Zero out the sockaddr_in structures
    memset(&host_addr, 0, sizeof(host_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("webserver (socket)");
        return 1;
    }
    printf("socket created successfully\n");

    // Prepare the sockaddr_in structure
    host_addr.sin_family = AF_INET;
    host_addr.sin_addr.s_addr = INADDR_ANY;
    host_addr.sin_port = my_htons(PORT);

    // Bind the socket to the address
    if (bind(sockfd, (struct sockaddr *)&host_addr, sizeof(host_addr)) != 0) {
        perror("webserver (bind)");
        return 1;
    }
    printf("socket successfully bound to address\n");

    // Listen for incoming connections
    if (listen(sockfd, 5) != 0) {
        perror("webserver (listen)");
        return 1;
    }
    printf("server listening on port %d\n", PORT);

    for (;;) {
        client_addrlen = sizeof(client_addr);
        newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addrlen);
        if (newsockfd < 0) {
            perror("webserver (accept)");
            continue;
        }
        printf("connection accepted\n");

        valread = read(newsockfd, buffer, BUFFER_SIZE - 1);
        if (valread < 0) {
            perror("webserver (read)");
            close(newsockfd);
            continue;
        }
        buffer[valread] = '\0';

        // Initialize parsing pointers
        ptr = buffer;
        method_ptr = method;
        uri_ptr = uri;
        version_ptr = version;

        // Extract method
        while (*ptr != ' ' && *ptr != '\0') {
            *method_ptr++ = *ptr++;
        }
        *method_ptr = '\0';
        if (*ptr == ' ') ptr++;

        // Extract URI
        while (*ptr != ' ' && *ptr != '\0') {
            *uri_ptr++ = *ptr++;
        }
        *uri_ptr = '\0';
        if (*ptr == ' ') ptr++;

        // Extract version
        while (*ptr != '\r' && *ptr != '\n' && *ptr != '\0') {
            *version_ptr++ = *ptr++;
        }
        *version_ptr = '\0';

        printf("[%s:%u] %s %s %s\n", my_inet_ntoa(client_addr.sin_addr), my_ntohs(client_addr.sin_port), method, uri, version);

        valwrite = write(newsockfd, resp, sizeof(resp) - 1);
        if (valwrite < 0) {
            perror("webserver (write)");
            close(newsockfd);
            continue;
        }

        close(newsockfd);
    }

    // Comment out unreachable code
    // close(sockfd);
    // return 0;
}
