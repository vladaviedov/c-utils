CFLAGS += -I./include
BUILD=../build
TARGET=$(BUILD)/obj/nanorl.o

.PHONY: all
all: headers $(TARGET)

$(TARGET): src/nanorl.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: headers
headers:
	cp -r include/** $(BUILD)/include/c-utils

# Test
TARGET_TEST=$(BUILD)/obj/nanorl_tests.o

.PHONY: test
test: $(TARGET_TEST)

$(TARGET_TEST): test/nanorl_tests.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Format
FORMAT_FILES=src/nanorl.c include/nanorl.h
	# test/stack_tests.cpp

.PHONY: checkformat
checkformat:
	$(FORMAT) $(FORMAT_CHECK_FLAGS) $(FORMAT_FILES)

.PHONY: format
format:
	$(FORMAT) $(FORMAT_FIX_FLAGS) $(FORMAT_FILES)
