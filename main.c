#include "authentication.h"
#include "transactions.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
#include "transactions.h"

# define ADMIN "admin"
# define ADMIN_PASS "admin@123"

int sock_fd;
typedef struct arg1{
    int new_sock_fd;
    char* username;
    char* password;
}arg;

void sigint_handler(int sig_num) {
    close(sock_fd);
    exit(0);
}

void user_menu(int new_sock_fd, char* username, char* password) {
    printf("User menu\n");

    char prompt[] = "\nChoose: \n\n1. Display books\n2. Issue book\n3. Return book\n4. Logout\n\nEnter choice: ";
    int length_of_prompt = strlen(prompt) + 1;
    write(new_sock_fd, &length_of_prompt, sizeof(length_of_prompt));

    char buffer[100];
    read(new_sock_fd, buffer, sizeof(buffer));
    write(new_sock_fd, prompt, strlen(prompt) + 1);

    while(1) {
        int user_choice;
        read(new_sock_fd, &user_choice, sizeof(user_choice));

        if (user_choice == 4) {
            break;
        }

        switch (user_choice) {
            case 1:{
                break;
            }
            case 2:{
                int book_id;
                read(new_sock_fd, &book_id, sizeof(book_id));

                int book_status = issue_book(book_id, username, password);
                write(new_sock_fd, &book_status, sizeof(book_status));
                break;
            }
            case 3:{
                break;
            }
            default:
                printf("Invalid choice\n");
                break;
        }
    }
}

void admin_menu(int new_sock_fd) {
    printf("\nAdmin menu\n");

    //char prompt[] = "\nChoose: \n\n1. Add user\n2. Delete user\n3. Modify user\n4. Add book\n5. Delete book\n6. Modify book\n7. Display books\n8. Display transactions\n9. Exit\n\nEnter choice: ";
    char prompt[] = "\nChoose: \n\n1. Add user\n2. Modify user\n3. Add book\n4. Delete book\n5. Modify book\n6. Display books\n7. Exit\n\nEnter choice: ";
    int length_of_prompt = strlen(prompt) + 1;
    write(new_sock_fd, &length_of_prompt, sizeof(length_of_prompt));
    
    char buffer[100];
    read(new_sock_fd, buffer, sizeof(buffer));
    write(new_sock_fd, prompt, strlen(prompt) + 1); 

    while(1) {
        int user_choice;
        read(new_sock_fd, &user_choice, sizeof(user_choice)); 

        if (user_choice == 7) {
            break;
        }

        switch (user_choice) {
            case 1:{
                char username[100];
                char password[100];
                char buffer[] = "Ready for input\n";
                
                read(new_sock_fd, username, 100);
                write(new_sock_fd, buffer, sizeof(buffer));
                read(new_sock_fd, password, 100);

                int user_status = create_user(username, password);
                printf("%d\n", user_status);
                write(new_sock_fd, &user_status, sizeof(user_status));
            }
                break;
            // case 2:{
            //     char username[100];
            //     char password[100];
            //     char buffer[] = "Ready for input\n";

            //     read(new_sock_fd, username, 100);
            //     write(new_sock_fd, buffer, sizeof(buffer));
            //     read(new_sock_fd, password, 100);

            //     int user_status = delete_user(username, password);
            //     write(new_sock_fd, &user_status, sizeof(user_status));
            // }
            //     break;
            case 2:{
                char username[100];
                char password[100];
                char new_password[100];
                char buffer[] = "Ready for input\n";

                read(new_sock_fd, username, 100);
                write(new_sock_fd, buffer, sizeof(buffer));
                read(new_sock_fd, password, 100);

                write(new_sock_fd, buffer, sizeof(buffer));
                read(new_sock_fd, new_password, 100);

                int user_status = modify_user(username, password, new_password);
                write(new_sock_fd, &user_status, sizeof(user_status));
            }
                break;
            case 3:{
                char book_title[100];
                char author[100];
                int quantity;

                read(new_sock_fd, book_title, 100);
                read(new_sock_fd, author, 100);
                read(new_sock_fd, &quantity, sizeof(quantity));

                int book_status = create_book_struct(book_title, author, quantity, 4);
                write(new_sock_fd, &book_status, sizeof(book_status));
            }
                break;
            case 4:{
                char book_title[100];
                char author[100];

                read(new_sock_fd, book_title, 100);
                read(new_sock_fd, author, 100);

                int book_status = delete_book(book_title, author);
                write(new_sock_fd, &book_status, sizeof(book_status));
            }
                break;
            case 5:{
                int case_id, id, quantity;
                char buffer[] = "Ready for input\n";
                char title[100], author[100];

                read(new_sock_fd, &id, sizeof(id));
                write(new_sock_fd, buffer, sizeof(buffer));
                read(new_sock_fd, &case_id, sizeof(case_id));
                write(new_sock_fd, buffer, sizeof(buffer));

                switch(case_id) { 
                    case 1:{
                        read(new_sock_fd, title, 100);
                    }
                        break;
                    case 2:{
                        read(new_sock_fd, author, 100);
                    }
                        break;
                    case 3:{
                        read(new_sock_fd, &quantity, sizeof(quantity));
                    }
                        break;
                }

                int book_status = update_book(id, title, author, quantity, case_id);
                write(new_sock_fd, &book_status, sizeof(book_status));
            }
                break;
            case 6:{
                int num_of_books = get_num_of_books();

                if (num_of_books == ERROR){
                    continue;
                }

                struct Book** books = (struct Book**)malloc(num_of_books * sizeof(struct Book*));
                int book_status = get_books(books);

                char buffer[100];
                write(new_sock_fd, &num_of_books, sizeof(num_of_books));
                read(new_sock_fd, buffer, sizeof(buffer));

                for (int i = 0; i < num_of_books; i++){
                    int id = books[i]->id;
                    char* title = books[i]->title;
                    char* author = books[i]->author;
                    int quantity = books[i]->quantity_in_stock;

                    write(new_sock_fd, &id, sizeof(id));
                    read(new_sock_fd, buffer, sizeof(buffer));

                    write(new_sock_fd, title, strlen(title) + 1);
                    read(new_sock_fd, buffer, sizeof(buffer));

                    write(new_sock_fd, author, strlen(author) + 1);
                    read(new_sock_fd, buffer, sizeof(buffer));

                    write(new_sock_fd, &quantity, sizeof(quantity));
                    read(new_sock_fd, buffer, sizeof(buffer));
                }
            }
                break;
            case 7:
                break;
        }
    }
    return ;
}

void* authorize_response(void* args) {
    int new_sock_fd = ((arg*)args)->new_sock_fd;
    char* username = ((arg*)args)->username;
    char* password = ((arg*)args)->password;

    int admin_sign = 2;
    int re_sign = 1;

    while (admin_sign != 1 && admin_sign != 0) {
        char start_prompt[] = "Is this an admin login? Type 1 for yes, and 0 for no... (0/1): ";
        send(new_sock_fd, start_prompt, sizeof(start_prompt), 0);
        recv(new_sock_fd, &admin_sign, sizeof(admin_sign), 0);
    }


    while(re_sign) {
        char username_prompt[] = "Enter username: ";
        char password_prompt[] = "Enter password: ";
        char username_response[100];
        char password_response[100];

        send(new_sock_fd, username_prompt, sizeof(username_prompt), 0);
        recv(new_sock_fd, username_response, sizeof(username_response), 0);
        send(new_sock_fd, password_prompt, sizeof(password_prompt), 0);
        recv(new_sock_fd, password_response, sizeof(password_response), 0);

        strcpy(username, username_response);
        strcpy(password, password_response);

        if (admin_sign == 0) {
            void* rec = authenticate(username, password, &re_sign);
            send(new_sock_fd, &re_sign , sizeof(re_sign), 0);
        }
        else{
            if (admin_sign == 1 && strcmp(username, ADMIN) == 0 && strcmp(password, ADMIN_PASS) == 0) {
                re_sign = 0;
            }
            send(new_sock_fd, &re_sign , sizeof(re_sign), 0);
        }
    }

    if (re_sign == 0) {
        if (admin_sign == 1) {
            admin_menu(new_sock_fd);
        }
        else {
            user_menu(new_sock_fd, username, password);
        }
    }
}

int main() {

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    signal(SIGINT, sigint_handler);

    if (sock_fd == -1) {
        printf("Socket creation failed\n");
        exit(1);
    }

    struct sockaddr_in server_address, client_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(INET_ADDR);
    server_address.sin_port = htons(PORT);

    if (bind(sock_fd, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        printf("Binding failed\n");
        exit(0);
    };

    listen(sock_fd, BACKLOG);

    printf("##       ########  ########  ##     ##  ######  \n");
    printf("##       ##     ## ##     ## ###   ### ##    ## \n");
    printf("##       ##     ## ##     ## #### #### ##       \n");
    printf("##       ##     ## ########  ## ### ##  ######  \n");
    printf("##       ##     ## ##     ## ##     ##       ## \n");
    printf("##       ##     ## ##     ## ##     ## ##    ## \n");
    printf("######## ########  ########  ##     ##  ######  \n");

    while (1) {
        socklen_t client_address_size = sizeof(client_address);
        int new_sock_fd = accept(sock_fd, (struct sockaddr*)&client_address, &client_address_size);

        pthread_t thread_id;
        arg* args = malloc(sizeof(arg));
        args->new_sock_fd = new_sock_fd;
        args->username = malloc(100 * sizeof(char));
        args->password = malloc(100 * sizeof(char));

        if (pthread_create(&thread_id, NULL, (void*)authorize_response, (void*)args) != 0) {
            exit(0);
        }

        if (pthread_detach(thread_id) < 0) {
            printf("Thread detach failed\n");
        }
    }
    return 0;
}