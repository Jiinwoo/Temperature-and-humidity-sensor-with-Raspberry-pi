//
// request.c: Does the bulk of the work for the web server.
// 
 
#include "stems.h"
#include "request.h"
#include <stdlib.h>
 
/*  time related functions */

char ptr3[]=".";

 
/* web request process routines */
 
void requestError(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg) 
{
  char buf[MAXLINE], body[MAXBUF];
 
  printf("Request ERROR\n");
 
  // Create the body of the error message
  sprintf(body, "<html><title>CS537 Error</title>");
  sprintf(body, "%s<body bgcolor=""fffff"">\r\n", body);
  sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
  sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
  sprintf(body, "%s<hr>CS537 Web Server\r\n", body);
 
  // Write out the header information for this response
  sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
  Rio_writen(fd, buf, strlen(buf));
  printf("%s", buf);
 
  sprintf(buf, "Content-Type: text/html\r\n");
  Rio_writen(fd, buf, strlen(buf));
  printf("%s", buf);
 
  sprintf(buf, "Content-Length: %lu\r\n\r\n", (long unsigned int)strlen(body));
  Rio_writen(fd, buf, strlen(buf));
  printf("%s", buf);
 
  // Write out the content
  Rio_writen(fd, body, strlen(body));
  printf("%s", body);
 
}
 
//
// Reads and discards everything up to an empty text line
// If it encounters 'Content-Length:', it read the value for later use
//
void requestReadhdrs(rio_t *rp, int *length)
{
  char buf[MAXLINE];
  char *s;
  char temp[10];
  *length = -1;
  Rio_readlineb(rp, buf, MAXLINE);
  while (strcmp(buf, "\r\n")) {
    sscanf(buf, "Content-Length: %d\n", length);
    Rio_readlineb(rp, buf, MAXLINE);
  }
  sprintf(temp,"%d",*length);
  Setenv("CONTENT_LENGTH",temp,1);
  s= getenv("CONTENT_LENGTH");
  //printf("bodyLength's env : %s\n",s);
	
  return;
}
 
//
// Return STATIC if static, DYNAMIC if dynamic content
// Input: uri
// output: filename
//         cgiargs (in the case of dynamic)
//
int parseURI(char *uri, char *filename, char *cgiargs) 
{
	char *s;//GET,POST 문자열 저장
	char *p;//GET 일때 QUERY_STRING 문자열 저장
	char *temp;
  if (!strstr(uri, "cgi")) {
    // static
    strcpy(cgiargs, "");
    sprintf(filename, ".%s", uri);
    if (uri[strlen(uri)-1] == '/') {
      strcat(filename, "index.html");
    }
    return STATIC;
  } else {
    char *ptr1 = strtok(uri,"?");
    sprintf(filename, ".%s", ptr1);
    cgiargs = strtok(NULL,"?"); //수정한 부분
    s= getenv("REQUEST_METHOD");
	if(strcmp(s,"GET")==0){
	Setenv("QUERY_STRING",(const char*)cgiargs,1);
	p= getenv("QUERY_STRING");
	}
    return DYNAMIC;
  }
}
 
//
// Fills in the filetype given the filename
//
void requestGetFiletype(char *filename, char *filetype)
{
  if (strstr(filename, ".html")) 
    strcpy(filetype, "text/html");
  else if (strstr(filename, ".gif")) 
    strcpy(filetype, "image/gif");
  else if (strstr(filename, ".jpg")) 
    strcpy(filetype, "image/jpeg");
  else 
    strcpy(filetype, "test/plain");
}
 
void requestServeDynamic(rio_t *rio, int fd, char *filename, char *cgiargs, int bodyLength, double arrivalTime)
{
  //
  // Followings are dummy. After they should be replaced with dynamic
  // request implementation.
  //
  char buf2[MAXLINE];
  char astr[MAXLINE] = "Current version does not support CGI program.";
char *s2 ;
 
  if (bodyLength > 0){
    Rio_readrestb(rio, cgiargs);
    Setenv("REST_CONTENTS",(const char *)cgiargs,1);
    s2= getenv("REST_CONTENTS");
	
  }
  if(0==Fork())
  {
	Dup2(fd,STDOUT_FILENO);
	Dup2(fd,STDIN_FILENO);

	Execve(filename,NULL,environ);
  }
}
 
 
void requestServeStatic(int fd, char *filename, int filesize, double arrivalTime)
{
  int srcfd;
  char *srcp, filetype[MAXLINE], buf[MAXBUF];
 
  // Rather than call read() to read the file into memory, 
  // which would require that we allocate a buffer, we memory-map the file
  requestGetFiletype(filename, filetype);
  srcfd = Open(filename, O_RDONLY, 0);
  srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
  Close(srcfd);
 
  // put together response
  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  sprintf(buf, "%sServer: My Web Server\r\n", buf);
 
  // Your statistics go here -- fill in the 0's with something useful!
  sprintf(buf, "%sStat-req-arrival: %lf\r\n", buf, arrivalTime);
  // Add additional statistic information here like above
  // ...
  //
  
  sprintf(buf, "%sContent-Length: %d\r\n", buf, filesize);
  sprintf(buf, "%sContent-Type: %s\r\n\r\n", buf, filetype);
 
  Rio_writen(fd, buf, strlen(buf));
 
  // Writes out to the client socket the memory-mapped file 
  Rio_writen(fd, srcp, filesize);
  Munmap(srcp, filesize);
}
 
// handle a request
void requestHandle(int connfd, double arrivalTime)
{
  char *s;//환경변수출력 
  char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
  rio_t rio;
  int reqType;
  char filename[MAXLINE], cgiargs[MAXLINE];
  struct stat sbuf;
  int bodyLength;
 
  Rio_readinitb(&rio, connfd);
  Rio_readlineb(&rio, buf, MAXLINE);
  sscanf(buf, "%s %s %s", method, uri, version);
  
  if(strcasecmp(method,"GET")==0){//METHOD환경변수 정의 
	Setenv("REQUEST_METHOD","GET",1);
  }
else{
	Setenv("REQUEST_METHOD","POST",1);
   }
	s= getenv("REQUEST_METHOD");
	//printf("REQUEST_METHOD's env : %s\n",s);

  requestReadhdrs(&rio, &bodyLength);
	
  reqType = parseURI(uri, filename, cgiargs);//
  if ((strcasecmp(method, "GET")!=0)&&(strcasecmp(method,"POST")!=0)){
    requestError(connfd, method, "501", "Not Implemented",
         "My Server does not implement this method");
    return;
  }
  
  if (stat(filename, &sbuf) < 0) {
    Rio_readrestb(&rio, buf);
    requestError(connfd, filename, "404", "Not found", "My Server could not find this file");
    return;
  }
 
  if (reqType == STATIC) {
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
      requestError(connfd, filename, "403", "Forbidden", "My Server could not read this file");
 
      return;
    }
    requestServeStatic(connfd, filename, sbuf.st_size, arrivalTime);
  } else {
     
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) {
      requestError(connfd, filename, "403", "Forbidden", "My Server could not run this CGI program");
      return;
    }
    requestServeDynamic(&rio, connfd, filename, cgiargs, bodyLength, arrivalTime);
  }
}
