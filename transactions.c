#include "authentication.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "transactions.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <pthread.h>
#include <signal.h>
#include "macros.h"
#include "book.h"
#include <fcntl.h>


int issue_book(int book_id, char* username, char* password) {
    struct Authentication auth;
    auth.key = -1;
    strcpy(auth.username, username);
    strcpy(auth.password, password);
    for (int i = 0; i < 20; i++) auth.borrow_items[i] = 0;
    auth.is_deleted = 0;

    if (search_user(&auth, 3) == USER_DOES_NOT_EXIST) { //Check if user exists
        return USER_DOES_NOT_EXIST;
    }

    struct Book book;
    book.id = book_id;

    if (search_book(&book, 2) == BOOK_DOES_NOT_EXIST) { //Check if book exists
        return BOOK_DOES_NOT_EXIST;
    }

    int empty_index = -1;

    for (int i = 0; i < BORROWING_LIMITS; i++) { //Check if borrowing limit is reached
        if (auth.borrow_items[i] == 0 && empty_index == -1) {
            empty_index = i;
        }
        if (auth.borrow_items[i] == book_id) {
            return BOOK_ALREADY_ISSUED;
        }
    }

    if (empty_index == -1) {
        return BORROWING_LIMIT_REACHED;
    }

    if (book.quantity_in_stock - 1 < 0) {
        return BOOK_OUT_OF_STOCK;
    }

    auth.borrow_items[empty_index] = book_id; //Issue book

    int fd = open("authenticator.dat", O_WRONLY | O_CREAT, 0666);
    if (fd == -1) {
        return ERROR;
    }

    int position = lseek(fd, (auth.key - 1 )* sizeof(struct Authentication), SEEK_SET);
    if (position == -1) {
        return ERROR;
    }

    if (write(fd, &auth, sizeof(struct Authentication)) == -1) {
        return ERROR;
    }
    close(fd);

    int fp = open("books.dat", O_WRONLY | O_CREAT, 0666);
    if (fp == -1) {
        return ERROR;
    }

    position = lseek(fp, (book.id - 1) * sizeof(struct Book), SEEK_SET);
    if (position == -1) {
        return ERROR;
    }

    book.quantity_in_stock--;
    if (write(fp, &book, sizeof(struct Book)) == -1) {
        return ERROR;
    }
    close(fp);
    return BOOK_ISSUED;
}