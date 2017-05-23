all: all-slakkenc

test: test-slakkenc

.PHONY: slakkenc
all-slakkenc:
	${MAKE} -C slakkenc all

.PHONY: test-slakkenc
test-slakkenc:
	${MAKE} -C slakkenc test
