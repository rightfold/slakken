CXX=g++
LD=g++

CXXFLAGS=-std=c++14 -Wall -Wextra -Wpedantic
LDFLAGS=-std=c++14

SOURCES=$(shell find src -name '*.cpp')
HEADERS=$(shell find src -name '*.hpp')
OBJECTS=$(patsubst src/%.cpp,build/src/%.o,${SOURCES})

all: build/slakken

.PHONY: test
test: all

build/slakken: ${OBJECTS}
	mkdir -p $(dir $@)
	${LD} ${LDFLAGS} $^ -o $@

build/src/%.o: src/%.cpp ${HEADERS}
	mkdir -p $(dir $@)
	${CXX} ${CXXFLAGS} -c -o $@ $<
