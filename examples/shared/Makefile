EXEC := $(basename $(shell ls *.cpp))
OBJS := ../../src/error ../../src/plugins/cli/io ../../src/plugins/repl/io


CC := g++
INCLUDE_PATH := ../../src
CC_ARGS := -Wall -Wextra -pedantic -I $(INCLUDE_PATH)


OBJS := $(addsuffix .o, $(OBJS))

.PHONY: all check clean distclean


all: $(EXEC)

$(EXEC): $(EXEC).cpp $(OBJS)
	$(CC) $(CC_ARGS) -o $@ $^

%.o: %.cpp
	$(CC) $(CC_ARGS) -o $@ -c $^

check: all
	valgrind ./$(EXEC)

clean:
	rm -f $(OBJS)

distclean: clean
	rm -f $(EXEC)
