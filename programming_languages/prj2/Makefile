#Targets:
#  Default target: compile project
#  clean:          remove all generated files.
#  submit:         build compressed archive with all project source files.

PROJECT = 	prj2

SRC_FILES = \
  fns.rkt \
  ugly-regexp/errors.rkt \
  ugly-regexp/parser.rkt \
  ugly-regexp/scanner.rkt \
  Makefile \
  README

build:
		echo "yay! no explicit compile and nothing to build."

clean:		
		rm -rf $(PROJECT).tar.gz


submit:
		tar -cvzf $(PROJECT).tar.gz $(SRC_FILES)
