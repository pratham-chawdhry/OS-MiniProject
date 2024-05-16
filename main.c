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

# define PORT 8080
# define INET_ADDR "127.0.0.1"
# define BACKLOG 10

void authorize_response(int new_sock_fd, char* username, char* password) {
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
}

int main() {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (sock_fd == -1) {
        printf("Socket creation failed\n");
        exit(1);
    }

    struct sockaddr_in server_address, client_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(INET_ADDR);
    server_address.sin_port = htons(PORT);

    bind(sock_fd, (struct sockaddr*)&server_address, sizeof(server_address));
    listen(sock_fd, BACKLOG);

    while (1) {
        socklen_t client_address_size = sizeof(client_address);
        int new_sock_fd = accept(sock_fd, (struct sockaddr*)&client_address, &client_address_size);

        int child = fork();
        if (child == 0) {
            int re_sign = 1;
            while(re_sign) {
                char* username = malloc(100 * sizeof(char));
                char* password = malloc(100 * sizeof(char));
                authorize_response(new_sock_fd, username, password);

                void* rec = authenticate(username, password, &re_sign);
                send(new_sock_fd, &re_sign , sizeof(re_sign), 0);
                display(rec);
            }

        }
    }
    return 0;
}