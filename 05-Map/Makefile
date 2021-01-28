CC=gcc


obj-m := copy_from_user_or_not_module.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	$(CC) app.c -o app

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	$(RM) app
