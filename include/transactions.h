#ifndef TRANSACTION_H
#define TRANSACTION_H

int issue_book(int book_id, char* username, char* password);
int return_book(int book_id, char* username, char* password);
int get_num_of_user_books(char* username, char* password, int arr[], int* num_of_books);

#endif