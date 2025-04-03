PWD=$(shell pwd)
export BUILD=$(PWD)/build

export CC=gcc
export CFLAGS=-std=c99 -I$(BUILD)/include
CFLAGS_RELEASE=-O2
CFLAGS_DEBUG=-Wall -Wextra -g
export AR=ar
export ARFLAGS=rvcS

CONFIG_PATH=build.conf
TARGET=$(BUILD)/lib/libutils.a
BUILD_DIRS=$(BUILD) \
		   $(BUILD)/obj \
		   $(BUILD)/objd \
		   $(BUILD)/lib \
		   $(BUILD)/bin \
		   $(BUILD)/include/c-utils \
		   $(BUILD)/test

# Release objects by default
export OBJ_DIR=$(BUILD)/obj

COMPONENTS=
OBJECTS=
TEST_COMPONENTS=
TEST_OBJECTS=
DOC_DIRS=
EXAMPLES=

.PHONY: release
release: CFLAGS += $(CFLAGS_RELEASE)
release: $(BUILD_DIRS) $(TARGET)

.PHONY: debug
debug: CFLAGS += $(CFLAGS_DEBUG)
debug: OBJ_DIR = $(BUILD)/objd
debug: $(BUILD_DIRS) $(TARGET)

# make_sublib(target_name)
define make_sublib
OBJECTS += $(BUILD)/lib/$(1).a
DOC_DIRS += $(1)/src $(1)/include

.PHONY: $(BUILD)/lib/$(1).a
$(BUILD)/lib/$(1).a:
	$(MAKE) -C $(1) \
		LIB_TARGET=$(BUILD)/lib/$(1).a
endef

# make_sublib_test(target_name)
define make_sublib_test
TEST_COMPONENTS += $(1)_tests
TEST_OBJECTS += $(BUILD)/objd/$(1)_tests.o

.PHONY: $(1)_tests
$(1)_tests:
	$(MAKE) -C $(1) test \
		TEST_TARGET=$(BUILD)/objd/$(1)_tests.o
endef

# make_sublib_example(target_name)
define make_sublib_example
EXAMPLES += $(BUILD)/bin/$(1)_example

.PHONY: $(BUILD)/bin/$(1)_example
$(BUILD)/bin/$(1)_example:
	$(MAKE) -C $(1) example \
		BIN_TARGET=$(BUILD)/bin/$(1)_example
endef

# make_build_dir(dir_name)
define make_build_dir
$(1):
	mkdir -p $$@
endef

# Build directory rules
$(foreach build_dir, $(BUILD_DIRS), \
	$(eval $(call make_build_dir,$(build_dir))))

# Configuration
include $(CONFIG_PATH)

ifeq ($(vector),1)
$(eval $(call make_sublib,vector))
$(eval $(call make_sublib_test,vector))
endif

ifeq ($(vector-ext),1)
$(eval $(call make_sublib,vector-ext))
$(eval $(call make_sublib_test,vector-ext))
endif

ifeq ($(stack),1)
$(eval $(call make_sublib,stack))
$(eval $(call make_sublib_test,stack))
endif

ifeq ($(nanorl),1)
$(eval $(call make_sublib,nanorl))
$(eval $(call make_sublib_example,nanorl))
endif

ifeq ($(unicode),1)
$(eval $(call make_sublib,unicode))
$(eval $(call make_sublib_test,unicode))
endif

# Build
$(TARGET): $(OBJECTS)
	rm -f $(TARGET)
	./repack.sh $(OBJ_DIR) $@ $^

.PHONY: clean
clean:
	rm -rf $(BUILD)

# Test
export CXX=g++
export CXXFLAGS=-Wall -Wextra -g -std=c++14 -I $(BUILD)/include
TEST_LDFLAGS=-lgtest -lgtest_main -lgcov
TEST_TARGET=$(BUILD)/test/runtest
COV_DIR=cov

.PHONY: test
test: debug $(TEST_COMPONENTS)
	rm -f $(TEST_TARGET)
	$(CXX) $(TEST_OBJECTS) $(TARGET) $(TEST_LDFLAGS) -o $(TEST_TARGET)
	$(TEST_TARGET)

.PHONY: coverage
coverage: CFLAGS_DEBUG += -fprofile-arcs -ftest-coverage
coverage: 
	$(MAKE) clean
	mkdir -p cov
	$(MAKE) debug CFLAGS_DEBUG='$(CFLAGS_DEBUG)'
	lcov --no-external --capture --initial -d . -o $(COV_DIR)/report_base.info
	$(MAKE) test
	lcov --no-external --capture -d $(shell pwd) -o $(COV_DIR)/report_aux.info
	lcov -a $(COV_DIR)/report_base.info -a $(COV_DIR)/report_aux.info \
		-o $(COV_DIR)/report.info
	genhtml $(COV_DIR)/report.info -o $(COV_DIR)

# Examples
export LDFLAGS=-L$(BUILD)/lib -lutils

.PHONY: example
example: $(EXAMPLES)

# Format
export FORMAT=clang-format
export FORMAT_CHECK_FLAGS=--dry-run --Werror
export FORMAT_FIX_FLAGS=-i
FORMAT_DIRS=\
	vector \
	vector-ext \
	stack \
	nanorl \
	unicode

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
