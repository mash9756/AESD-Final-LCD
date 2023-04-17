
CC=gcc
RPIGCC=/home/admin/Downloads/cross-pi-gcc-10.2.0-0/bin/arm-linux-gnueabihf-gcc
FLAGS=-Wall -Werror
TARGET=LCD

all: default

default: main_o
	$(RPIGCC) $(FLAGS) main.o LCD.o -o main -I/home/admin/Documents/Final-Project/pigpio/ -L/usr/local/lib -pthread -lpigpio -lrt

main_o:
	$(RPIGCC) $(FLAGS) -c $(TARGET).c -o $(TARGET).o -I/home/admin/Documents/Final-Project/pigpio/ -L/usr/local/lib -pthread -lpigpio -lrt
	$(RPIGCC) $(FLAGS) -c main.c -o main.o -I/home/admin/Documents/Final-Project/pigpio/ -L/usr/local/lib -pthread -lpigpio -lrt

host: main_host_o
	$(CC) $(FLAGS) main.o LCD.o -o main -pthread -lpigpio -lrt

main_host_o:
	$(CC) $(FLAGS) -c $(TARGET).c -o $(TARGET).o -pthread -lpigpio -lrt
	$(CC) $(FLAGS) -c main.c -o main.o -pthread -lpigpio -lrt

clean:
	rm -f *.o $(TARGET) main
