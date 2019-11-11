#include "stems.h"
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>
#include <mysql.h>
#include <errno.h>

char *name;
char *nameval;
char *time1;
char *timeval;
char *value;
char *valueval;

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

int main(int argc, char *argv[])
{
	
  int a;//환경변수 REST_CONTESTS의 길이
 char *temp;//8192 이상의 데이터를 받았을때도 dup2를 통해 제대로 받았는지 확인할 변
  char temp2[MAXBUF];
  rio_t rio;
  char temp3[1000];
  int b;//헤더의 CONTENT_LENGTH
  char *s=getenv("CONTENT_LENGTH");
  char *rest = getenv("REST_CONTENTS");
  time_t z ;
  strcpy(temp2,rest);
  rio_readinitb(&rio,STDIN_FILENO);
  a = strlen(temp2);//길이
  b=atoi(s);//requestbody내

  if(a-1<b){
	
 	int k=b-a;//읽어와야할 바이트의 수 
	rio_readnb(&rio,temp2,k);//차이나는 바이트만큼 temp2에 저장
	sprintf(temp2,"%s%s",temp2,temp);
   }


parse(temp2);//분리
z = atoi(timeval);
fprintf(stderr,"\r\n경고 : %s sensor로부터 %s 시각에 %f 라는 값이 발생했습니다\n",nameval,ctime(&z),atof(valueval));

  printf("HTTP/1.0 200 OK\r\n");
  printf("Server: My Alarm Server\r\n");
  printf("Content-Length: %d\r\n", a);
  printf("Content-Type: text/plain\n\r\n");

  fflush(stdout);

  return(0);
}
