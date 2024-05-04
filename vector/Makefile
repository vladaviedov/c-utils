BUILD=../build
TARGET=$(BUILD)/obj/vector.o

.PHONY: all
all: headers $(TARGET)

$(TARGET): src/vector.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: headers
headers:
	cp -r include $(BUILD)

# Formatting
FORMAT_FILES=src/vector.h include/vector.h

.PHONY: checkformat
checkformat:
	$(FORMAT) $(FORMAT_CHECK_FLAGS) $(FORMAT_FILES)

.PHONY: format
format:
	$(FORMAT) $(FORMAT_FIX_FLAGS) $(FORMAT_FILES)
