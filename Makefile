CC = gcc
CXX=g++
PREFIX=/usr
SRC_DIR=src
CFLAGS = -Wall -g
INCLUDES = -I./$(SRC_DIR)
LDFLAGS = 
LIBS = -L$(PREFIX)/lib -lssh
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:.c=.o)
MAIN = wr-port-blocker

.PHONY: depend clean

all:    $(MAIN)

$(MAIN): $(OBJS) 
	$(CXX) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LDFLAGS) $(LIBS)

.c.o:
	$(CXX) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
