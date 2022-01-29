#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>

#define SERVER_PORT 80
#define MAXLINE 4096
#define SA struct sockaddr


int setupServer(short port, int backlog) {
    int                 server_socket;
    struct sockaddr_in  serv_addr;

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket error!");
        return -1;
    }

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    if ((bind(server_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
        perror("Bind error!");
        return -1;
    }
    if ((listen(server_socket, backlog)) < 0) {
        perror("Listen error!");
        return -1;
    }
    return server_socket;
}

int acceptNewConnection(int server_socket) {
    size_t              addr_size = sizeof(struct sockaddr_in);
    int                 client_socket;
    struct sockaddr_in  client_addr;

    if ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, (socklen_t*)&addr_size)) < 0) {
        perror("Accept error!");
        return -1;
    }
    return client_socket;
}

#define BUFSIZE 256
//#define PATH_MAX 256

void*   handleConnection(int client_socket) {
    char    buffer[256];
    size_t  bytes_read;
    int     msg_size;
    char    actualpath[PATH_MAX+1];

    for (; (bytes_read = read(client_socket, buffer+msg_size, sizeof(buffer) - msg_size - 1)) > 0; ) {
        msg_size += bytes_read;
        if (msg_size > BUFSIZE - 1 || buffer[msg_size - 1] == '\n') break;
    }
    if (bytes_read < 0) {
        perror("Recieve error!");
    }
    buffer[msg_size-1] = 0;

    printf("REQUEST: %s\n", buffer);
    fflush(stdout);

    if (realpath(buffer, actualpath) == 0) {
        printf("ERROR(bad path): %s\n", buffer);
        close(client_socket);
        return NULL;
    }

    FILE *fp = fopen(actualpath, "r");
    if (!fp) {
        printf("ERROR(open): %s\n", buffer);
        close(client_socket);
        return NULL;
    }

    for (; (bytes_read = fread(buffer, 1, BUFSIZE, fp)) > 0; ) {
        write(client_socket, buffer, bytes_read);
    }
    close(client_socket);
    fclose(fp);
    printf("closing connection\n");
    return NULL;
}


int main() {

    int                 server_socket, client_socket;
    fd_set              current_sockets, ready_sockets;

    server_socket = setupServer(80, 10);

    FD_ZERO(&current_sockets);
    FD_SET(server_socket, &current_sockets);
    for (;true;) {

        printf("waiting for a connection on PORT %d", SERVER_PORT);
        ready_sockets = current_sockets;
        if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0) {
            perror("Select error!");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < FD_SETSIZE; ++i) {
            if (FD_ISSET(i, &ready_sockets)) {
                if (i == server_socket) {
                    // this is a new connection
                    client_socket = acceptNewConnection(server_socket);
                    FD_SET(client_socket, &current_sockets);
                } else {
                    handleConnection(i);
                    FD_CLR(i, &current_sockets);
                }

            }
        }
    }

    return 0;
}
