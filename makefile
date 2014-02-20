ws:ws.c rio.o sock.o syn.o
	gcc -o ws ws.c rio.o sock.o syn.o -pthread
rio.o:rio.c
	gcc -c rio.c
sock.o:sock.c
	gcc -c sock.c
syn.o:syn.c
	gcc -c syn.c
clean:
	rm *.o
