all: clean bitmap.o memory_manager.o create_lib

create_lib:
	ar -rs memory_manager.a bitmap.o memory_manager.o  

bitmap.o:
	gcc -c bitmap.c -o bitmap.o

memory_manager.o:
	gcc -c memory_manager.c -o memory_manager.o

test_main.o:
	gcc test_main.c bitmap.o memory_manager.o -o test_main.o

clean:
	rm -f test_main.o bitmap.o memory_manager.o

compress:
	tar -cvf proj1_ledward6.zip bitmap.c common.h interposition.h memory_manager.c memory_manager.h Makefile README

