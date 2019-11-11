#include "stems.h"
#include "request.h"
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>

// 
// To run:
// 1. Edit config-ws.txt with following contents
//    <port number>
// 2. Run by typing executable file
//    ./server
// Most of the work is done within routines written in request.c
//
struct timeval startTime;
typedef struct Data1{
	int fd;
	double time;
}Data;
typedef struct Queue1{
	int front;
	int rear;
	Data *socket_data;
}Queue;

Queue q;
sem_t ok;
sem_t full;
sem_t empty;
int thread_size,queue_size;

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


void queue_init()
{
	q.front = 0;
	q.rear = 0;
	q.socket_data = (Data*)malloc(sizeof(Data)*queue_size);
}
void queue_insert(Data data)
{
	if((q.rear+1)%queue_size ==q.front){
		printf(" 큐가 꽉찼음\n");
		exit(-1);
	}
	q.rear  = (q.rear+1)%queue_size ;
	q.socket_data[q.rear] = data;
}
Data queue_delete()
{
	
	if(q.rear ==q.front){
		printf(" 큐가 비어있음\n");
		exit(-1);
	}
	q.front= (q.front+1)%queue_size;
	return q.socket_data[q.front];
}
void getargs_ws(int *port,int *thread_size,int *queue_size)
{
  FILE *fp;

  if ((fp = fopen("config-ws.txt", "r")) == NULL)
    unix_error("config-ws.txt file does not open.");

  fscanf(fp, "%d", port);
  fscanf(fp, "%d", thread_size);
  fscanf(fp, "%d", queue_size);
  fclose(fp);
}

void consumer(int connfd, double arrivalTime)
{
	
  requestHandle(connfd, arrivalTime);
  Close(connfd);
}

void *Worker_thread()
{
	Data temp;
while(1)
{
	sem_wait(&empty);
	sem_wait(&ok);
	temp = queue_delete();
	sem_post(&ok);
	
	consumer(temp.fd, temp.time);

	char *method =getenv("REQUEST_METHOD");
	if(!strcmp(method,"POST")){
		pid_t pid= fork();
		if(pid == 0){
			Execve("./alarmclient",(NULL),(NULL));
		}
	}

	sem_post(&full);
}
}


int main(void)
{//변수
  int listenfd, connfd, port, clientlen;
  
  int i;
  Data temp;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_t *threads;
  struct sockaddr_in clientaddr;
  
  //초기화
  getargs_ws(&port,&thread_size,&queue_size);
  sem_init(&full,0,queue_size);
  sem_init(&empty,0,0);
  sem_init(&ok,0,1);
  
  initWatch();
  queue_init(queue_size);
  threads = (pthread_t*)malloc(sizeof(pthread_t)*thread_size);
  for(i =0;i<thread_size;i++){
	pthread_create(&threads[i],&attr,Worker_thread,NULL);
	pthread_detach(&threads[i]);
}//


listenfd = Open_listenfd(port);
  while (1) {
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
    temp.fd = connfd;
    temp.time = getWatch();
    sem_wait(&full);
    sem_wait(&ok);
    queue_insert(temp);
    sem_post(&ok);
    sem_post(&empty);
    
  }

  return(0);
}
