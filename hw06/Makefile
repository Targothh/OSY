CXX = g++
CXXFLAGS = -Wall -g -O2
EXTRA_CFLAGS ?= -fsanitize=address -fno-omit-frame-pointer

SRCS = factory.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC = factory

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $(EXEC) $(EXTRA_CFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(EXTRA_CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)

