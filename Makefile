export CC=gcc
export CFLAGS=-Wall -Wextra -g -std=c99 -I ../build/include
AR=ar
ARFLAGS=rsc
BUILD=build

TARGET=$(BUILD)/lib/libutils.a
COMPONENTS=
OBJECTS=

# Configuration
CONFIG=build.conf
include $(CONFIG)

ifeq ($(vector),1)
COMPONENTS += vector
OBJECTS += $(BUILD)/obj/vector.o
endif

# Build
.PHONY: all
all: dirs $(TARGET)

.PHONY: dirs
dirs:
	mkdir -p $(BUILD)
	mkdir -p $(BUILD)/lib
	mkdir -p $(BUILD)/obj
	mkdir -p $(BUILD)/include

.PHONY: $(TARGET)
$(TARGET): $(COMPONENTS)
	rm -f $(TARGET)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

.PHONY: vector
vector:
	$(MAKE) -C vector

.PHONY: clean
clean:
	rm -rf $(BUILD)

# Format
export FORMAT=clang-format
export FORMAT_CHECK_FLAGS=--dry-run --Werror
export FORMAT_FIX_FLAGS=-i

.PHONY: checkformat
checkformat:
	$(MAKE) -C vector checkformat

.PHONY: format
format:
	$(MAKE) -C vector format
