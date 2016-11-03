#include<stdio.h>
#include<stdarg.h>

int myprintf(char* format, ...){
  char *s, c;
  int d;
  int printf_return;
  va_list printf_args;
  va_start(printf_args, format);
  printf_return = vprintf(format, printf_args);
  va_end(printf_args);
  printf("\nArgument list:\n");
  va_start(printf_args, format);
  while(*format){
    if(*format == '%'){
      format ++;
      switch(*format){
        case 's':
          s = va_arg(printf_args, char*);
          printf("String --> %s\n", s);
          break;
        case 'c':
          c = va_arg(printf_args, int);
          printf("Char --> %c\n", c);
          break;
        case 'd':
          d = va_arg(printf_args, int);
          printf("Integer --> %d\n", d);
          break;
      }
    }
    format ++;
  }
  va_end(printf_args);
  return printf_return;
}
