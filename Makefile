CC      = gcc
CFLAGS  = -Wall -Wextra -g
LDLIBS  = -lreadline

SRCS = main.c tokenizer.c prompt.c builtins.c executor.c
OBJS = $(SRCS:.c=.o)

lshell: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDLIBS)

# each .o is rebuilt if its .c or any header changes
%.o: %.c $(wildcard *.h)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) lshell

.PHONY: clean
