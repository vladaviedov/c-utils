BUILD=../build
TARGET=$(BUILD)/obj/vector.o

.PHONY: all
all: headers $(TARGET)

$(TARGET): src/vector.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: headers
headers:
	cp -r include $(BUILD)

# Test
TARGET_TEST=$(BUILD)/obj/vector_tests.o

.PHONY: test
test: $(TARGET_TEST)

$(TARGET_TEST): test/vector_tests.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Format
FORMAT_FILES=src/vector.c include/vector.h

.PHONY: checkformat
checkformat:
	$(FORMAT) $(FORMAT_CHECK_FLAGS) $(FORMAT_FILES)

.PHONY: format
format:
	$(FORMAT) $(FORMAT_FIX_FLAGS) $(FORMAT_FILES)
