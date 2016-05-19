CC=gcc 
CFLAGS=-g
DEPS = 

#names of source files
SRC = mreplace.c 

#remove .c from all SRC files 
BASENAMES = $(SRC:.c=)


#.PHONY declaration just says that 'all' isn't a file name.
.PHONY: all
all: $(BASENAMES) alloc.so


#this will compile our .so ...this means your malloc source must be named alloc.c 
alloc.so: alloc.c
	$(CC) $^ $(FLAGS) -o $@ -shared -fPIC 

#compile all .o into executable
%: %.o $(DEPS)
	$(CC) -o $@ $<


#https://www.gnu.org/software/make/manual/html_node/Cleanup.html#Cleanup
.PHONY: clean
clean:
	-rm -f *.o alloc.so $(BASENAMES)