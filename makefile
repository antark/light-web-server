CC = clang

ws:ws.c rio.o sock.o syn.o
	${CC} -o $@ $^ -pthread

rio.o:rio.c
	${CC} -o $@ -c rio.c

sock.o:sock.c
	${CC} -o $@ -c sock.c

syn.o:syn.c
	${CC} -o $@ -c syn.c

clean:
	rm -rf *.o