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


MYSQL mysql;
MYSQL_RES* res;
MYSQL_ROW row;
void Dbinit(void)
{

	mysql_query(&mysql,"CREATE DATABASE OpenSource");

	mysql_query(&mysql,"USE OpenSource");

	mysql_query(&mysql,"CREATE TABLE SensorList(Name TEXT,Id INT not null auto_increment,Cnt INT,Ave FLOAT,PRIMARY KEY(Id) )");

}
int DbgetId(char *name)
{
	int id ;
	mysql_query(&mysql,"SELECT * FROM SensorList");
	res = mysql_store_result(&mysql);
	
	while((row=mysql_fetch_row(res)))
	{
		if(!strcmp(name,row[0])){
		id = atoi(row[1]);	
		}
	}
mysql_free_result(res);
return id;
}
float Dbgetavg(int id)
{
	char temp4[1000];
	float ret;
	int count =0;
	sprintf(temp4,"SELECT * FROM Sensor%d",id);
	mysql_query(&mysql,temp4);
	res = mysql_store_result(&mysql);
	while((row=mysql_fetch_row(res)))
	{
		if(!strcmp(name,row[0])){
		id = atoi(row[1]);	
		}
		ret += atof(row[2]);
		count++;
	}
mysql_free_result(res);
	
return (float)ret/count;
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
int main(int argc, char *argv[])
{
	
  int a;//환경변수 REST_CONTESTS의 길이
 char *temp;//8192 이상의 데이터를 받았을때도 dup2를 통해 제대로 받았는지 확인할 변
  char temp2[MAXBUF];
  char temp4[MAXBUF];
  rio_t rio;

  char temp3[1000];

  int b;//헤더의 CONTENT_LENGTH
  //char *astr = "Currently, CGI program is running, but argument passing is not implemented.";
  char *s=getenv("CONTENT_LENGTH");
  char *rest = getenv("REST_CONTENTS");

    int filedes;
    char size[MAXBUF];

  strcpy(temp2,rest);
  rio_readinitb(&rio,STDIN_FILENO);
  a = strlen(temp2);//길이
  b=atoi(s);//requestbody내
  if(a-1<b){

 	int k=b-a;//읽어와야할 바이트의 수 
	rio_readnb(&rio,temp2,k);//차이나는 바이트만큼 temp2에 저장
	sprintf(temp2,"%s%s",temp2,temp);
   }

sprintf(size,"%d",strlen(temp2));


    if((filedes = open("./fifo", O_WRONLY)) < 0){

        printf("fail to call open()\n");

        exit(1);

    }

       if(write(filedes, size, MAXBUF)==-1){

            printf("fail to call write()\n");

            exit(1);

        }

       if(write(filedes, temp2,strlen(temp2) )==-1){

            printf("fail to call write()\n");
            exit(1);

        }

	close(filedes);
	parse(temp2);//분리
  
  	mysql_init(&mysql);
  	if(!mysql_real_connect(&mysql,NULL,NULL,"p@ssw0rd",NULL,3306,(char *)NULL,0)){
	printf("%s\n",mysql_error(&mysql));
	exit(1);
	}
  
	Dbinit();
  
	sprintf(temp3,"SELECT * FROM SensorList WHERE Name = '%s'",nameval);
	if(mysql_query(&mysql,temp3)){
		printf("10:%s\n",mysql_error(&mysql));
		exit(1);
	}
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);
	if(row ==NULL)//없음
	{//List 와 sensor(num) 둘다 추가해주어야함 
		sprintf(temp3,"INSERT INTO SensorList VALUES('%s',NULL,1,%f)",nameval,atof(valueval));
		if(mysql_query(&mysql,temp3)){
		printf("12:%s\n",mysql_error(&mysql));
		exit(1);
		}
		int id = DbgetId(nameval);
		sprintf(temp3,"CREATE TABLE Sensor%d(Time TEXT,Idx INT not null auto_increment,Value FLOAT,PRIMARY KEY(Idx))",id);
		if(mysql_query(&mysql,temp3)){
		printf("13:%s\n",mysql_error(&mysql));
		exit(1);
		}
		sprintf(temp3,"INSERT INTO Sensor%d VALUES('%s',NULL,%f)",id,timeval,atof(valueval));
		if(mysql_query(&mysql,temp3)){
		printf("14:%s\n",mysql_error(&mysql));
		exit(1);
		}
	}else{//있음 sensor(num)에 먼저 추가해준뒤 해당 valuer값들의 평균을 list에 업데이트 
		int id = DbgetId(nameval);
		int idx;
		float avg;
		sprintf(temp3,"INSERT INTO Sensor%d VALUES('%s',NULL,%f)",id,timeval,atof(valueval));
		if(mysql_query(&mysql,temp3)){
		printf("15:%s\n",mysql_error(&mysql));
		exit(1);
		}
		idx = mysql_insert_id(&mysql);
		avg = Dbgetavg(id);
		sprintf(temp3,"UPDATE SensorList SET Cnt = '%d', Ave='%f' WHERE Id='%d' ",idx,avg,id);
		if(mysql_query(&mysql,temp3)){
		printf("16:%s\n",mysql_error(&mysql));
		exit(1);
		}
	}



  printf("HTTP/1.0 200 OK\r\n");
  printf("Server: My Web Server\r\n");
  printf("Content-Length: %d\r\n", a);
  printf("Content-Type: text/plain\n\r\n");

mysql_free_result(res);
mysql_close(&mysql);
 
  fflush(stdout);



  return(0);
}
