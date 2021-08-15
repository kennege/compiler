CC = gcc

TARGET = compiler
SRCS = compiler.c utils.c token.c lexer.c ast.c parser.c 
OPTIONS = -g -Wall

$(TARGET): $(SRCS)
	$(CC) $(OPTIONS) -o $(TARGET) $(SRCS)

all:$(TARGET)

clean:
	$(RM) $(TARGET)