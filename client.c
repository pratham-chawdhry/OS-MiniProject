#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <signal.h>

# define PORT 8080
# define INET_ADDR "127.0.0.1"

void authorize(int sock_fd) {
    int re_sign = 1;
    while (re_sign) {
        char str[100];
        recv(sock_fd, str, 100, 0);
        printf("%s", str);

        char* username = malloc(100 * sizeof(char));
        char* password = malloc(100 * sizeof(char));

        scanf("%s", username);
        send(sock_fd, username, 100, 0);

        recv(sock_fd, str, 100, 0);
        printf("%s", str);

        scanf("%s", password);
        send(sock_fd, password, 100, 0);

        recv(sock_fd, &re_sign, sizeof(re_sign), 0);
    }
}

int main() {
    signal(SIGINT, SIG_IGN);
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (sock_fd == -1) {
        printf("Socket creation failed\n");
        exit(1);
    }
    struct sockaddr_in server_address, client_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(INET_ADDR);
    
    if (connect(sock_fd, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        printf("Connection failed\n");
        exit(1);
    }

    authorize(sock_fd);
    return 0;
}
