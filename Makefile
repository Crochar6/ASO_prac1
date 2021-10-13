obj-m+=main.o
	
all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
install:
	sudo insmod main.ko
	rm -rf /home/pi/Documents/CustomScripts
	mkdir /home/pi/Documents/CustomScripts
	touch /home/pi/Documents/CustomScripts/button1.sh
	touch /home/pi/Documents/CustomScripts/button2.sh
	touch /home/pi/Documents/CustomScripts/button3.sh
	touch /home/pi/Documents/CustomScripts/button4.sh
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean