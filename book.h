#ifndef BOOK_H
#define BOOK_H

struct Book {
    int id;
    char title[100];
    char author[100];
    int quantity_in_stock;
    int deleted;
};

int bookcmp(const void *a, const void *b);

#endif