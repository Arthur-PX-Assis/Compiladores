CC = gcc
CFLAGS = -Wall -Wextra
TARGET = compilador
SRCS = main.c csmall.c

TEST_FILE = testes/ex6.txt

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

run: $(TARGET)
	./$(TARGET) $(TEST_FILE)

clean:
	rm -f $(TARGET)