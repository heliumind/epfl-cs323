CFLAGS  += -O0 -std=gnu11 -Wall -pedantic -g -fno-omit-frame-pointer -DSTAFF -fPIC
LDLIBS  += -lcheck -lm -lrt -pthread -lsubunit

COMMON +=  stack.o error.o schedule.o thread_list.o thread.o  sched_policy.o              l2_time.o priority.o sys_thread.o scheduler_state.o futex.o utils.o spinlock.o mutex.o channel.o linked_list.o
HEADERS += stack.h error.h sched_policy.h schedule.h thread_list.h thread_info.h thread.h l2_time.h priority.h sys_thread.h scheduler_state.h futex.h utils.h spinlock.h mutex.h channel.h linked_list.h
PROVIDED_OBJS = sched_policy.o thread.o
TESTS = test_locks 
APP = main

OTHERVAR := $(filter-out thread.o sched_policy.o,$(COMMON))
## TESTS should be for feedback, not for given code
#TESTS += test_multi_threading
#TESTS += test_should_error

## ---------------------------------------------------
## --------- Template stuff : Do not touch -----------

all: $(APP) $(TESTS)

feedback: 
	docker pull atrib/cs323_lab2:w8
	-docker run --rm --name=cs323 --cap-add=SYS_PTRACE -v ${shell pwd}:/done atrib/cs323_lab2:w8 ${TESTS}

test: $(TESTS)
	${foreach test,${TESTS},LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${PWD} ./${test};}

common.so: ${COMMON}
	${CC} ${CPPFLAGS} ${CFLAGS} -g -shared -o common.so ${COMMON}

%.o: %.c $(HEADERS)

clean:
	@rm -f $(APP) $(TESTS) common.so
	@rm -f $(OTHERVAR)

.PHONY: docker
# TODO: 1. Make requirements generic
docker: ${TESTS}
	mkdir -p test_executables
	${foreach test,${TESTS},\
		strip ${test};\
		mv ${test} test_executables;}
	docker build --pull --tag atrib/cs323_lab2:w${WEEK} . 

# Template for requirements for APPS and TESTS
# Apart from the corresponding .c files, APPS and TESTS
# require COMMON and HEADERS too
define REQS_template
$(1): $$($(1).c) common.so
endef

$(foreach app,$(APP),$(eval $(call REQS_template,$(app))))
$(foreach test,$(TESTS),$(eval $(call REQS_template,$(test))))

define GRADE_template
grade:: $(1)
	-$(GRADING_PY) -s ./$(1).c -x ./$(1)
endef

$(foreach test, $(TESTS), $(eval $(call GRADE_template,$(test))))
