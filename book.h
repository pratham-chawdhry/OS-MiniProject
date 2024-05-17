#ifndef BOOK_H
#define BOOK_H

struct Book {
    int id;
    char title[100];
    char author[100];
    int quantity_in_stock;
    int deleted;
};

extern int num_of_unique_books;

int display_books(struct Book book);
int create_book_struct(char* title, char* author, int quantity_in_stock, int id);
int delete_book(char* title, char* author);
int get_num_of_books();
int get_books(struct Book** books);
int update_book(int id, char* new_title, char* new_author, int new_quantity_in_stock, int case_id);


#endif