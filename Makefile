CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -lssl -lcrypto

SRCS = imap.c
OBJS = $(SRCS:.c=.o)
EXECUTABLE = fetchmail

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXECUTABLE)
