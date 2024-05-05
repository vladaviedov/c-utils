export CC=gcc
export CFLAGS=-Wall -Wextra -g -std=c99 -I ../build/include
AR=ar
ARFLAGS=rsc
BUILD=build

TARGET=$(BUILD)/lib/libutils.a
COMPONENTS=
OBJECTS=
TEST_COMPONENTS=
TEST_OBJECTS=

# Configuration
CONFIG=build.conf
include $(CONFIG)

ifeq ($(vector),1)
COMPONENTS += vector
OBJECTS += $(BUILD)/obj/vector.o
TEST_COMPONENTS += vector_tests
TEST_OBJECTS += $(BUILD)/obj/vector_tests.o
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
	mkdir -p $(BUILD)/test

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

# Test
export CXX=g++
export CXXFLAGS=-Wall -Wextra -g -std=c++14 -I ../build/include
TEST_LDFLAGS=-lgtest -lgtest_main
TEST_TARGET=$(BUILD)/test/runtest

.PHONY: test
test: all $(TEST_COMPONENTS)
	rm -f $(TEST_TARGET)
	$(CXX) $(TEST_OBJECTS) $(TARGET) $(TEST_LDFLAGS) -o $(TEST_TARGET)
	$(TEST_TARGET)

.PHONY: vector_tests
vector_tests:
	$(MAKE) -C vector test

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
