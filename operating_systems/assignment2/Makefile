obj-m += test.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc utest.c -o ./utest

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
