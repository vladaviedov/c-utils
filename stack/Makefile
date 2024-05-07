BUILD=../build
TARGET=$(BUILD)/obj/stack.o

.PHONY: all
all: headers $(TARGET)

$(TARGET): src/stack.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: headers
headers:
	cp -r include $(BUILD)

# Test
TARGET_TEST=$(BUILD)/obj/stack_tests.o

.PHONY: test
test: $(TARGET_TEST)

$(TARGET_TEST): test/stack_tests.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Format
FORMAT_FILES=src/stack.c include/stack.h

.PHONY: checkformat
checkformat:
	$(FORMAT) $(FORMAT_CHECK_FLAGS) $(FORMAT_FILES)

.PHONY: format
format:
	$(FORMAT) $(FORMAT_FIX_FLAGS) $(FORMAT_FILES)
