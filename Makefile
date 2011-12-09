# create *.o:  $ make
# create xxx.exe: $ make TARGET=xxx

SRC = $(shell ls *.cpp *.c)
OBJ0 = $(SRC:.c=.o)
OBJ  = $(OBJ0:.cpp=.o)

CC = g++
ifeq ($(OS),Windows_NT)			# Windows
	CFLAGS = -Wall -Wextra -O3
	EXE_LIST =  $(OBJ:.o=.exe)
#   LOADLIBES = $(shell ls *.o | grep -v $@) -lregex
	LOADLIBES = $(filter-out $(TARGET).o, $(OBJ)) -lregex -lxml2 -lws2_32
else							# Linux
	CFLAGS = -Wall -Wextra -O3 -I/usr/include/libxml2
	EXE_LIST =  $(OBJ:.o=)
	LOADLIBES = $(filter-out $(TARGET).o, $(OBJ)) -lxml2
endif


.SUFFIXES: .c .cpp .o
.PHONY: clean all run depend remake

all: $(OBJ) $(TARGET)

.cpp.o:
	$(CC) -c $< -o $@ $(CFLAGS)
.c.o:
	$(CC) -c $< -o $@ $(CFLAGS)



clean:
	@rm -f $(OBJ) $(EXE_LIST)

remake: clean all

depend:
	$(CC) -MM $(SRC) > depend.inc
-include depend.inc

