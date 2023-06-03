.PHONY: clean

server: image_server.o
	gcc -o server image_server.o -lpthread
image_server.o: image_server.c image_server.h
	gcc -c -o image_server.o image_server.c
clean:
	        rm -f *.o

