all: all-slakkenc all-slakkenrt

test: test-slakkenc test-slakkenrt

.PHONY: slakkenc
all-slakkenc:
	${MAKE} -C slakkenc all

.PHONY: test-slakkenc
test-slakkenc:
	${MAKE} -C slakkenc test

.PHONY: all-slakkenrt
all-slakkenrt:
	${MAKE} -C slakkenrt all

.PHONY: test-slakkenrt
test-slakkenrt:
	${MAKE} -C slakkenrt test
