#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

struct Authentication {
    int key;
    char username[100];
    char password[100];
};

struct Authentication* authenticate(char* username, char* password, int* re_sign);
void display(struct Authentication* auth);

#endif