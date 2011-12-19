# create *.o:
#   $ make
# create xxx.exe with -DTEST option:
#  $ make TEST=xxx; or  $ make remake TEST=xxx

SRC = $(shell ls *.cpp *.c)
OBJ0 = $(SRC:.c=.o)
OBJ  = $(OBJ0:.cpp=.o)

CC = g++
ifeq ($(OS),Windows_NT)			# Windows
	CFLAGS = -Wall -Wextra -O3
	EXE_LIST =  $(OBJ:.o=.exe)
#   LOADLIBES = $(shell ls *.o | grep -v $@) -lregex
	LOADLIBES = $(filter-out $(TEST).o, $(OBJ)) -lregex -lxml2 -lws2_32
else							# Linux
	CFLAGS = -Wall -Wextra -O3 -I/usr/include/libxml2 -I/usr/local/ssl/include
	EXE_LIST =  $(OBJ:.o=)
	LOADLIBES = $(filter-out $(TEST).o, $(OBJ)) -lxml2 -L/usr/local/ssl/lib -lcrypto
endif


.SUFFIXES: .c .cpp .o
.PHONY: test all clean clean_testobj remake run depend

ifdef TEST
# create executable file
all: clean_testobj $(OBJ) $(TEST)
clean_testobj:
	rm -f $(TEST).o
run: all
	./$(TEST)
else
all: $(OBJ)
endif

.cpp.o .c.o:
	@if [ "$(TEST).o" = "$@" ]; then \
	  echo "create $@ with -DTEST"; $(CC) -c $< -o $@ $(CFLAGS) -DTEST; \
	else \
	  echo "create $@"; $(CC) -c $< -o $@ $(CFLAGS); \
	fi

clean:
	@rm -f $(OBJ) $(EXE_LIST)

remake: clean all

depend:
	$(CC) -MM $(SRC) > depend.inc
-include depend.inc

