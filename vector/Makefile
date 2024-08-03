CFLAGS += -I./include

.PHONY: all
all: $(BUILD)/include/c-utils/vector.h $(LIB_TARGET)

$(LIB_TARGET): $(OBJ_DIR)/vector_vector.o
	$(AR) $(ARFLAGS) $@ $^

$(OBJ_DIR)/vector_vector.o: src/vector.c include/vector.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/include/c-utils/vector.h: include/vector.h
	cp -v $^ $@

# Test
.PHONY: test
test: $(TEST_TARGET)

$(TEST_TARGET): test/vector_tests.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Format
FORMAT_FILES=src/vector.c include/vector.h test/vector_tests.cpp

.PHONY: checkformat
checkformat:
	$(FORMAT) $(FORMAT_CHECK_FLAGS) $(FORMAT_FILES)

.PHONY: format
format:
	$(FORMAT) $(FORMAT_FIX_FLAGS) $(FORMAT_FILES)
