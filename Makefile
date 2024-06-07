export CC=gcc
export CFLAGS=-Wall -Wextra -g -std=c99 -I ../build/include
export AR=ar
export ARFLAGS=rvsc
BUILD=build

TARGET=$(BUILD)/lib/libutils.a

COMPONENTS=
OBJECTS=
TEST_COMPONENTS=
TEST_OBJECTS=
DOC_DIRS=

.PHONY: all
all: dirs $(TARGET)

# make_sublib(target_name)
define make_sublib
OBJECTS += $(BUILD)/lib/$(1).a
DOC_DIRS += $(1)/src $(1)/include

.PHONY: $(1)
$(BUILD)/lib/$(1).a:
	$(MAKE) -C $(1) \
		LIB_TARGET=../$(BUILD)/lib/$(1).a
endef

# make_sublib_test(target_name)
define make_sublib_test
TEST_COMPONENTS += $(1)_tests
TEST_OBJECTS += $(BUILD)/obj/$(1)_tests.o

.PHONY: $(1)_tests
$(1)_tests:
	$(MAKE) -C $(1) test \
		TEST_TARGET=../$(BUILD)/obj/$(1)_tests.o
endef

# Configuration
CONFIG_PATH=build.conf
include $(CONFIG_PATH)

ifeq ($(vector),1)
$(eval $(call make_sublib,vector))
$(eval $(call make_sublib_test,vector))
endif

ifeq ($(stack),1)
$(eval $(call make_sublib,stack))
$(eval $(call make_sublib_test,stack))
endif

# Build
.PHONY: dirs
dirs:
	mkdir -p $(BUILD)
	mkdir -p $(BUILD)/lib
	mkdir -p $(BUILD)/obj
	mkdir -p $(BUILD)/include/c-utils
	mkdir -p $(BUILD)/test

.PHONY: $(TARGET)
$(TARGET): $(OBJECTS)
	rm -f $(TARGET)
	./repack.sh $@ $^

.PHONY: clean
clean:
	rm -rf $(BUILD)

# Test
export CXX=g++
export CXXFLAGS=-Wall -Wextra -g -std=c++14 -I ../build/include
TEST_LDFLAGS=-lgtest -lgtest_main -lgcov
TEST_TARGET=$(BUILD)/test/runtest
COV_DIR=cov

.PHONY: test
test: all $(TEST_COMPONENTS)
	rm -f $(TEST_TARGET)
	$(CXX) $(TEST_OBJECTS) $(TARGET) $(TEST_LDFLAGS) -o $(TEST_TARGET)
	$(TEST_TARGET)

.PHONY: coverage
coverage: CFLAGS += -fprofile-arcs -ftest-coverage
coverage: 
	$(MAKE) clean
	mkdir -p cov
	$(MAKE) all CFLAGS='$(CFLAGS)'
	lcov --no-external --capture --initial -d . -o $(COV_DIR)/report_base.info
	$(MAKE) test
	lcov --no-external --capture -d $(shell pwd) -o $(COV_DIR)/report_aux.info
	lcov -a $(COV_DIR)/report_base.info -a $(COV_DIR)/report_aux.info -o $(COV_DIR)/report.info
	genhtml $(COV_DIR)/report.info -o $(COV_DIR)

# Format
export FORMAT=clang-format
export FORMAT_CHECK_FLAGS=--dry-run --Werror
export FORMAT_FIX_FLAGS=-i
FORMAT_DIRS=\
	vector \
	stack

.PHONY: checkformat
checkformat:
	for dir in $(FORMAT_DIRS); do \
		$(MAKE) -C $$dir checkformat; \
	done

.PHONY: format
format:
	for dir in $(FORMAT_DIRS); do \
		$(MAKE) -C $$dir format; \
	done

# Documentation
DOXYGEN=doxygen
DOXYGEN_CONF=Doxyfile

.PHONY: docs
docs:
	DOC_DIRS='$(DOC_DIRS)' $(DOXYGEN) $(DOXYGEN_CONF)
