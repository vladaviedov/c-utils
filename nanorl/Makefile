CFLAGS += -I./include
OBJECTS=$(OBJ_DIR)/nanorl_nanorl.o \
		$(OBJ_DIR)/nanorl_terminfo.o \
		$(OBJ_DIR)/nanorl_io.o \
		$(OBJ_DIR)/nanorl_fastload.o \
		$(OBJ_DIR)/nanorl_dfa.o

.PHONY: all
all: $(BUILD)/include/c-utils/nanorl.h $(LIB_TARGET)

$(LIB_TARGET): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

$(OBJ_DIR)/nanorl_nanorl.o: src/nanorl.c src/terminfo.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/nanorl_terminfo.o: src/terminfo.c src/terminfo.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/nanorl_io.o: src/io.c src/io.h src/terminfo.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/nanorl_fastload.o: src/fastload.c src/fastload.h src/terminfo.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/nanorl_dfa.o: src/dfa.c src/dfa.h src/terminfo.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/include/c-utils/nanorl.h: include/nanorl.h
	cp -v $^ $@

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
