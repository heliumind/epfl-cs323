CFLAGS  += -O0 -std=gnu11 -Wall -pedantic -g -fno-omit-frame-pointer -DSTAFF -fPIC
LDLIBS  += -lcheck -lm -lrt -pthread -lsubunit

COMMON =  stack.o error.o malloc.o sched_policy.o schedule.o thread_list.o               thread.o
HEADERS = stack.h error.h malloc.h sched_policy.h schedule.h thread_list.h thread_info.h thread.h
TESTS = test_threading  
APP = main

## ---------------------------------------------------
## ------- Additions for week 04: scheduling ---------
TESTS += test_scheduler
COMMON  += l1_time.o priority.o
HEADERS += l1_time.h priority.h

## ---------------------------------------------------
## ------- Additions for week 05: allocators ---------
TESTS += test_malloc

## ---------------------------------------------------
## --------- Template stuff : Do not touch -----------

all: $(APP) $(TESTS)

feedback:
	docker pull atrib/cs323_lab1:w5
	-docker run --rm --name=cs323 --cap-add=SYS_PTRACE -v ${shell pwd}:/done atrib/cs323_lab1:w5 ${TESTS}

test: ${TESTS}
	${foreach test,${TESTS},LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${PWD} ./${test};}

common.so: ${COMMON}
	${CC} ${CPPFLAGS} ${CFLAGS} -g -shared -o common.so ${COMMON}

%.o: %.c $(HEADERS)

clean:
	@rm -f $(APP) $(TESTS) common.so
	@rm -f *.o

# Template for requirements for APPS and TESTS
# Apart from the corresponding .c files, APPS and TESTS
# require COMMON and HEADERS too
define REQS_template
$(1): $$($(1).c) common.so
endef

$(foreach app,$(APP),$(eval $(call REQS_template,$(app))))
$(foreach test,$(TESTS),$(eval $(call REQS_template,$(test))))

