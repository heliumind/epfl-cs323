COMMONFLAGS := -O0 -Wall -Wextra -pedantic -g -fsanitize=address \
				-Wno-unused-parameter
CFLAGS  += $(COMMONFLAGS) -std=gnu11
CXXFLAGS  += $(COMMONFLAGS) -std=gnu++14
LDFLAGS +=

# Detect OS
detected_OS := $(shell uname)

# Add libs if building on Linux, not required on OS X
ifeq ($(detected_OS),Linux)
	LDLIBS += -lasan
endif

APP := demo borsh

all: $(APP)

clean:
	@rm -f $(APP) demo.o borsh.o

# Template for requirements for APPS
define REQS_template
$(1): $$($(1).o)
endef

$(foreach app,$(APP),$(eval $(call REQS_template,$(app))))