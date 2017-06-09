CC = gcc
CXX=g++
SRC_DIR=src
CFLAGS = -Wall -g -std=c++11
INCLUDES = -I./$(SRC_DIR) 
LFLAGS = 
LIBS = 

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:.c=.o)
MAIN = wr-port-blocker

.PHONY: depend clean

all:    $(MAIN)

$(MAIN): $(OBJS) 
	$(CXX) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

.c.o:
	$(CXX) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
