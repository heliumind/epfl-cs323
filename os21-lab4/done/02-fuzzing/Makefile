export CC := clang
export CXX := clang++
COMMONFLAGS := -O0 -Wall -Wextra -pedantic -g -fsanitize=address,fuzzer \
		-Iinclude
CFLAGS  += $(COMMONFLAGS) -std=gnu11
CXXFLAGS  += $(COMMONFLAGS) -std=gnu++14
LDFLAGS += -Llib
LIBPNGVERS := 001 002 003

# Detect OS
detected_OS := $(shell uname)

# Add libs if building on Linux, not required on OS X
ifeq ($(detected_OS),Linux)
	LDLIBS += -lz
endif

HEADERS := include/png.h
APP := fuzzer_native_001 fuzzer_native_002 fuzzer_simplified_003

all: $(APP)

define LIBPNG_template
lib/libpng16_$1.a: magma
	$(MAKE) -C magma setup/libpng16.patch bugs/$1.patch libpng16
	cp magma/build/libpng16.a lib/libpng16_$1.a
endef
$(foreach ver,$(LIBPNGVERS),$(eval $(call LIBPNG_template,$(ver))))

fuzzer_native_%: fuzzer_native.o lib/libpng16_%.a
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

fuzzer_simplified_%: fuzzer_simplified.o lib/libpng16_%.a
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

magma:
	tar xzf magma.tar.gz
	mkdir -p lib

clean:
	@rm -f $(APP)

