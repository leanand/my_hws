all: clean bitmap.o memory_manager.o test_main.o
  
bitmap.o:
	gcc -c bitmap.c -o bitmap.o

memory_manager.o:
	gcc -c memory_manager.c -o memory_manager.o

test_main.o:
	gcc test_main.c bitmap.o memory_manager.o -o test_main.o

clean:
	rm -f test_main.o bitmap.o memory_manager.o
