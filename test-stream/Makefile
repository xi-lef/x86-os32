VERBOSE = @
CXX = g++
CC_SOURCES = $(shell find .. -name "*.cc" -and ! -name '.*' )
CXXFLAGS = -std=c++11 -m32 -I../object -I.
TARGET = test

all: $(TARGET)

$(TARGET): $(CC_SOURCES)
	$(VERBOSE) $(CXX) -o $@ $(CXXFLAGS) $^

clean:
	$(VERBOSE) rm -f $(TARGET)

.PHONY: all clean
