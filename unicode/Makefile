CFLAGS += -I./include
OBJECTS=$(OBJ_DIR)/unicode_uchar.o \
		$(OBJ_DIR)/unicode_ustring.o \
		$(OBJ_DIR)/unicode_ucwidth.o

.PHONY: all
all: $(BUILD)/include/c-utils/ucwidth.h $(LIB_TARGET)

$(LIB_TARGET): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

$(OBJ_DIR)/unicode_uchar.o: src/uchar.c include/uchar.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/unicode_ustring.o: src/ustring.c include/ustring.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/unicode_ucwidth.o: src/ucwidth.c include/ucwidth.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/include/c-utils/ustring.h: include/ustring.h
	cp -v $^ $@

$(BUILD)/include/c-utils/ucwidth.h: include/ucwidth.h
	cp -v $^ $@

$(BUILD)/include/c-utils/uchar.h: include/uchar.h
	cp -v $^ $@

# Test
.PHONY: test
test: $(TEST_TARGET)

$(TEST_TARGET): test/uchar_tests.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Format
FORMAT_FILES=$(shell find src -type f) \
			 $(shell find include -type f) \
			 $(shell find example -type f)

.PHONY: checkformat
checkformat:
	$(FORMAT) $(FORMAT_CHECK_FLAGS) $(FORMAT_FILES)

.PHONY: format
format:
	$(FORMAT) $(FORMAT_FIX_FLAGS) $(FORMAT_FILES)
