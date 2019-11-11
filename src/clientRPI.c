#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#define MAXTIMINGS 83
#define DHTPIN 7

int dht11_dat[5] = {0, 0, 0, 0, 0};
char hostname[MAXLINE], filename[MAXLINE];
int second;
int temp1[1000];
int temp2[1000];

void clientSend(int fd, char *filename, char *body)
{
  char buf[MAXLINE];
  char hostname[MAXLINE];
 
  Gethostname(hostname, MAXLINE);
 
  /* Form and send the HTTP request */
  sprintf(buf, "POST %s HTTP/1.1\n", filename);
  sprintf(buf, "%sHost: %s\n", buf, hostname);
  sprintf(buf, "%sContent-Type: text/plain; charset=utf-8\n", buf);
  sprintf(buf, "%sContent-Length: %d\n\r\n", buf, strlen(body));
  sprintf(buf, "%s%s\n", buf, body);
  Rio_writen(fd, buf, strlen(buf));
}

void userTask(char *myname, char *hostname, int port, char *filename, int a, char* temp)
{


  int clientfd;
  char msg[MAXLINE];
   
  sprintf(msg, "name=%s&time=%d&value=%f", myname, a, temp);
  clientfd = Open_clientfd(hostname, port);
  clientSend(clientfd, filename, msg);
  clientPrint(clientfd);
  Close(clientfd);
}

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
    if (sscanf(buf, "Content-Length: %d ", &length) == 1)
      printf("Length = %d\n", length);
    printf("Header: %s", buf);
    n = Rio_readlineb(&rio, buf, MAXBUF);
  }
 
  /* Read and display the HTTP Body */
  n = Rio_readlineb(&rio, buf, MAXBUF);
  while (n > 0) {
    printf("%s", buf);
    n = Rio_readlineb(&rio, buf, MAXBUF);
  }
}

void getargs_cp(char *hostname, int *port, char *filename, int *second)
{
  FILE *fp;
 
  fp = fopen("config-pi.txt", "r");
  if (fp == NULL)
    unix_error("config-pi.txt file does not open.");
 
 
  fscanf(fp, "%s", hostname);
  fscanf(fp, "%d", port);
  fscanf(fp, "%s", filename);
  fscanf(fp, "%f", second);
  fclose(fp);
}

void read_dht11_dat()
{
	uint8_t laststate = HIGH;
	uint8_t counter = 0;
	uint8_t j = 0, i;
	uint8_t flag = HIGH;
	uint8_t state = 0;
	float f;
	memset(dht11_dat, 0, sizeof(int) * 5);
	pinMode(DHTPIN, OUTPUT);
	digitalWrite(DHTPIN, LOW);
	delay(18);
	digitalWrite(DHTPIN, HIGH);
	delayMicroseconds(30);
	pinMode(DHTPIN, INPUT);
	for (i = 0; i < MAXTIMINGS; i++) {
		counter = 0;
		while (digitalRead(DHTPIN) == laststate) {
			counter++;
			delayMicroseconds(1);
			if (counter == 200) break;
		}
		laststate = digitalRead(DHTPIN);
		if (counter == 200) break; // if while breaked by timer, break for
		if ((i >= 4) && (i % 2 == 0)) {
			dht11_dat[j / 8] <<= 1;
		if (counter > 20) dht11_dat[j / 8] |= 1;
		j++;
		}
	}
	if ((j >= 40) && (dht11_dat[4] == ((dht11_dat[0] + dht11_dat[1] + dht11_dat[2] +
	dht11_dat[3]) & 0xff))) {
		printf("humidity = %d.%d %% Temperature = %d.%d *C \n", dht11_dat[0],
		dht11_dat[1], dht11_dat[2], dht11_dat[3]);
//출력
	sprintf(temp1,"%d.%d",dht11_dat[0],dht11_dat[1]);
	sprintf(temp2,"%d.%d",dht11_dat[2],dht11_dat[3]);
	time_t t;
	time(&t);
	userTask("humidity", hostname, port, filename, t, temp1);
//1초 기다림
	delay(sencond);
	userTask("temperature", hostname, port, filename, t, temp2);	
	}
	else printf("Data get failed\n");
} 
int main(void)
{
	
	getargs_cp(hostname, &port, filename, &second);
	second =*1000/2;
	printf("dht11 Raspberry pi\n");
	if (wiringPiSetup() == -1) exit(1);
	while (1) {
	read_dht11_dat();
	delay(second);
	}
	return 0;
}
