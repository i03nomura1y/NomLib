# create *.o:
#   $ make
# create xxx.exe with -DTEST option:
#  $ make TEST=xxx; or  $ make remake TEST=xxx

SRC = $(shell ls *.cpp *.c)
OBJ0 = $(SRC:.c=.o)
OBJ  = $(OBJ0:.cpp=.o)
TARGET = libNomLib.a

CC = g++
ifeq ($(OS),Windows_NT)			# Windows
	CFLAGS = -Wall -Wextra -O3
	EXE_LIST =  $(OBJ:.o=.exe)
#   LOADLIBES = $(shell ls *.o | grep -v $@) -lregex
#	LOADLIBES = $(filter-out $(TEST).o, $(OBJ)) -lregex -lxml2 -lws2_32
	LOADLIBES = $(filter-out $(TEST).o, $(OBJ)) -lregex -lxml2 -lz -liconv -lws2_32 -Wl,--enable-auto-import
else							# Linux
	CFLAGS = -Wall -Wextra -O3 -I/usr/include/libxml2 -I/usr/local/ssl/include
	EXE_LIST =  $(OBJ:.o=)
	LOADLIBES = $(filter-out $(TEST).o, $(OBJ)) -lxml2 -L/usr/local/ssl/lib -lcrypto
endif


.SUFFIXES: .c .cpp .o
.PHONY: test all clean clean_testobj remake run depend

ifdef TEST
# create executable file
all: $(OBJ) $(TEST)
clean:
	rm -f $(TEST).o
run: all
	./$(TEST)
else
all: $(TARGET)
clean:
	@rm -f $(OBJ) $(EXE_LIST)
$(TARGET): $(OBJ)
	@ar r $@ $^
endif

.cpp.o .c.o:
	@if [ "$(TEST).o" = "$@" ]; then \
	  echo "create $@ with -DTEST"; $(CC) -c $< -o $@ $(CFLAGS) -DTEST; \
	else \
	  echo "create $@"; $(CC) -c $< -o $@ $(CFLAGS); \
	fi

remake: clean all

depend:
	$(CC) -MM $(SRC) > depend.inc
-include depend.inc

