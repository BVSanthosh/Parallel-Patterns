CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lpng -pthread

SRCS = example.c convolution.c para-pat.c task_queue.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: example convolution

example: example.o para-pat.o task_queue.o
	$(CC) $(CFLAGS) -o example example.o para-pat.o task_queue.o $(LDFLAGS)

convolution: convolution.o para-pat.o task_queue.o
	$(CC) $(CFLAGS) -o convolution convolution.o para-pat.o task_queue.o $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) example convolution
