main : file_metafile.o main.o message.o btmap.o
	gcc -g file_metafile.o main.o btmap.o message.o -o main -lcrypto
main.o : main.c
	gcc -g -c main.c  
file_metafile.o : file_metafile.h file_metafile.c
	gcc -g -c file_metafile.c 
btmap.o : btmap.h btmap.c
	gcc -g -c btmap.c
message.o : message.h message.c
	gcc -g -c message.c

clean:
	rm -rf *.o main

