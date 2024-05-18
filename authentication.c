#include "authentication.h"
#include "macros.h"
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

struct Authentication* authenticate(char* username, char* password, int* re_sign) {
    authenticator_details.key = -1;
    strcpy(authenticator_details.username, username);
    strcpy(authenticator_details.password, password);
    for (int i = 0; i < 20; i++) authenticator_details.borrow_items[i] = 0;

    bool auth = authenticate_user(&authenticator_details);
    
    if (auth == true) *re_sign = 0;
    return &authenticator_details;  
}

void display(struct Authentication* auth) {
    printf("Key: %d\n", auth->key);
    printf("Username: %s\n", auth->username);
    printf("Password: %s\n", auth->password);
    return ;
}

int search_user(struct Authentication* auth, int id) {
    int fd = open("authenticator.dat", O_RDONLY, 0666);
    if (fd == -1) {
        return ERROR;
    }

    struct Authentication comparison;
    while (read(fd, &comparison, sizeof(struct Authentication)) > 0) {
        if (comparison.is_deleted == 1) continue; //ignore deleted users
        if (id == 1){ //we need to check if a user exists, so username is important
            if (strcmp(auth->username, comparison.username) == 0) {
                close(fd);
                return USER_EXISTS;
            }
        }
        else if (id == 2){ //Deletion we need to verify if user agrees, so username password is important
            if (strcmp(auth->username, comparison.username) == 0 && strcmp(auth->password, comparison.password) == 0) {
                auth->key = comparison.key;
                auth->is_deleted = 1;
                close(fd);
                return USER_EXISTS;
            }
        }
        else if (id == 3){ //we need to check if a user exists, so username is important and verify password before changing password
            if (strcmp(auth->username, comparison.username) == 0 && strcmp(auth->password, comparison.password) == 0) {
                auth->key = comparison.key;
                for (int i = 0; i < 20; i++) auth->borrow_items[i] = comparison.borrow_items[i]; //copy borrow items
                close(fd);
                return USER_EXISTS;
            }
        }
    }

    close(fd);
    return USER_DOES_NOT_EXIST;
}

int create_user(char* username, char* password) {
    struct Authentication auth;
    auth.key = -1;
    strcpy(auth.username, username);
    strcpy(auth.password, password);
    for (int i = 0; i < 20; i++) auth.borrow_items[i] = 0;
    auth.is_deleted = 0;

    if (search_user(&auth,1) == USER_EXISTS) {
        return USER_CANT_BE_CREATED;
    }

    int fd = open("authenticator.dat", O_WRONLY | O_CREAT, 0666);
    if (fd == -1) {
        return ERROR;
    }

    int position = lseek(fd, 0, SEEK_END);

    if (position == -1) {
        return ERROR;
    }

    auth.key = position / sizeof(struct Authentication) + 1;

    if (write(fd, &auth, sizeof(struct Authentication)) == -1) {
        return ERROR;
    }
    close(fd);

    return USER_CREATED;
}

int delete_user(char* username, char* password) {
    struct Authentication auth;
    auth.key = -1;
    strcpy(auth.username, username);
    strcpy(auth.password, password);
    for (int i = 0; i < 20; i++) auth.borrow_items[i] = 0;
    auth.is_deleted = 1;

    if (search_user(&auth, 2) == USER_DOES_NOT_EXIST) {
        return USER_DOES_NOT_EXIST;
    }

    int fd = open("authenticator.dat", O_WRONLY | O_CREAT, 0666);
    if (fd == -1) {
        return ERROR;
    }

    int position = lseek(fd, (auth.key - 1 )* sizeof(struct Authentication), SEEK_SET);
    if (position == -1) {
        return ERROR;
    }

    auth.key = position / sizeof(struct Authentication) + 1;

    if (write(fd, &auth, sizeof(struct Authentication)) == -1) {
        return ERROR;
    }
    close(fd);
    return USER_DELETED;
}


int modify_user(char* username, char* password, char* new_password) {
    struct Authentication auth;
    auth.key = -1;
    strcpy(auth.username, username);
    strcpy(auth.password, password);
    for (int i = 0; i < 20; i++) auth.borrow_items[i] = 0;
    auth.is_deleted = 0;

    if (search_user(&auth, 3) == USER_DOES_NOT_EXIST) {
        return USER_DOES_NOT_EXIST;
    }

    int fd = open("authenticator.dat", O_WRONLY | O_CREAT, 0666);
    if (fd == -1) {
        return ERROR;
    }

    int position = lseek(fd, (auth.key - 1 )* sizeof(struct Authentication), SEEK_SET);
    if (position == -1) {
        return ERROR;
    }

    strcpy(auth.password, new_password);

    if (write(fd, &auth, sizeof(struct Authentication)) == -1) {
        return ERROR;
    }
    close(fd);
    return USER_MODIFIED;
}