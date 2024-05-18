#ifndef TRANSACTION_H
#define TRANSACTION_H

int issue_book(int book_id, char* username, char* password);
int return_book(int book_id, char* username, char* password);

#endif