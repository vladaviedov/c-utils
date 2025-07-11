CFLAGS += -I./include
OBJECTS=$(OBJ_DIR)/unicode_uchar.o \
		$(OBJ_DIR)/unicode_ustring.o \
		$(OBJ_DIR)/unicode_ucwidth.o
HEADERS=$(BUILD)/include/c-utils/uchar.h \
		$(BUILD)/include/c-utils/ustring.h \
		$(BUILD)/include/c-utils/ucwidth.h

EAST_ASIAN_DATA=data/EastAsianWidth-16.0.0.txt
EAST_ASIAN_SCRIPT=scripts/east_asian_parser.awk
EAST_ASIAN_GEN=gen/fullwidth.gen

ZERO_WIDTH_DATA=data/UnicodeData-16.0.0.txt
ZERO_WIDTH_SCRIPT=scripts/zero_width_parser.awk
ZERO_WIDTH_GEN=gen/zerowidth.gen

.PHONY: all
all: $(HEADERS) $(LIB_TARGET)

$(LIB_TARGET): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

$(EAST_ASIAN_GEN): $(EAST_ASIAN_DATA)
	mkdir -p gen
	$(AWK) -f $(EAST_ASIAN_SCRIPT) $^ > $@

$(ZERO_WIDTH_GEN): $(ZERO_WIDTH_DATA)
	mkdir -p gen
	$(AWK) -f $(ZERO_WIDTH_SCRIPT) $^ > $@

$(OBJ_DIR)/unicode_uchar.o: src/uchar.c include/uchar.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/unicode_ustring.o: src/ustring.c include/ustring.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/unicode_ucwidth.o: src/ucwidth.c include/ucwidth.h $(EAST_ASIAN_GEN) $(ZERO_WIDTH_GEN)
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

$(TEST_TARGET): test/unicode_tests.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Format
FORMAT_FILES=$(shell find src -type f) \
			 $(shell find include -type f) \
			 $(shell find test -type f -name '*.cpp')

.PHONY: checkformat
checkformat:
	$(FORMAT) $(FORMAT_CHECK_FLAGS) $(FORMAT_FILES)

.PHONY: format
format:
	$(FORMAT) $(FORMAT_FIX_FLAGS) $(FORMAT_FILES)
