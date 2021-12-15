CC=clang
CFLAGS  += -O0 -std=gnu11 -Wall -pedantic -g -fno-omit-frame-pointer -DSTAFF -fPIC
# Detect OS
detected_OS := $(shell uname)
# Add -lasan, -lm, -lsubunit and -lrt if building on Linux, not required on OS X
ifeq ($(detected_OS),Linux)
	LDLIBS += -lasan -lrt -lm -lsubunit
endif
LDLIBS  += -lcheck -pthread
# Stop clang from compiling about Check headers
CFLAGS  += -Wno-gnu-zero-variadic-macro-arguments


# add sanitize to ldflags, required when building using clang native on OS X
ifeq ($(detected_OS),Darwin)
	LDFLAGS += -fsanitize=address

	# Add current path to library paths
	CFLAGS += -Wl,-rpath .
else
	CFLAGS += -Wl,-rpath=.
endif


COMMON += blk_io.o close.o fs_util.o open.o open_file_table.o read.o readdir.o seek.o disk.o
HEADERS += blk_io.h fs_api.h fs_structs.h fs_util.h open_file_table.h disk.h
TESTS = test_fs
APP = lab3_cat lab3_ls

## ---------------------------------------------------
## --------- Template stuff : Do not touch -----------

all: $(APP) $(TESTS)

feedback: $(TESTS)
	docker pull atrib/cs323_lab3:w10
	-docker run --rm --name=cs323 --cap-add=SYS_PTRACE -v ${shell pwd}:/done atrib/cs323_lab3:w10 ${TESTS}
common.so: ${COMMON}
	${CC} ${CPPFLAGS} ${CFLAGS} -g -shared -o common.so ${COMMON}

%.o: %.c $(HEADERS)

clean:
	@rm -f $(APP) $(TESTS) common.so *.o

test: ${TESTS}
	${foreach test,${TESTS}, ./${test} ../test_fs}

%.o: %.c $(HEADERS)

# Template for requirements for APPS and TESTS
# Apart from the corresponding .c files, APPS and TESTS
# require COMMON and HEADERS too
define REQS_template
$(1): $$($(1).c) common.so
	$(CC) $(CFLAGS) -c -g $(1).c -o $(1).o
	$(CC) $(CFLAGS) -g $(1).o $(LDLIBS) common.so $(LDFLAGS) -o $(1)
endef

$(foreach app,$(APP),$(eval $(call REQS_template,$(app))))
$(foreach test,$(TESTS),$(eval $(call REQS_template,$(test))))

