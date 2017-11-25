all: clean hex program

reset:
	avrdude -c arduino -P /dev/ttyUSB0 -b 19200 -p atmega168 -F

program: main.hex
	avrdude -c arduino -P /dev/ttyUSB0 -b 19200 -p atmega168 -F -U main.hex

hex: main.o
	avr-objcopy -O ihex main.o main.hex

main.o:
	avr-gcc -g -mmcu=atmega168 -o main.o main.c

clean:
	rm *.hex *.o
