CFLAGS += -I./include

.PHONY: all
all: headers $(LIB_TARGET)

$(LIB_TARGET): $(BUILD)/obj/vector_vector.o
	$(AR) $(ARFLAGS) $@ $^

$(BUILD)/obj/vector_vector.o: src/vector.c include/vector.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: headers
headers:
	cp -r include/** $(BUILD)/include/c-utils

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
