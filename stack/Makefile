CFLAGS += -I./include

.PHONY: all
all: $(BUILD)/include/c-utils/stack.h $(LIB_TARGET)

$(LIB_TARGET): $(OBJ_DIR)/stack_stack.o
	$(AR) $(ARFLAGS) $@ $^

$(OBJ_DIR)/stack_stack.o: src/stack.c include/stack.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/include/c-utils/stack.h: include/stack.h
	cp -v $^ $@

# Test
.PHONY: test
test: $(TEST_TARGET)

$(TEST_TARGET): test/stack_tests.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Format
FORMAT_FILES=src/stack.c include/stack.h test/stack_tests.cpp

.PHONY: checkformat
checkformat:
	$(FORMAT) $(FORMAT_CHECK_FLAGS) $(FORMAT_FILES)

.PHONY: format
format:
	$(FORMAT) $(FORMAT_FIX_FLAGS) $(FORMAT_FILES)
