DIROBJ := obj/
DIREXE := exec/
DIRHEA := include/
DIRSRC := src/

CFLAGS := -I$(DIRHEA) -c -Wall -std=c99
LDLIBS := -pthread -lrt -lm
CC := gcc

all : dirs manager barber client

dirs:
	mkdir -p $(DIROBJ) $(DIREXE)

manager: $(DIROBJ)manager.o  $(DIROBJ)semaphoreI.o
	$(CC) -lm -o $(DIREXE)$@ $^ $(LDLIBS)

barber: $(DIROBJ)barber.o  $(DIROBJ)semaphoreI.o
	$(CC) -o $(DIREXE)$@ $^ $(LDLIBS)

client: $(DIROBJ)client.o  $(DIROBJ)semaphoreI.o
	$(CC) -o $(DIREXE)$@ $^ $(LDLIBS)

$(DIROBJ)%.o: $(DIRSRC)%.c
	$(CC) $(CFLAGS) $^ -o $@

test:
	./exec/manager 5 3

solution:
	./exec/manager 15 5

clean : 
	rm -rf *~ core $(DIROBJ) $(DIREXE) $(DIRHEA)*~ $(DIRSRC)*~