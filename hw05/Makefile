CXX = g++
CXXFLAGS = -Wall -g -O2
EXTRA_CFLAGS ?= -fsanitize=address -fno-omit-frame-pointer

SRCS = prod-cons.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC = prod-cons

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $(EXEC) $(EXTRA_CFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(EXTRA_CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)

