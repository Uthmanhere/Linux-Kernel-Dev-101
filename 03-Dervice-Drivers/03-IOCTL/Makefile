CC=gcc


obj-m := module.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	$(CC) app.c -o app

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	$(RM) app
