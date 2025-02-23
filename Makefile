CC 			= g++-13
LD			= g++-13
CXXFLAGS 	= -O0 -Wall -std=c++20 -fPIC -shared -g
LDFLAGS		= -lstdc++ -std=c++20 -fPIC -shared -pthread -g
TEST_LDFLAGS= -lstdc++ -L. -g
TEST_LDLIBS = -lmythreadpool

SOURCES 	= $(wildcard *.cc)
HEADERS		= $(wildcard *.hh)
TEST_SOURCES= $(wildcard tests/*.cc)

OBJS 		= $(patsubst %.cc, %.o, $(SOURCES))
TEST_OBJS 	= $(patsubst %.cc, %.o, $(TEST_SOURCES))

TARGET		:= libmythreadpool.so
TEST_TARGET := tests/main

all : $(TARGET) $(TEST_TARGET)

%.o : %.cc %.hh
	$(CC) $(CXXFLAGS) -c $< -o $@

tests/%.o : tests/%.cc
	$(CC) $(CXXFLAGS) -c $< -o $@

$(TARGET) : $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(TARGET)

$(TEST_TARGET) : $(TEST_OBJS) $(TARGET)
	$(LD) $(TEST_LDFLAGS) $(TEST_OBJS) $(TEST_LDLIBS) -o $(TEST_TARGET)

test : $(TEST_TARGET)
	LD_LIBRARY_PATH=. $(TEST_TARGET)

verify: 
	clang++ tests/verifier.cpp -o tests/verifier
	tests/verifier

clean : 
	rm -rf $(OBJS) $(TEST_OBJS) $(TARGET) $(TEST_TARGET)

.PHONY : all test clean verify

