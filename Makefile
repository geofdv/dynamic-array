CC = gcc
CFLAGS = -std=c89 -Wall -Werror -g3 -pedantic -DLINUX -D_GNU_SOURCE 
SRCMODULES = dynarray.c
OBJMODULES = $(SRCMODULES:.c=.o)

%.o: %.c %.h
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

exe: dynarray_test.c $(OBJMODULES)
	$(CC) $(INCLUDES) $(CFLAGS) $^ -o $@

run_tests: exe
	./exe

mem_check: exe
	valgrind ./exe --leak-check=full -s

ifneq (clean, $(SRCMODULES))
-include deps.mk
endif

deps.mk: $(SRCMODULES)
	$(CC) -MM $^ > $@

clean:
	rm -f *.o exe
