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
        struct Book borrowed_book;
        borrowed_book.id = auth.borrow_items[i];

        if (search_book(&borrowed_book, 2) == BOOK_DOES_NOT_EXIST) {
            auth.borrow_items[i] = 0;
        }
        if (auth.borrow_items[i] == 0 && empty_index == -1) {
            empty_index = i;
        }
        if (auth.borrow_items[i] == book_id) {
            return BOOK_ALREADY_ISSUED;
        }

        printf("%d\n", auth.borrow_items[i]);
    }

    if (empty_index == -1) {
        return BORROWING_LIMIT_REACHED;
    }

    if (book.quantity_in_stock - 1 < 0) {
        return BOOK_OUT_OF_STOCK;
    }

    auth.borrow_items[empty_index] = book_id; //Issue book

    //Issue book, delete book, try to either search or return borrowed book
    
    int fd = open("authenticator.dat", O_WRONLY | O_CREAT, 0666);
    if (fd == -1) {
        printf("Error while modifying book 1...\n");
        return ERROR;
    }

    int position = lseek(fd, (auth.key - 1 )* sizeof(struct Authentication), SEEK_SET);
    if (position == -1) {
        printf("Error while modifying book 2...\n");
        return ERROR;
    }

    if (write(fd, &auth, sizeof(struct Authentication)) == -1) {
        printf("Error while modifying book 3...\n");
        return ERROR;
    }
    close(fd);

    int fp = open("books.dat", O_WRONLY | O_CREAT, 0666);
    if (fp == -1) {
        printf("Error while modifying book 4...\n");
        return ERROR;
    }

    position = lseek(fp, (book.id - 1) * sizeof(struct Book), SEEK_SET);
    if (position == -1) {
        printf("Error while modifying book 5...\n");
        return ERROR;
    }

    book.quantity_in_stock--;
    if (write(fp, &book, sizeof(struct Book)) == -1) {
        printf("Error while modifying book 6...\n"); 
        return ERROR;
    }
    close(fp);
    return BOOK_ISSUED;
}

int return_book(int book_id, char* username, char* password) {
    struct Authentication auth;
    auth.key = -1;
    strcpy(auth.username, username);
    strcpy(auth.password, password);
    for (int i = 0; i < 20; i++) auth.borrow_items[i] = 0;
    auth.is_deleted = 0;

    if (search_user(&auth, 3) == USER_DOES_NOT_EXIST) { //Check if user exists
        return USER_DOES_NOT_EXIST;
    }

    struct Book book; //Check if book is issued
    book.id = book_id;

    int i;

    for (i = 0; i < BORROWING_LIMITS; i++) {
        if (auth.borrow_items[i] == book_id) { //break if book is found
            break;
        }
        else if (i == BORROWING_LIMITS - 1) { //if book is not found, return book is not issued
            return BOOK_ISNT_BORROWED;
        }
    }
    
    auth.borrow_items[i] = 0;

    if (search_book(&book, 2) == BOOK_DOES_NOT_EXIST) { //Check if book exists
        return BOOK_DOES_NOT_EXIST;
    }

    //user exists, book exists and book is borrowed

    int fd = open("authenticator.dat", O_WRONLY | O_CREAT, 0666);
    if (fd == -1) {
        printf("Error while modifying book 1...\n");
        return ERROR;
    }

    int position = lseek(fd, (auth.key - 1 )* sizeof(struct Authentication), SEEK_SET);
    if (position == -1) {
        printf("Error while modifying book 2...\n");
        return ERROR;
    }

    if (write(fd, &auth, sizeof(struct Authentication)) == -1) {
        printf("Error while modifying book 3...\n");
        return ERROR;
    }
    close(fd);

    int fp = open("books.dat", O_WRONLY | O_CREAT, 0666);
    if (fp == -1) {
        printf("Error while modifying book 4...\n");
        return ERROR;
    }

    position = lseek(fp, (book.id - 1) * sizeof(struct Book), SEEK_SET);
    if (position == -1) {
        printf("Error while modifying book 5...\n");
        return ERROR;
    }

    book.quantity_in_stock++;
    if (write(fp, &book, sizeof(struct Book)) == -1) {
        printf("Error while modifying book 6...\n"); 
        return ERROR;
    }
    close(fp);
    return BOOK_RETURNED;
}