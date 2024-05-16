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

# define PORT 8068
# define INET_ADDR "127.0.0.1"
# define BACKLOG 10

# define ADMIN "admin"
# define ADMIN_PASS "admin@123"

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

    int admin_sign = 2;
    int re_sign = 1;

    while (admin_sign != 1 && admin_sign != 0) {
        char start_prompt[] = "Is this an admin login? Type 1 for yes, and 0 for no... (0/1): ";
        send(new_sock_fd, start_prompt, sizeof(start_prompt), 0);
        recv(new_sock_fd, &admin_sign, sizeof(admin_sign), 0);
        fflush(stdout);
        printf("%d",admin_sign);
    }


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

        if (admin_sign == 0) {
            void* rec = authenticate(username, password, &re_sign);
            send(new_sock_fd, &re_sign , sizeof(re_sign), 0);
            display(rec);
        }
        else{
            if (admin_sign == 1 && strcmp(username, ADMIN) == 0 && strcmp(password, ADMIN_PASS) == 0) {
                re_sign = 0;
            }
            send(new_sock_fd, &re_sign , sizeof(re_sign), 0);
        }
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