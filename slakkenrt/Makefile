CXX=g++
LD=g++

CXXFLAGS=-std=c++14 -Wall -Wextra -Wpedantic -Ivendor
LDFLAGS=-std=c++14

SOURCES=$(shell find src -name '*.cpp' ! -name 'main.cpp')
HEADERS=$(shell find src -name '*.hpp')
OBJECTS=$(patsubst src/%.cpp,build/src/%.o,${SOURCES})

TEST_SOURCES=$(shell find test -name '*.cpp' ! -name 'main.cpp')
TEST_HEADERS=$(shell find test -name '*.hpp')
TEST_OBJECTS=$(patsubst test/%.cpp,build/test/%.o,${TEST_SOURCES})

all: build/slakken

.PHONY: test
test: build/slakken-test
	$<

.PHONY: docs
docs: ${SOURCES} ${HEADERS}
	doxygen Doxyfile

build/slakken: build/src/main.o ${OBJECTS}
	mkdir -p $(dir $@)
	${LD} ${LDFLAGS} $^ -o $@

build/slakken-test: build/test/main.o ${OBJECTS} ${TEST_OBJECTS}
	mkdir -p $(dir $@)
	${LD} ${LDFLAGS} $^ -o $@

build/%.o: %.cpp ${HEADERS} ${TEST_HEADERS}
	mkdir -p $(dir $@)
	${CXX} ${CXXFLAGS} -c -o $@ $<

src/bytecode.hpp: build/opcode.inc

build/opcode.inc: src/opcode.def
	mkdir -p $(dir $@)
	perl Opcode.pl
