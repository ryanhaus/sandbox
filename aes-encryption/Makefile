CC = gcc
SRCS = main.c aes.c
CFLAGS = -Wall -g
TARGET = aes-encryption

$(TARGET): $(SRCS:.c=.o)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(TARGET) $(SRCS:.c=.o)
