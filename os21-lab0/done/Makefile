
CFLAGS  += -std=c11 -Wall -pedantic -fPIC
LDLIBS  += -lcheck -lm -lrt -pthread -lsubunit
PWD = ${shell pwd}

COMMON = week01.o
HEADERS = week01.h
TESTS = tests

COMMON += week02.o
HEADERS += week02.h
APP += main

# ## ---------------------------------------------------
# ## --------- Template stuff : Do not touch -----------
.PHONY: all test feedback clean
all: ${APP} ${TESTS}

feedback:
	docker pull atrib/cs323_lab0:w2
	-docker run --rm --name=cs323 --cap-add=SYS_PTRACE -v ${shell pwd}:/done atrib/cs323_lab0:w2 ${TESTS}

test: ${TESTS}
	${foreach test,${TESTS},LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${PWD} ./${test}}

common.so: ${COMMON}
	${CC} ${CPPFLAGS} ${CFLAGS} -g -fPIC -shared -o common.so ${COMMON}

%.o: %.c ${HEADERS}

clean:
	rm -f ${APP} ${TESTS} common.so *.o

# Template for requirements for APPS and TESTS
# Apart from the corresponding .o files, APPS and TESTS
# link with the shared library
define REQS_template
${1}: $${${1}.o} ${SOLUTION} common.so
endef

${foreach app,${APP},${eval ${call REQS_template,${app}}}}
${foreach test,${TESTS},${eval ${call REQS_template,${test}}}}
