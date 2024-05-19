# LDBMS (Library DBMS)
﻿
The Library Database Management System (DBMS) is designed to efficiently manage library operations while ensuring top-notch security. Key features include:

1.  **Security**: User authentication and password-protected admin access.
2.  **Book Management**: Comprehensive cataloging and tracking of book availability.
3.  **Efficiency**: Utilizes efficient file handling, multithreading, and socket programming.
4.  **Concurrent Access**: Supports simultaneous user access for seamless operations.
5.  **Performance and Consistency**: Optimized performance and data consistency maintenance.

With these features, the system offers a secure, efficient, and reliable solution for library management needs.

## Features 
  - Admin User
	- Add Users
	- Modify User's Password
	- Add Books
	- Delete Books
	- Modify Books
	- See Books in Library
 - Normal User 
	- Issue Books
	- Return Books
	- See Books to be Returned

## Project structure
```src``` : contains source code for client and server \
```include``` : contains all header files \
```bin``` : will store final binaries and other files produced during runtime like binary files
## How to run
### installing  prerequisites
#### ubuntu
``` bash
sudo apt update
sudo apt install gcc make -y
```
### building the project 
``` bash
git clone https://github.com/pratham-chawdhry/OS-MiniProject.git
cd OS-MiniProject
make all #to build both server and client
make server #to build server only
make client #to build client only
make clean #to clean bin folder
```
### running the project
to run the server 
``` bash 
cd bin
./server
```
to run the client
```  bash
cd bin
./client
```
Note : client can't run without starting the server