CC = gcc
CFLAGS = -g


default: A1Part1 part2

A1Part1: A1.o error.o
	$(CC) $(CFLAGS) -o A1Part1 A1.o error.o

A1.o: A1.c apue.h
	$(CC) $(CFLAGS) -c A1.c

error.o: error.c apue.h
	$(CC) $(CFLAGS) -c error.c

part2: A1Part2

A1Part2: A1part2.o error.o
	$(CC) $(CFLAGS) -o A1Part2 A1part2.o error.o

A1part2.o: A1part2.c apue.h
	$(CC) $(CFLAGS) -c A1part2.c

error.o: error.c apue.h
	$(CC) $(CFLAGS) -c error.c

clean:
	$(RM) A1Part1 A1Part2 *.o
