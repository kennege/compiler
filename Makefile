CC = gcc

TARGET = goLite
SRCS = interpreter.c utils.c token.c lexer.c parser.c ast.c translator.c stack.c integer.c float.c string.c scope_table.c test.c
OPTIONS = -g -Wall

$(TARGET): $(SRCS)
	$(CC) $(OPTIONS) -o $(TARGET) $(SRCS)

all:$(TARGET)

clean:
	$(RM) $(TARGET)