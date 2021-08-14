CC = gcc

TARGET = compiler
SRCS = compiler.c lexer.c parser.c token.c utils.c
OPTIONS = -g -Wall
LIBS := -lm

$(TARGET): $(SRCS)
	$(CC) $(OPTIONS) -o $(TARGET) $(SRCS) $(LIBS)

all:$(TARGET)

clean:
	$(RM) $(TARGET)