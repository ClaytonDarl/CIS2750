CPPFLAGS = -Iinclude
CFLAGS = -std=c11 -g -Wall -pedantic

all: parser

parser:
	gcc $(CPPFLAGS) $(CFLAGS) -shared -fpic src/CalendarParser.c src/helperFunctions.c src/LinkedListAPI.c -o ../sharedLib.so

clean:
	rm *.so
