#include "stems.h"
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>
#include <mysql.h>

MYSQL mysql;
MYSQL_RES* res;
MYSQL_ROW row;
//
// This program is intended to help you test your web server.
// You can use it to test that you are correctly having multiple 
// threads handling http requests.
//
// htmlReturn() is used if client program is a general web client
// program like Google Chrome. textReturn() is used for a client
// program in a embedded system.
//
// Standalone test:
// # export QUERY_STRING="name=temperature&time=3003.2&value=33.0"
// # ./dataGet.cgi

void htmlReturn(void)
{
  
  char content[MAXLINE];
  char *buf;
  char *ptr;
  char *name;
  int num;
  char temp[1000];
  char temp2[1000];
  int readCount;
  char result[MAXLINE];
  time_t a;
  char * time_str;
  /* Make the response body */
  /*sprintf(content, "%s<html>\r\n<head>\r\n", content);
  sprintf(content, "%s<title>CGI test result</title>\r\n", content);
  sprintf(content, "%s</head>\r\n", content);
  sprintf(content, "%s<body>\r\n", content);
  sprintf(content, "%s<h2>Welcome to the CGI program</h2>\r\n", content);*/
  buf = getenv("QUERY_STRING");
  //sprintf(content,"%s<p>Env : %s</p>\r\n", content, buf);
  //ptr = strsep(&buf, "&");
  ptr = strtok(buf,"=");
  if(!strcmp(ptr,"NAME")){//NAME 
	ptr = strtok(NULL,"&");
	name = ptr;
	strtok(NULL,"=");
	ptr = strtok(NULL,"");
	num = atoi(ptr);
	sprintf(temp,"SELECT * FROM SensorList WHERE Name = '%s'",name);
	if(mysql_query(&mysql,temp)){
		printf("1:%s\n",mysql_error(&mysql));
		exit(1);
	}
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);
	readCount = atoi(row[2])-num+1;
	if(readCount<=0){
		sprintf(temp,"SELECT * FROM Sensor%d",atoi(row[1]));

		if(mysql_query(&mysql,temp)){
		printf("1:%s\n",mysql_error(&mysql));
		exit(1);
		}
		res = mysql_store_result(&mysql);
		sprintf(result,"Time\t\t\t\tIdx\tValue\n");
		while((row=mysql_fetch_row(res)))
		{
			a=atoi(row[0]);
		 	time_str = ctime(&a);
			time_str[strlen(time_str)-1] = '\0';
			sprintf(result,"%s%s\t%d\t%f\n",result,time_str,atoi(row[1]),atof(row[2]));
		}
		mysql_free_result(res);
	}else{
	sprintf(temp,"SELECT * FROM Sensor%d WHERE Idx >= %d",atoi(row[1]),readCount);
	mysql_free_result(res);
	if(mysql_query(&mysql,temp)){
		printf("1:%s\n",mysql_error(&mysql));
		exit(1);
	}
	res = mysql_store_result(&mysql);
	
	sprintf(result,"Time\t\t\t\tIdx\tValue\n");
	while((row=mysql_fetch_row(res)))
	{
		a=atoi(row[0]);
	 	time_str = ctime(&a);
		time_str[strlen(time_str)-1] = '\0';
		sprintf(result,"%s%s\t%d\t%f\n",result,time_str,atoi(row[1]),atof(row[2]));
	}
	mysql_free_result(res);
	}
}
 else{//command 

	ptr = strtok(NULL,"&");
	if(!strcmp(ptr,"LIST")){//LIST
		sprintf(temp,"SELECT * FROM SensorList");
		if(mysql_query(&mysql,temp)){
		printf("1:%s\n",mysql_error(&mysql));
		exit(1);
		}
		res = mysql_store_result(&mysql);
		row = mysql_fetch_row(res);
		sprintf(result,"%s\t",row[0]);
		while((row=mysql_fetch_row(res)))
		{
			sprintf(result,"%s%s\t",result,row[0]);

		}
		sprintf(result,"%s\n",result);
		mysql_free_result(res);
	}else{//INFO
		strtok(NULL,"=");
		name = strtok(NULL,"");
		sprintf(temp,"SELECT * FROM SensorList WHERE Name ='%s'",name);
		if(mysql_query(&mysql,temp)){
			printf("2:%s\n",mysql_error(&mysql));
			exit(1);
		}
		res = mysql_store_result(&mysql);
		row = mysql_fetch_row(res);
		
		sprintf(result,"count\taverage\n%d\t%f\n",atoi(row[2]),atof(row[3]));
		mysql_free_result(res);

	}
}
  
 sprintf(temp2,"HTTP/1.0 200 OK\r\n");
 sprintf(temp2,"%sContent-Length: %d\r\n",temp2,strlen(result));
 sprintf(temp2,"%sContent-Type: text/html\n\r\n",temp2);
 sprintf(temp2,"%s%s",temp2,result);
 printf("%s",temp2);
 
  fflush(stdout);
}

void textReturn(void)
{
  char content[MAXLINE];
  char *buf;
  char *ptr;

  buf = getenv("QUERY_STRING");
  sprintf(content,"%sEnv : %s\n", content, buf);
  ptr = strsep(&buf, "&");
  while (ptr != NULL){
    sprintf(content, "%s%s\n", content, ptr);
    ptr = strsep(&buf, "&");
  }
  
  /* Generate the HTTP response */
  printf("Content-Length: %d\n", strlen(content));
  printf("Content-Type: text/plain\r\n\r\n");
  printf("%s", content);
  fflush(stdout);
}

int main(void)
{
  mysql_init(&mysql);
  if(!mysql_real_connect(&mysql,NULL,NULL,"p@ssw0rd","OpenSource",3306,(char *)NULL,0)){
	printf("%s\n",mysql_error(&mysql));
		exit(1);
	}
  htmlReturn();
  //textReturn();
  mysql_free_result(res);
  mysql_close(&mysql);
  return(0);
}
