CC = gcc

TARGET = compiler
SRCS = compiler.c utils.c token.c lexer.c parser.c ast.c translator.c memory.c test.c
OPTIONS = -g -Wall

$(TARGET): $(SRCS)
	$(CC) $(OPTIONS) -o $(TARGET) $(SRCS)

all:$(TARGET)

clean:
	$(RM) $(TARGET)