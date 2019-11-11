
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>
#include "stems.h"

char *name;
char *nameval;
char *time1;
char *timeval;
char *value;
char *valueval;


void clientSend(int fd, char *filename,int port, char *body)
{
  char buf[MAXLINE];
  char hostname[MAXLINE];
 
  Gethostname(hostname, MAXLINE);
 
  /* Form and send the HTTP request */
  sprintf(buf, "POST %s HTTP/1.1\n", filename);
  sprintf(buf, "%sHost: %s:%d\n", buf, hostname,port);
  sprintf(buf, "%sConnection: keep-alive\r\n", buf);
  sprintf(buf, "%sContent-Length: %d\n\r\n", buf, strlen(body));
  sprintf(buf, "%s%s\n", buf, body);
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
    /* If you want to look for certain HTTP tags... */
    if (sscanf(buf, "Content-Length: %d ", &length) == 1);
      //printf("Length = %d\n", length);
	printf("Header:%s", buf);
	n = Rio_readlineb(&rio, buf, MAXBUF);
  }
 	
  /* Read and display the HTTP Body */
  n = Rio_readlineb(&rio, buf, MAXBUF);
  while (n > 0) {
    printf("%s", buf);
    n = Rio_readlineb(&rio, buf, MAXBUF);
  }
}
 
/* currently, there is no loop. I will add loop later */
void userTask(char *myname, char *hostname, int port, char *filename,char *time, float value)
{


  int clientfd;
  char msg[MAXLINE];
 
  sprintf(msg, "name=%s&time=%s&value=%f", myname,time ,value);
  clientfd = Open_clientfd(hostname, port);
  clientSend(clientfd, filename,port, msg);
  clientPrint(clientfd);
  Close(clientfd);
}
 
void getargs_cp(char *hostname, int *port, char *filename, int *threshold)
{
  FILE *fp;
 
  fp = fopen("config-pc.txt", "r");
  if (fp == NULL)
    unix_error("config-cp.txt file does not open.");
 
 
  fscanf(fp, "%s", hostname);
  fscanf(fp, "%d", port);
  fscanf(fp, "%s", filename);
  fscanf(fp, "%d", threshold);
  fclose(fp);
}

void parse(char *str)
{
	char delimeter[10]="=&";
	name = strtok(str,delimeter);
	nameval = strtok(NULL,delimeter);
	time1 = strtok(NULL,delimeter);
	timeval = strtok(NULL,delimeter);
	value=strtok(NULL,delimeter);
	valueval=strtok(NULL,delimeter);
}



int main(void)
{

  //char msg[SIZE];
  int filedes;
  int nread, cnt;





  char myname[MAXLINE], hostname[MAXLINE], filename[MAXLINE];
  int port;
  int threshold;

  char cmd1[MAXLINE];
  char cmd2[MAXLINE];
  int size;
  getargs_cp(hostname, &port, filename, &threshold);


 
  if(mkfifo("./fifo",0666) == -1){

        //printf("fail to call fifo()\n");

    }


    if((filedes = open("./fifo", O_RDWR)) < 0){

        printf("fail to call fifo()\n");


    }


memset(cmd2,NULL,MAXLINE);
	
        if((nread = read(filedes, cmd1, MAXBUF)) < 0 ){

            printf("fail to call read()\n");

            exit(1);

        }
	size = atoi(cmd1);
        if((nread = read(filedes, cmd2, size)) < 0 ){

          printf("fail to call read()\n");

            exit(1);

  
 	 }
	cmd2[strlen(cmd2)-1]='\0';
        //printf("받은 메세지: %s\n", cmd2);

	parse(cmd2);
	
	if(atof(valueval)>threshold)
	{
		
		userTask(nameval,hostname,port,filename,timeval,atof(valueval));
	

	}
	
	
	


    unlink("./fifo");



 
  return(0);

} 
