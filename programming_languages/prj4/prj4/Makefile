#Targets:
#  Default target: compile project
#  clean:          remove all generated files.
#  submit:         build compressed archive with all project source files.

PROJECT = 	prj4

SRC_FILES = \
  prj4-sol.pl \
  Makefile \
  README

build:
		echo "yay! no explicit compile and nothing to build."

clean:		
		rm -rf $(PROJECT).tar.gz


submit:         $(SRC_FILES)
		tar -cvzf $(PROJECT).tar.gz $(SRC_FILES)
