CC=g++
CFLAGS=-g -Wall

DOBJS=decode.o BitStreams.o

# rule for building the executable
default:  decode

decode: $(DOBJS)
	$(CC) -o decode $(DOBJS)

all:      decode

# list of file dependencies
decode.o:           decode.cpp BitStreams.h LinkedBinaryTree.h
BitStreams.o:       BitStreams.h BitStreams.cpp

# the following rule is used to compile .cpp files to .o
.cpp.o:
	$(CC) $(CFLAGS) -c $<


# the following removes all .o files, but leaves the executable
clean:
	rm -f *.o
