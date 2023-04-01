
CC=gcc
FLAGS=-Wall -Werror
TARGET=LCD

all: default

default: main_o
	$(CC) $(FLAGS) main.o LCD.o -o main -l wiringPi

main_o:
	$(CC) $(FLAGS) -c $(TARGET).c -o $(TARGET).o -l wiringPi
	$(CC) $(FLAGS) -c main.c -o main.o -l wiringPi

clean:
	rm -f *.o $(TARGET) main