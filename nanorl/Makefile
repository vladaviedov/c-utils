CFLAGS += -I./include
BUILD=../build
OBJECTS=$(BUILD)/obj/nanorl.o \
		$(BUILD)/obj/terminfo.o \
		$(BUILD)/obj/printer.o

.PHONY: all
all: headers $(LIB_TARGET)

$(LIB_TARGET): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

$(BUILD)/obj/nanorl.o: src/nanorl.c src/terminfo.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/obj/terminfo.o: src/terminfo.c src/terminfo.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/obj/printer.o: src/printer.c src/printer.h src/terminfo.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: headers
headers:
	cp -r include/** $(BUILD)/include/c-utils

# Test
.PHONY: test
test: $(TARGET_TEST)

$(TARGET_TEST): test/nanorl_tests.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Example
.PHONY: example
example: example/nanorl_example.c
	$(CC) $(CFLAGS) $< -o $(BIN_TARGET) $(LDFLAGS)

# Format
FORMAT_FILES=src/nanorl.c include/nanorl.h example/nanorl_example.c
	# test/stack_tests.cpp

.PHONY: checkformat
checkformat:
	$(FORMAT) $(FORMAT_CHECK_FLAGS) $(FORMAT_FILES)

.PHONY: format
format:
	$(FORMAT) $(FORMAT_FIX_FLAGS) $(FORMAT_FILES)
