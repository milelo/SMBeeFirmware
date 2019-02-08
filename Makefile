DEVICE     = attiny10
CLOCK      = 500000
#PROGRAMMER = avrispmkII
PROGRAMMER = usbasp
PORT	   = usb
BAUD       = 9600
FILENAME   = src/main
#COMPILE    = avr-gcc -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)
COMPILE    = avr-gcc -Wall -Os -mmcu=$(DEVICE)

all: clean build upload

build:
	$(COMPILE) -S $(FILENAME).c -o $(FILENAME).s #generate asm 
	$(COMPILE) -c $(FILENAME).c -o $(FILENAME).o
	$(COMPILE) -o $(FILENAME).elf $(FILENAME).o
	avr-objcopy -j .text -j .data -O ihex $(FILENAME).elf $(FILENAME).hex
	avr-size --format=avr --mcu=$(DEVICE) $(FILENAME).elf

usb:
	ls /dev/cu.*
	
upload:
	#avrdude -v -p $(DEVICE) -c $(PROGRAMMER) -P $(PORT) -b $(BAUD) -U flash:w:$(FILENAME).hex:i 
	avrdude -v -p $(DEVICE) -c $(PROGRAMMER) -P $(PORT) -U flash:w:$(FILENAME).hex:i 

clean:
	rm -f src/main.o
	rm -f src/main.elf
	rm -f src/main.hex

rstdisbl:
	avrdude -v -p $(DEVICE) -c $(PROGRAMMER) -P $(PORT) -U fuse:w:0xfe:m

clrconfig:
	#can't just write 0xff:
	#need to follow: 16.4.3.4. Erasing the Configuration Section
	avrdude -v -p $(DEVICE) -c $(PROGRAMMER) -P $(PORT) -U fuse:w:0xff:m

