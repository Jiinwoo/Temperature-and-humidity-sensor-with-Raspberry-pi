#
# To compile, type "make" or make "all"
# To remove files, type "make clean"
#
OBJS = server.o request.o stems.o clientGet.o clientPost.o alarmclient.o alarmserver.o 
TARGET = server

CC = gcc -I/usr/include/mysql -L /usr/local/lib/x86_64-linux-gnu -l mysqlclient
CFLAGS = -g -Wall 


LIBS = -lpthread 

.SUFFIXES: .c .o 

all: server clientPost clientGet dataGet.cgi dataPost.cgi alarmclient alarmserver alarm.cgi

server: server.o request.o stems.o
	$(CC) $(CFLAGS) -o server server.o request.o stems.o $(LIBS) -I/usr/include/mysql -L /usr/local/lib/x86_64-linux-gnu -l mysqlclient

clientGet: clientGet.o stems.o
	$(CC) $(CFLAGS) -o clientGet clientGet.o stems.o

clientPost: clientPost.o stems.o
	$(CC) $(CFLAGS) -o clientPost clientPost.o stems.o $(LIBS)

dataGet.cgi: dataGet.o stems.o
	$(CC) $(CFLAGS) -o dataGet.cgi dataGet.o stems.o -I/usr/include/mysql -L /usr/local/lib/x86_64-linux-gnu -l mysqlclient

dataPost.cgi: dataPost.o stems.o
	$(CC) $(CFLAGS)  -o dataPost.cgi dataPost.o stems.o -I/usr/include/mysql -L /usr/local/lib/x86_64-linux-gnu -l mysqlclient
alarm.cgi: alarm.o stems.o
	$(CC) $(CFLAGS)  -o alarm.cgi alarm.o stems.o

alarmclient: alarmclient.o stems.o
	$(CC) $(CFLAGS) -o alarmclient alarmclient.o stems.o $(LIBS)
alarmserver: alarmserver.o stems.o request.o
	$(CC) $(CFLAGS) -o alarmserver alarmserver.o stems.o request.o $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -o $@  -c $< 

server.o: stems.h 
clientGet.o: stems.h
clientPost.o: stems.h request.h
alarmclient.o: stems.h request.h
alarmserver.o: stems.h request.h 

clean:
	-rm -f $(OBJS) server clientPost clientGet dataGet.cgi dataPost.cgi
