/*
 * clientGet.c: A very, very primitive HTTP client for console.
 * 
 * To run, prepare config-cg.txt and try: 
 *      ./clientGet
 *
 * Sends one HTTP request to the specified HTTP server.
 * Prints out the HTTP response.
 *
 * For testing your server, you will want to modify this client.  
 *
 * When we test your server, we will be using modifications to this client.
 *
 */


#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <semaphore.h>
#include "stems.h"
#include <unistd.h>
#include <string.h>

/*
 * Send an HTTP request for the specified file 
 */
void clientSend(int fd, char *filename)
{
  char buf[MAXLINE];
  char hostname[MAXLINE];

  Gethostname(hostname, MAXLINE);

  /* Form and send the HTTP request */
  sprintf(buf, "GET %s HTTP/1.1\n", filename);
  sprintf(buf, "%shost: %s\n", buf, hostname);
  sprintf(buf, "%sConnection: keep-alive\n\r\n", buf);
  Rio_writen(fd, buf, strlen(buf));
}
  
/*
 * Read the HTTP response and print it out
 */
void clientPrint(int fd)
{
  rio_t rio;
  char buf[MAXBUF];  
  int length = 0;
  int n;
  
  Rio_readinitb(&rio, fd);

  /* Read and display the HTTP Header */
  n = Rio_readlineb(&rio, buf, MAXBUF);
  while (strcmp(buf, "\r\n") && (n > 0)) {
    //printf("Header: %s", buf);
    printf("Header:%s", buf);
	n = Rio_readlineb(&rio, buf, MAXBUF);

    /* If you want to look for certain HTTP tags... */
    if (sscanf(buf, "Content-Length: %d ", &length) == 1) {
      //printf("Length = %d\n", length);
    }
	
  }

  /* Read and display the HTTP Body */
  n = Rio_readlineb(&rio, buf, MAXBUF);
  while (n > 0) {
    printf("%s", buf);
    n = Rio_readlineb(&rio, buf, MAXBUF);
  }
}

/* currently, there is no loop. I will add loop later */
void userTask(char hostname[], int port, char webaddr[])
{
  int clientfd;

  clientfd = Open_clientfd(hostname,port);
  clientSend(clientfd, webaddr);
  clientPrint(clientfd);
  Close(clientfd);
}

void getargs_cg(char hostname[], int *port, char webaddr[])
{
  FILE *fp;

  fp = fopen("config-cg.txt", "r");
  if (fp == NULL)
    unix_error("config-cg.txt file does not open.");

  fscanf(fp, "%s", hostname);
  fscanf(fp, "%d", port);
  fscanf(fp, "%s", webaddr);
  fclose(fp);
}


int main(void)
{
  char hostname[MAXLINE], webaddr[MAXLINE], webaddr2[MAXLINE];
  int port;
  char *command[3];
  char cmd[MAXLINE];
  pid_t pid;
  char *value;
  int value2;
 
  char b[1000];
  pid = fork();
if(pid==0)
{
	execve("./alarmserver",(NULL),environ);
}else
{
	while(1)
	{
	strcpy(webaddr,webaddr2);
	 getargs_cg(hostname, &port, webaddr);
	    command[0];
	    command[1];
	    command[2];
	    printf(">>");
	    fgets(cmd,MAXLINE,stdin);   

	    
	    command[0]=strtok(cmd," ");
	    command[1] =strtok(NULL," ");
	    command[2] =strtok(NULL,"");
	    
	    
	    if(!strcasecmp(command[0],"help\n"))
	    {
	    printf("LIST : sensorList table 에 있는 sensor들의 name들을 반환한다.\n");
	    printf("INFO s-name : sensor 's-name'의 가장 최근 (time,data)를 반환한다.\n");
	    printf("GET s-name :  's-name'의 가장 최근 (time,data)를 반환한다.\n");
	    printf("GET s-name $$ : sensor 's-name'의 가장 최근 $$개의 (time,data)를 반환한다.\n");
	    printf("value <n> : set sensor value to <n>.\n");
	    printf("eixt : clientGet 프로세스를 종료한다.\n");
	    printf("quit : clientGet 프로세스를 종료한다.\n");

	    continue;
	    }

	    if(!strcasecmp(command[0],"exit"))
		return 0;
	    //printf("cmd1 : %s \n cmd2 : %s\n",command[0],command[1]);

	    if(!strcasecmp(command[0],"list\n")&&command[1]==NULL)
		{
		
		sprintf(b,"?command=LIST");
		strcat(webaddr,b);
		//printf("%s",webaddr);
	   
		}

	    else if(!strcasecmp(command[0],"info"))
	    {
		command[1][strlen(command[1])-1]='\0';
		value = command[1];
		sprintf(b,"?command=INFO&value=%s",value);
		strcat(webaddr,b);
		//printf("%s\n",webaddr);
		
	    }
	    else if(!strcasecmp(command[0],"get")&&command[1]!=NULL&&command[2]==NULL) // 세번째 인자 없을때
	    {
		command[1][strlen(command[1])-1]='\0';
		value = command[1];
		sprintf(b,"?NAME=%s&N=1",value);
		strcat(webaddr,b);
		//printf("%s\n",webaddr);
	    }
	    else if(!strcasecmp(command[0],"get")&&command[1]!=NULL&&command[2]!=NULL)
	    {
		
		value = command[1];
		value2=atoi(command[2]);
		sprintf(b,"?NAME=%s&N=%d",value ,value2);
		strcat(webaddr,b);
		//printf("%s\n",webaddr);
	    }

	    else
		printf("잘못된 명령어입니다.");
		
	userTask(hostname, port, webaddr);
	
	memset(webaddr,NULL,sizeof(char)*MAXLINE);
	}
	kill(pid,SIGINT);
}
  
  
  return(0);
}
