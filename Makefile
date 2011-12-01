SRC = $(shell ls *.cpp)
OBJ0 = $(SRC:.c=.o)
OBJ  = $(OBJ0:.cpp=.o)

CC = g++
ifeq ($(OS),Windows_NT)
# Windows
	CFLAGS = -Wall -Wextra -O3
else
# Linux
	CFLAGS = -Wall -Wextra -O3 -I/usr/include/libxml2
endif

.SUFFIXES: .c .cpp .o
.PHONY: clean all run depend remake

all: $(OBJ)

.cpp.o:
	$(CC) -c $< -o $@ $(CFLAGS)
.c.o:
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	@rm -f $(OBJ)

remake: clean all

depend:
	$(CC) -MM $(SRC) > depend.inc
-include depend.inc

