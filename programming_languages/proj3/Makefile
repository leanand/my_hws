#Targets:
#  Default target: compile project
#  clean:          remove all generated files.
#  submit:         build compressed archive with all project source files.

PROJECT = 	prj3

SRC_FILES = \
  prj3-sol.hs \
  Makefile \
  README

build:
		echo "yay! no explicit compile and nothing to build."

clean:		
		rm -rf $(PROJECT).tar.gz


submit:         $(SRC_FILES)
		tar -cvzf $(PROJECT).tar.gz $(SRC_FILES)
