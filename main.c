#include "authentication.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <pthread.h>
#include <signal.h>

# define PORT 8080
# define INET_ADDR "127.0.0.1"
# define BACKLOG 10

int sock_fd;
typedef struct arg1{
    int new_sock_fd;
    char* username;
    char* password;
}arg;

void sigint_handler(int sig_num) {
    close(sock_fd);
    exit(0);
}

void* authorize_response(void* args) {
    int new_sock_fd = ((arg*)args)->new_sock_fd;
    char* username = ((arg*)args)->username;
    char* password = ((arg*)args)->password;

    int re_sign = 1;

    while(re_sign) {
        char username_prompt[] = "Enter username: ";
        char password_prompt[] = "Enter password: ";
        char username_response[100];
        char password_response[100];

        send(new_sock_fd, username_prompt, sizeof(username_prompt), 0);
        recv(new_sock_fd, username_response, sizeof(username_response), 0);
        send(new_sock_fd, password_prompt, sizeof(password_prompt), 0);
        recv(new_sock_fd, password_response, sizeof(password_response), 0);

        strcpy(username, username_response);
        strcpy(password, password_response);

        void* rec = authenticate(username, password, &re_sign);
        send(new_sock_fd, &re_sign , sizeof(re_sign), 0);
        display(rec);
    }
}

int main() {
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    signal(SIGINT, sigint_handler);

    if (sock_fd == -1) {
        printf("Socket creation failed\n");
        exit(1);
    }

    struct sockaddr_in server_address, client_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(INET_ADDR);
    server_address.sin_port = htons(PORT);

    if (bind(sock_fd, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        printf("Binding failed\n");
    };
    listen(sock_fd, BACKLOG);

    while (1) {
        socklen_t client_address_size = sizeof(client_address);
        int new_sock_fd = accept(sock_fd, (struct sockaddr*)&client_address, &client_address_size);

        pthread_t thread_id;
        arg* args = malloc(sizeof(arg));
        args->new_sock_fd = new_sock_fd;
        args->username = malloc(100 * sizeof(char));
        args->password = malloc(100 * sizeof(char));

        if (pthread_create(&thread_id, NULL, (void*)authorize_response, (void*)args) != 0) {
            exit(0);
        }
    }
    return 0;
}