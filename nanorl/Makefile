CFLAGS += -I./include
OBJECTS=$(BUILD)/obj/nanorl_nanorl.o \
		$(BUILD)/obj/nanorl_terminfo.o \
		$(BUILD)/obj/nanorl_io.o \
		$(BUILD)/obj/nanorl_fastload.o \
		$(BUILD)/obj/nanorl_dfa.o

.PHONY: all
all: headers $(LIB_TARGET)

$(LIB_TARGET): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

$(BUILD)/obj/nanorl_nanorl.o: src/nanorl.c src/terminfo.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/obj/nanorl_terminfo.o: src/terminfo.c src/terminfo.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/obj/nanorl_io.o: src/io.c src/io.h src/terminfo.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/obj/nanorl_fastload.o: src/fastload.c src/fastload.h src/terminfo.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/obj/nanorl_dfa.o: src/dfa.c src/dfa.h src/terminfo.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: headers
headers:
	cp -r include/** $(BUILD)/include/c-utils

# Example
.PHONY: example
example: example/nanorl_example.c
	$(CC) $(CFLAGS) $< -o $(BIN_TARGET) $(LDFLAGS)

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
