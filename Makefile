CC=g++

OBJS= main.o  SIMPLESOCKET.o
DEMOTARGET=main server client

client.o:	client.C
	$(CC) -c $<  -std=c++11

server.o:	server.C
	$(CC) -c $<  -std=c++11
	

SIMPLESOCKET.o:	SIMPLESOCKET.C
	$(CC) -c $<  -std=c++11


main.o:	main.C
	$(CC) -c $<  -std=c++11	



main:	$(OBJS)
	$(CC) -o $@ $^ -L/usr/lib/x86_64-linux-gnu -lpigpio -lpthread -ldl -lstdc++  -std=c++11


server:	server.o
	$(CC) -o server server.o  SIMPLESOCKET.o -L/usr/lib/x86_64-linux-gnu -lpigpio -lpthread -ldl -lstdc++  -std=c++11
	

client:	client.o
	$(CC) -o client client.o SIMPLESOCKET.o -L/usr/lib/x86_64-linux-gnu -lncurses -lpthread -ldl -lstdc++  -std=c++11

clean:
	-rm -r -f   $(DEMOTARGET) *.o DOXYGENDOC  *.txt

doc:
	doxygen Doxyfile 


all:	$(DEMOTARGET)
	make clean  && make main && make server && make client

run:	main	
	./main
