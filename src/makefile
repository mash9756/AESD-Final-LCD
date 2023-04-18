
CC=gcc
RPIGCC=arm-linux-gnueabihf-gcc
FLAGS=-Wall -Werror
TARGET=LCD
LIBS=-L/home/admin/pigpio-master
HDRS=-I/home/admin/pigpio-master

all: default

default: main_o
	$(RPIGCC) $(FLAGS) main.o $(TARGET).o -o main $(LIBS) $(HDRS) -pthread -lpigpio -lrt

main_o:
	$(RPIGCC) $(FLAGS) -c $(TARGET).c -o $(TARGET).o $(LIBS) $(HDRS) -pthread -lpigpio -lrt
	$(RPIGCC) $(FLAGS) -c main.c -o main.o $(LIBS) $(HDRS) -pthread -lpigpio -lrt

host: main_host_o
	$(CC) $(FLAGS) main.o $(TARGET).o -o main -pthread -lpigpio -lrt

main_host_o:
	$(CC) $(FLAGS) -c $(TARGET).c -o $(TARGET).o -pthread -lpigpio -lrt
	$(CC) $(FLAGS) -c main.c -o main.o -pthread -lpigpio -lrt

clean:
	rm -f *.o $(TARGET) main
