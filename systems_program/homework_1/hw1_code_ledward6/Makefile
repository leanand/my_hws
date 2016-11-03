all: hw1_myprintf hw1_str create_lib

hw1_str:
	gcc -c hw1_str.c -o hw1_str.o

hw1_myprintf:
	gcc -c hw1_myprintf.c -o hw1_myprintf.o

create_lib:
	ar -rs hw1.a hw1_str.o hw1_myprintf.o

clean:
	rm -f hw1_str.o hw1_myprintf.o hw1.a hw1_code_ledward6.zip

compress:
	tar cvf hw1_code_ledward6.zip hw1.h hw1_str.c hw1_myprintf.c Makefile