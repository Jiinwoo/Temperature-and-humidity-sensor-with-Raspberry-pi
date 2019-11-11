#include "stems.h"
#include "request.h"

struct timeval startTime;
void getargs_ws(int *port)
{
  FILE *fp;

  if ((fp = fopen("config-ps.txt", "r")) == NULL)
    unix_error("config-ws.txt file does not open.");

  fscanf(fp, "%d", port);
  fclose(fp);
}

void initWatch(void)
{
  gettimeofday(&startTime, NULL);
}
 
double getWatch(void)
{
  struct timeval curTime;
  double tmp;
 
  gettimeofday(&curTime, NULL);
  tmp = (curTime.tv_sec - startTime.tv_sec) * 1000.0;
  return (tmp - (curTime.tv_usec - startTime.tv_usec) / 1000.0);
}

void consumer(int connfd, long arrivalTime)
{
  requestHandle(connfd, arrivalTime);
  Close(connfd);
}


int main(void)
{
  int listenfd, connfd, port, clientlen;
  struct sockaddr_in clientaddr;
  initWatch();
  getargs_ws(&port);

  listenfd = Open_listenfd(port);
  while (1) {
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
    consumer(connfd, getWatch());
  }

  return(0);

}
