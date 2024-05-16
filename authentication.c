#include "authentication.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <stdbool.h>

struct Authentication authenticator_details;

bool authenticate_user(struct Authentication* auth) {
    int fd = open("authenticator.dat", O_RDONLY, 0666);
    if (fd == -1) {
        return false;
    }

    struct Authentication comparison;
    while (read(fd, &comparison, sizeof(struct Authentication)) > 0) {
        if (strcmp(auth->username, comparison.username) == 0) {
            if (strcmp(auth->password, comparison.password) != 0) {
                printf("Wrong password. Please try again\n");
                auth->key = 0;
                return false;
            }
            else{
                comparison.key = auth->key;
            }
            close(fd);
            return true;
        }
    }
    close(fd);
    return false;
}

bool write_to_file(struct Authentication* auth) {
    int fd = open("authenticator.dat", O_WRONLY | O_CREAT, 0666);
    if (fd == -1) {
        return false;
    }

    long int position = lseek(fd, 0, SEEK_END);
    int size = sizeof(struct Authentication);
    auth->key = position / size + 1;
    printf("Position: %ld, Size: %d\n", position, size);

    write(fd, auth, sizeof(struct Authentication));
    close(fd);
    return true;
}

struct Authentication* authenticate(char* username, char* password, int* re_sign) {
    authenticator_details.key = -1;
    strcpy(authenticator_details.username, username);
    strcpy(authenticator_details.password, password);

    bool auth = authenticate_user(&authenticator_details);
    
    if (auth == true) *re_sign = 0;
    if (auth == false && authenticator_details.key != 0) {
        write_to_file(&authenticator_details);
        *re_sign = 0;
    }
    
    return &authenticator_details;  
}

void display(struct Authentication* auth) {
    printf("Key: %d\n", auth->key);
    printf("Username: %s\n", auth->username);
    printf("Password: %s\n", auth->password);
    return ;
}