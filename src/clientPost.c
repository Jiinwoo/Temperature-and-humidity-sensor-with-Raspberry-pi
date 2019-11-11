/*
 * clientPost.c: A very, very primitive HTTP client for sensor
 * 
 * To run, prepare config-cp.txt and try: 
 *      ./clientPost
 *
 * Sends one HTTP request to the specified HTTP server.
 * Get the HTTP response.
 */
 
 
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>
#include "stems.h"
 
void clientSend(int fd, char *filename, char *body)
{
  char buf[MAXLINE];
  char hostname[MAXLINE];
 
  Gethostname(hostname, MAXLINE);
 
  /* Form and send the HTTP request */
  sprintf(buf, "POST %s HTTP/1.1\n", filename);
  sprintf(buf, "%sHost: %s\n", buf, hostname);
  sprintf(buf, "%sContent-Type: text/plain; charset=utf-8\n", buf);
  sprintf(buf, "%sContent-Length: %d\r\n\r\n", buf, strlen(body));
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
void userTask(char *myname, char *hostname, int port, char *filename, int a, float value)
{


  int clientfd;
  char msg[MAXLINE];
   
  sprintf(msg, "name=%s&time=%d&value=%f", myname, a, value);
  clientfd = Open_clientfd(hostname, port);
  clientSend(clientfd, filename, msg);
  clientPrint(clientfd);
  Close(clientfd);
}
 
void getargs_cp(char *hostname, int *port, char *filename,char *myname, float *value)
{
  FILE *fp;
 
  fp = fopen("config-cp.txt", "r");
  if (fp == NULL)
    unix_error("config-cp.txt file does not open.");
 
 
  fscanf(fp, "%s", hostname);
  fscanf(fp, "%d", port);
  fscanf(fp, "%s", filename);
  fscanf(fp, "%s", myname);
  fscanf(fp, "%f", value);
  fclose(fp);
}

/*void putargs_cp(char *hostname, int port, char *filename,char *myname, float value)
{
  FILE *fp;
 
  fp = fopen("config-cp.txt", "w");
  if (fp == NULL)
    unix_error("config-cp.txt file does not open.");
 
 
   fprintf(fp, "%s\r\n", hostname);
   fprintf(fp, "%d\r\n", port);
   fprintf(fp, "%s\r\n", filename);
   fprintf(fp, "%s\r\n", myname);
   fprintf(fp, "%f\r\n", value);
  fclose(fp);
}*/
 
int main(void)
{

  
  char myname[MAXLINE], hostname[MAXLINE], filename[MAXLINE];
  int port;
  float value;
  //char time1[50];

srand(time(NULL));
  char *command[2];
  printf("if you want to see commands. type 'help'\n");
  char cmd[MAXLINE];
  //float value2;
  char * time_str;

  //int a;
   getargs_cp(hostname, &port, filename, myname, &value);
while(1){
    command[0]=NULL;
    command[1]=NULL;
    printf(">>");
    fgets(cmd,MAXLINE,stdin);   
    command[0]=strtok(cmd," ");
    command[1] =strtok(NULL,"");
   
    if(!strcmp(command[0],"help\n")){
    printf("help : list available commands.\n");
    printf("name : print current sensor name.\n");
    printf("name <sensor> : changesensor name to <sensor>.\n");
    printf("value : print current value of sensor.\n");
    printf("value <n> : set sensor value to <n>.\n");
    printf("send : (current sensor name, time, value) to server.\n");
    printf("random <n> : send (name, time, random value) to server <n> times.\n");
    printf("quit : quit the program.\n");
    continue;
    }
    if(!strcmp(command[0],"exit"))
        return 0;
    //printf("cmd1 : %s \n cmd2 : %s\n",command[0],command[1]);
    if(!strcmp(command[0],"name\n")&&command[1]==NULL)
        printf("Current name is '%s'\n",myname);
    else if(!strcmp(command[0],"name"))
    {
	command[1][strlen(command[1])-1]='\0';
        sprintf(myname,"%s",command[1]);
	//command[1]마지막에 \r\n이 있어서제거
	
	
        printf("Current name is '%s'\n",myname);
    }

    else if(!strcmp(command[0],"value\n")&&command[1]==NULL)
        printf("Current value is '%f'\n",value);
    else if(!strcmp(command[0],"value"))
    {
	value = atof(command[1]);
        //strcpy(value,command[1]);
	
        printf("Current value is '%f'\n",value);
    }
    else if(!strcmp(command[0],"send\n")&&command[1]==NULL)
    {
        time_t t;
	time(&t);
	
	userTask(myname, hostname, port, filename, t, value);
	time_str = ctime( &t);
	time_str[strlen(time_str)-1] = '\0';
	printf("..sending name=%s&time=%svalue=%f\n",myname,time_str,value);
    }
    else if(!strcmp(command[0],"random"))
    {

      int count = atoi(command[1]);
	int i;
	for(i=0;i<count;i++){
        time_t t;
	time(&t);
	//time_str = ctime(&t);
	//time_str[strlen(time_str)-1] = '\0';
	float random_float = (((float)rand()/(float)(RAND_MAX)) *20) -10;
	
	value += random_float;
	
	userTask(myname, hostname, port, filename, t, value);
	time_str = ctime( &t);
	time_str[strlen(time_str)-1] = '\0';
	printf("..sending name=%s&time=%svalue=%f\n",myname,time_str,value);
	sleep(1);//1초간격
}
        
    }
    else{
        printf("잘못된 명령입니다\n");
        break;
    }
 
 
}
  return(0);
} 
