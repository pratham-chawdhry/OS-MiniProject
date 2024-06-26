#ifndef MACROS_H
#define MACROS_H

#include <pthread.h>

extern pthread_mutex_t mutex_users;
extern pthread_cond_t mutex_books;

#define PORT 6609

#define INET_ADDR "127.0.0.1"
#define BACKLOG 10

#define ERROR -1
#define BOOK_EXISTS 0
#define BOOK_DOES_NOT_EXIST 1
#define BOOK_YET_TO_BE_FOUND 2
#define BOOK_ADDED 3
#define BOOK_DELETED 4
#define BOOK_MODIFIED 5
#define BOOK_SUCCESS 6
#define BOOK_CANT_BE_MODIFIED 7

#define USER_CREATED 8
#define USER_CANT_BE_CREATED 9
#define USER_EXISTS 10
#define USER_DOES_NOT_EXIST 11
#define USER_DELETED 12
#define USER_MODIFIED 13

#define BOOK_ALREADY_ISSUED 14
#define BORROWING_LIMIT_REACHED 15
#define BOOK_OUT_OF_STOCK 16
#define BOOK_ISSUED 17
#define BOOK_ISNT_BORROWED 18
#define BOOK_RETURNED 19

#define BORROWING_LIMITS 5

#define SUCCESS 20
#define FAILURE 21

#endif