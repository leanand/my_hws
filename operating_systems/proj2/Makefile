obj-m += process_list.o

all:	compile compile_user_code 

insert_module: process_list.ko
	sudo insmod process_list.ko
	sudo chmod 664 /dev/process_list

remove_module:
	sudo rmmod process_list.ko

compile:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

compile_user_code:
	gcc process_list_user.c -o process_list_user.out

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -rf process_list_user.out