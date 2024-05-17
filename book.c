#include "book.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <signal.h>
#include "macros.h"
#include <string.h>
#include <fcntl.h>

int display_books(struct Book book) {
    printf("Book ID: %d\n", book.id);
    printf("Title: %s\n", book.title);
    printf("Author: %s\n", book.author);

}

int add_book(struct Book book) {
    int fp = open("books.dat", O_WRONLY | O_CREAT, 0666);
    if (fp == -1) {
        return ERROR;
    }

    int position = lseek(fp, 0, SEEK_END);
    if (position != -1) {
        book.id = position / sizeof(struct Book) + 1;
        if (write(fp, &book, sizeof(struct Book)) != -1) {
            return BOOK_ADDED;
        }
    }

    return ERROR;
}

int search_book(struct Book* book) {
    int fp = open("books.dat", O_RDONLY | O_CREAT, 0666);
    if (fp == -1) {
        return ERROR;
    }

    struct Book comparison_book;
    while (read(fp, &comparison_book, sizeof(struct Book)) > 0) {
        if (comparison_book.deleted == 0) {
            if (strcmp(book->title, comparison_book.title) == 0 && strcmp(book->author, comparison_book.author) == 0) {
                book->id = comparison_book.id;
                close(fp);
                return BOOK_EXISTS;
            }
        }
    }
    return BOOK_DOES_NOT_EXIST;
}

int create_book_struct(char* title, char* author, int quantity_in_stock, int id) {
    struct Book book;
    book.id = BOOK_YET_TO_BE_FOUND;
    strcpy(book.title, title);
    strcpy(book.author, author);
    book.quantity_in_stock = quantity_in_stock;
    book.deleted = 0;

    switch (id) {
        case 4:
        {
            if (search_book(&book) == BOOK_EXISTS) {
                return BOOK_EXISTS;
            }
            else{
                return add_book(book);
            }
        }
        break;
    }
}

int delete_book(char* title, char* author) {
    struct Book book;
    book.id = BOOK_YET_TO_BE_FOUND;
    strcpy(book.title, title);
    strcpy(book.author, author);
    book.deleted = 1;

    int result = search_book(&book);
    if (result == BOOK_DOES_NOT_EXIST) {
        return BOOK_DOES_NOT_EXIST;
    }
    else{
        int fp = open("books.dat", O_WRONLY | O_CREAT, 0666);
        if (fp == -1) {
            return ERROR;
        }
        display_books(book);

        lseek(fp, (book.id - 1) * sizeof(struct Book), SEEK_SET);
        if (write(fp, &book, sizeof(struct Book)) == -1) {
            return ERROR;
        }
    }
    return BOOK_DELETED;
}

int update_book() {
    return 0;
}
