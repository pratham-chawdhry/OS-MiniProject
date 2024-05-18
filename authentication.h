#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

struct Authentication {
    int key;
    char username[100];
    char password[100];
    int borrow_items[20];
    int is_deleted;
};

int search_user(struct Authentication* auth, int id);
int create_user(char* username, char* password);
int delete_user(char* username, char* password);
int modify_user(char* username, char* password, char* new_password);
struct Authentication* authenticate(char* username, char* password, int* re_sign);
void display(struct Authentication* auth);

#endif