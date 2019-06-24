CC=gcc
DIR_INCLUDE = include
CFLAGS = --std=gnu99 -I.
DEPS = $(wildcard $(DIR_INCLUDE)/*.h)  
OUTPUT = target
TARGET = xfs
SRC = $(filter-out test_%.c, $(wildcard *.c))
OBJS = $(patsubst %.c,${OUTPUT}/%.o, $(notdir ${SRC}))
TEST_OBJS = $(filter-out $(OUTPUT)/main.o, $(OBJS))
TEST_OUTPUT = $(OUTPUT)/test

all: all_before build_target

test: all_before test_before test_bitmap test_types_size

test_before:
	-test -d $(TEST_OUTPUT) || mkdir $(TEST_OUTPUT)

build_target: $(OUTPUT)/$(TARGET)

all_before:
	-test -d $(OUTPUT) || mkdir $(OUTPUT)

$(OUTPUT)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $(CFLAGS) $<

$(OUTPUT)/$(TARGET): $(OBJS)
	$(CC) -o $@ $(CFLAGS) $(OBJS)

test_bitmap: $(TEST_OUTPUT)/test_bitmap
	$(TEST_OUTPUT)/test_bitmap

$(TEST_OUTPUT)/test_bitmap: test_bitmap.c
	$(CC) -o $@ $(CFLAGS) test_bitmap.c

test_types_size: $(TEST_OUTPUT)/test_types_size $(DIR_INCLUDE)/bitmap.h
	$(TEST_OUTPUT)/test_types_size

$(TEST_OUTPUT)/test_types_size: test_types_size.c $(DIR_INCLUDE)/xfs/fs_types.h
	$(CC) -o $@ $(CFLAGS) test_types_size.c

clean:
	rm -rf $(OUTPUT)
