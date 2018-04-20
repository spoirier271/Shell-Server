

#include "myunp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ARRAY_SIZE_MAX 1000

int main(int argc, char **argv)
{
	//initializations
	int     listenfd, connfd, i;
	struct  sockaddr_in servaddr;
	char buff[MAXLINE];
	char command_input[MAXLINE];
	char result[ARRAY_SIZE_MAX];
	char str[ARRAY_SIZE_MAX];
	FILE *fp;
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	//initialize socket
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(15616); /* daytime server */

	//bind socket
    Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
	
	//set to listening
    Listen(listenfd, LISTENQ);
	
	//accept connection from client
	connfd = Accept(listenfd, (SA *) NULL, NULL);
	
	//connection loop
    for ( ; ; ) {
    
    	//initialize buffers
    	for (i = 0; i < MAXLINE; i++) {
    		buff[i] = 0;
    		command_input[i] = 0;
		}
		
		//initialize result array
		for (i = 0; i < ARRAY_SIZE_MAX; i++) {
			str[i] = 0;
			result[i] = 0;
		}
		
		//print prompt to string		
        snprintf(buff, sizeof(buff), "%s\r\n", "What is your command?");
        
        //Prompt client for command
        Write(connfd, buff, strlen(buff));
        
        //Read command input from client
		Read(connfd, command_input, MAXLINE);
		if(strcmp(command_input,"exit") == 0)
			break;

		//Execute command and store into file
		fp = popen(command_input, "r");
		result[0] = 'a'; //control byte-not EOF
		
		//Write executed result to client
		while (fgets(str, sizeof(str)-1, fp) != NULL) {
			strcpy(result+1,str);
			Write(connfd, result, strlen(result));
		}
		
		//write special character to client to indicate no more lines to be sent
		result[0] = 'b';
		result[1] = 0;
		Write(connfd, result, strlen(result));
		
		//read special character from client to indicate client is ready for next command
		Read(connfd, command_input, MAXLINE);
		if(command_input[0] != 'b')
			printf("Invalid response from client\n");
		pclose(fp);

    }
    
    //close connection with client
	Close(connfd);
}
