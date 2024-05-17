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

int num_of_unique_books = 0;

int display_books(struct Book book) {
    printf("Book ID: %d\n", book.id);
    printf("Title: %s\n", book.title);
    printf("Author: %s\n", book.author);
    printf("Quantity in stock: %d\n", book.quantity_in_stock);
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
            close(fp);
            return BOOK_ADDED;
        }
    }
    close(fp);
    return ERROR;
}

int search_book(struct Book* book, int comparison_id) {
    int fp = open("books.dat", O_RDONLY | O_CREAT, 0666);
    if (fp == -1) {
        return ERROR;
    }

    struct Book comparison_book;
    while (read(fp, &comparison_book, sizeof(struct Book)) > 0) {
        if (comparison_book.deleted == 0) {
            if (comparison_id == 1) {
                if (strcmp(book->title, comparison_book.title) == 0 && strcmp(book->author, comparison_book.author) == 0) {
                    book->id = comparison_book.id;
                    close(fp);
                    return BOOK_EXISTS;
                }
            }
            else if (comparison_id == 2) {
                if (book->id == comparison_book.id) {
                    strcpy(book->title, comparison_book.title);
                    strcpy(book->author, comparison_book.author);
                    book->quantity_in_stock = comparison_book.quantity_in_stock;
                    close(fp);
                    return BOOK_EXISTS;
                }
            }
        }
    }
    close(fp);
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
            if (search_book(&book, 1) == BOOK_EXISTS) {
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

    int result = search_book(&book, 1);
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
            close(fp);
            return ERROR;
        }
        close(fp);
    }
    return BOOK_DELETED;
}

int get_num_of_books() {
    int fp = open("books.dat", O_RDONLY | O_CREAT, 0666);
    if (fp == -1) {
        return ERROR;
    }

    lseek(fp, 0, SEEK_SET);
    num_of_unique_books = 0;
    struct Book book;
    while (read(fp, &book, sizeof(struct Book)) > 0) {
        if (book.deleted == 0) {
            num_of_unique_books++;
        }
    }

    close(fp);
    return num_of_unique_books;
}

int get_books(struct Book** books) {
    int fp = open("books.dat", O_RDONLY | O_CREAT, 0666);
    if (fp == -1) {
        return ERROR;
    }

    lseek(fp, 0, SEEK_SET);
    int count = 0;
    struct Book book;

    while (read(fp, &book, sizeof(struct Book)) > 0) {
        if (book.deleted == 0) {
            books[count] = (struct Book*)malloc(sizeof(struct Book));
            if (books[count] == NULL) {
                close(fp);
                return ERROR; 
            }
            books[count]->id = book.id;
            strcpy(books[count]->title, book.title);
            strcpy(books[count]->author, book.author);
            books[count]->quantity_in_stock = book.quantity_in_stock;
            count++;
        }
    }

    close(fp);
    return BOOK_SUCCESS; 
}

int update_book(int id, char* new_title, char* new_author, int new_quantity_in_stock, int case_id) {
    struct Book book;
    book.id = id;

    int result = search_book(&book, 2);

    if (result == BOOK_DOES_NOT_EXIST) {
        return BOOK_DOES_NOT_EXIST;
    }
    else{
        switch (case_id) {
            case 1:
            {
                strcpy(book.title, new_title);
            } break;
            case 2:
            {
                strcpy(book.author, new_author);
            } break;
            case 3:
            {
                book.quantity_in_stock = new_quantity_in_stock;
            } break;
        }

        int book_status = search_book(&book, 1);

        if (book_status == BOOK_EXISTS && case_id != 3) {
            return BOOK_CANT_BE_MODIFIED;
        }

        display_books(book);
        int fp = open("books.dat", O_WRONLY | O_CREAT, 0666);
        if (fp == -1) {
            return ERROR;
        }

        lseek(fp, (book.id - 1) * sizeof(struct Book), SEEK_SET);
        if (write(fp, &book, sizeof(struct Book)) == -1) {
            close(fp);
            return ERROR;
        }
        close(fp);
    }

    return BOOK_MODIFIED;
}