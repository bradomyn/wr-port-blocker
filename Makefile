CC = gcc
CXX=g++
PREFIX=/usr
SRC_DIR=src
CFLAGS = -Wall -g -std=c++11
INCLUDES = -I./$(SRC_DIR) -I/usr/local/include
LDFLAGS = 
LIBS = -L$(PREFIX)/lib -lssh -L/usr/local/lib -llldpctl -lsnmp -lboost_system
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
