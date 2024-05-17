#ifndef BOOK_H
#define BOOK_H

struct Book {
    int id;
    char title[100];
    char author[100];
    int quantity_in_stock;
    int deleted;
};

int create_book_struct(char* title, char* author, int quantity_in_stock, int id);
int delete_book(char* title, char* author);

#endif