CFLAGS += -I./include
BUILD=../build

.PHONY: all
all: headers $(LIB_TARGET)

$(LIB_TARGET): $(BUILD)/obj/nanorl.o $(BUILD)/obj/terminfo.o
	$(AR) $(ARFLAGS) $@ $^

$(BUILD)/obj/nanorl.o: src/nanorl.c src/terminfo.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/obj/terminfo.o: src/terminfo.c src/terminfo.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: headers
headers:
	cp -r include/** $(BUILD)/include/c-utils

# Test
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
