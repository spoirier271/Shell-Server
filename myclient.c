

#include "myunp.h"
#include <inttypes.h> /* strtoumax */
#include <stdbool.h>

#define ARRAY_SIZE_MAX 1000

int main(int argc, char **argv)
{
	//initializations
	char command[MAXLINE];
	int     sockfd, n, i, j, k, start, bytes_read;
	char    recvline[MAXLINE + 1];
	struct sockaddr_in servaddr;
	char *c;
	char    buff[MAXLINE];
	char    result_receive[ARRAY_SIZE_MAX];
	char work_line[ARRAY_SIZE_MAX];
	bool nl_found, end_loop;
	intmax_t port = strtoimax(argv[2], &c, 10);
	k = 0;
	
	//check for valid input: user must specify ip address and port number
	if (argc != 3)
		err_quit("usage: a.out <IPaddress>");

	//create socket
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_sys("socket error");

	//initialize socket
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons((uint16_t) port);
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
		err_quit("inet_pton error for %s", argv[1]);

	//connect to server
	if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
		err_sys("connect error");

	//connection loop
	while (1) {
		//read server prompt
		Read(sockfd, recvline, MAXLINE);
		fputs(recvline, stdout);
		
		//get command from user
		strcpy(command,"");
		while(1) {
			scanf("%[^\n]%*c", &command);
			if(strlen(command) > 0) {
				break;
			}
		}
		
		//send command input to server
		snprintf(buff, sizeof(buff), "%s", command);
		Write(sockfd, buff, strlen(buff));
		
		//check for exit command
		if(strcmp(command,"exit") == 0)
			break;
		
		//initialize receive loop variables
		for(i = 0; i < sizeof(result_receive); i++)
			result_receive[i] = 0;
		nl_found = false;
		end_loop = false;
		int line_count = 0;
			
		//Get executed command result from server and print to screen
		while(1) {
			
			//initialize receive array again
			for(j = 0; j < sizeof(result_receive); j++)
				result_receive[j] = 0;
			
			//store data from server and record number of bytes read
			bytes_read = Read(sockfd, result_receive, sizeof(result_receive));
			
			//check if there are no more lines to be received
			start = 0;
			if((result_receive[start] == 'b') && (k == 0)) {
				end_loop = true;
				break;
			}
			
			//loop through read line
			for(j = 0; j < bytes_read; j++) {
			
				//if line has ended then update variable marking where read starts
				if(nl_found)
					start = j;
				
				//check if special character indicating no more bytes to be sent has been received	
				if(nl_found && (result_receive[start] == 'b')) {
					end_loop = true;
					break;
				}
				
				//check if new line character has been reached and update line to be printed
				if(result_receive[j] == '\n') {
					work_line[k++] = '\n';
					work_line[k] = 0;
					printf("%s", work_line + 1);
					nl_found = true;
					i++;
					k = 0;
				}
				
				//in the case of an incomplete line, store fragmented line but don't print until next iteration
				else {
					nl_found = false;
					work_line[k++] = result_receive[j];
				}
			}
			//if special character is detected then end loop and prepare for next command
			if(end_loop)
				break;
		}
		
// 		printf("line count: %d\n", line_count);
		
		//Acknowledge received results
		buff[0] = 'b';
		buff[1] = 0;
		Write(sockfd, buff, strlen(buff));
	}
	
	//close connection with server and exit program
	Close(sockfd);
	exit(0);
}
