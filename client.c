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

void admin_menu(int sock_fd){
    printf("Admin menu\n");
    char prompt[100];

    int length_of_prompt;
    read(sock_fd, &length_of_prompt, sizeof(length_of_prompt));

    char line[length_of_prompt];

    char buffer[] = "Ready for input\n";
    write(sock_fd, buffer, sizeof(buffer));
    read(sock_fd, line, sizeof(line));

    getchar();
    while(1) {
        char char_option = '0';
        while(char_option < '1'|| char_option > '9') {
            printf("%s", line);
            while(1){
            scanf("%c", &char_option);
                if (char_option == '\n' || char_option == ' ') {
                    continue;
                }
                else{
                    break;
                }
            }
            getchar();
        }

        int option = char_option - '0';
        write(sock_fd, &option, sizeof(option));

        if (option == 9) {
            break;
        }

        switch (option) {
            case 1:{}
                break;
            case 2:{}
                break;
            case 3:{}
                break;
            case 4:{
                char book_title[100];
                char author[100];
                int quantity = -1;

                printf("\nEnter book title: ");
                fflush(stdout);
                read(0, book_title, 100);
                int length = strcspn(book_title, "\n");
                book_title[length] = '\0';
                write(sock_fd, book_title, 100);

                printf("Enter author: ");
                fflush(stdout);
                read(0, author, 100);
                length = strcspn(author, "\n");
                author[length] = '\0';
                write(sock_fd, author, 100);

                while (quantity < 0) {
                    printf("Enter quantity: ");
                    scanf("%d", &quantity);
                }
                write(sock_fd, &quantity, sizeof(quantity));

                int book_status;
                read(sock_fd, &book_status, sizeof(book_status));

                if (book_status == BOOK_EXISTS) {
                    printf("\nBook already exists...\n");
                }
                else if (book_status == ERROR) {
                    printf("\nError while adding book...\n");
                }
                else if (book_status == BOOK_ADDED) {
                    printf("\nBook added successfully...\n");
                }
            }
                break;
            case 5:{
                char book_title[100];
                char author[100];

                printf("\nEnter book title: ");
                fflush(stdout);
                read(0, book_title, 100);
                int length = strcspn(book_title, "\n");
                book_title[length] = '\0';
                write(sock_fd, book_title, 100);

                printf("Enter author: ");
                fflush(stdout);
                read(0, author, 100);
                length = strcspn(author, "\n");
                author[length] = '\0';
                write(sock_fd, author, 100);

                int book_status;
                read(sock_fd, &book_status, sizeof(book_status));

                if (book_status == BOOK_DOES_NOT_EXIST) {
                    printf("\nBook does not exist...\n");
                }
                else if (book_status == ERROR) {
                    printf("\nError while deleting book...\n");
                }
                else if (book_status == BOOK_DELETED) {
                    printf("\nBook deleted successfully...\n");
                }
            }
                break;
            case 6:{
                int id, case_id = 0;
                char buffer[100];

                printf("Choose a valid Book ID: ");
                scanf("%d", &id);
                
                while (case_id < 1 || case_id > 3) {
                    printf("\nChoose: \n\n1. Modify book title\n2. Modify author\n3. Modify quantity\n\nEnter choice: ");
                    scanf("%d", &case_id);
                }

                write(sock_fd, &id, sizeof(id));
                read(sock_fd, buffer, sizeof(buffer));
                write(sock_fd, &case_id, sizeof(case_id));
                read(sock_fd, buffer, sizeof(buffer));

                switch (case_id) {
                    case 1:{
                        char book_title[100];

                        printf("\nEnter book title: ");
                        fflush(stdout);
                        read(0, book_title, 100);
                        int length = strcspn(book_title, "\n");
                        book_title[length] = '\0';
                        write(sock_fd, book_title, 100);
                    }
                        break;
                    case 2:{
                        char author[100];

                        printf("\nEnter author: ");
                        fflush(stdout);
                        read(0, author, 100);
                        int length = strcspn(author, "\n");
                        author[length] = '\0';
                        write(sock_fd, author, 100);
                    }
                        break;
                    case 3:{
                        int quantity = -1;

                        while (quantity < 0) {
                            printf("\nEnter quantity: ");
                            scanf("%d", &quantity);
                        }
                        write(sock_fd, &quantity, sizeof(quantity));
                    }
                        break;
                }

                int book_status;
                read(sock_fd, &book_status, sizeof(book_status));

                if (book_status == ERROR) {
                    printf("\nError while modifying book...\n");
                }
                else if (book_status == BOOK_DOES_NOT_EXIST){
                    printf("\nBook does not exist...\n");
                }
                else if (book_status == BOOK_CANT_BE_MODIFIED) {
                    printf("\nBook cannot be modified...\n");
                }
                else if (book_status == BOOK_MODIFIED) {
                    printf("\nBook modified successfully...\n");
                }
            }
                break;
            case 7:{
                char buffer[] = "Ready for input\n";

                int num_of_books;
                read(sock_fd, &num_of_books, sizeof(num_of_books));
                write(sock_fd, buffer, sizeof(buffer));

                printf("\nNumber of books: %d\n", num_of_books);

                for (int i = 0; i < num_of_books; i++){
                    int id;
                    read(sock_fd, &id, sizeof(id));
                    write(sock_fd, buffer, sizeof(buffer));

                    char title[100];
                    read(sock_fd, title, 100);
                    write(sock_fd, buffer, sizeof(buffer));

                    char author[100];
                    read(sock_fd, author, 100);
                    write(sock_fd, buffer, sizeof(buffer));

                    int quantity;
                    read(sock_fd, &quantity, sizeof(quantity));

                    printf("\nBook: %d\nTitle: %s\nAuthor: %s\nQuantity in stock: %d\n", id, title, author, quantity);

                    write(sock_fd, buffer, sizeof(buffer));
                }
            }
                break;
            case 8:{}
                break;
            case 9:{}
                break;
        }
    }
}

void authorize(int sock_fd) {
    int admin_sign = 2;
    int re_sign = 1;

    while (admin_sign != 1 && admin_sign != 0) {
        char start_prompt[100];
        recv(sock_fd, &start_prompt, sizeof(start_prompt), 0);
        printf("%s", start_prompt);
        scanf("%d", &admin_sign);
        send(sock_fd, &admin_sign, sizeof(admin_sign), 0);
    }

    while (re_sign) {
        char str[100];
        recv(sock_fd, str, 100, 0);
        printf("\n%s", str);

        char* username = malloc(100 * sizeof(char));
        char* password = malloc(100 * sizeof(char));

        scanf("%s", username);
        send(sock_fd, username, 100, 0);

        recv(sock_fd, str, 100, 0);
        printf("%s", str);

        scanf("%s", password);
        send(sock_fd, password, 100, 0);

        if (admin_sign == 0) {
            recv(sock_fd, &re_sign, sizeof(re_sign), 0);
        }
        else{
            recv(sock_fd, &re_sign, sizeof(re_sign), 0);
            if (re_sign == 0){
                printf("\nAdmin Login Successful\n");
            }
            else{
                printf("\nAdmin Login Failed. Wrong Credentials, please try again...\n");
            }
        }
    }

    if (re_sign == 0) {
        if (admin_sign == 1) {
            admin_menu(sock_fd);
        }
    }
}

int main() {
    signal(SIGINT, SIG_IGN);
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (sock_fd == -1) {
        printf("Socket creation failed\n");
        exit(1);
    }
    struct sockaddr_in server_address, client_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(INET_ADDR);
    
    if (connect(sock_fd, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        printf("Connection failed\n");
        exit(1);
    }

    authorize(sock_fd);
    return 0;
}
