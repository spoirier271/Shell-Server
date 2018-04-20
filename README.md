# Shell_Server
Language: C
Description: Client/Server Socket Programs that process and return shell commands issued by client 

myclient.c: Client program that accepts user input in the form of shell commands which are forwarded to server.
myserver.c: Server program that binds listening socket, accepts connections from client program, processes shell command and returns output to client.
myunp.c: Wrapper functions for C POSIX socketlibrary
myunp.h: Header for wrapper functions in myunp.c
