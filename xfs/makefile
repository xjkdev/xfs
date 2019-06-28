CC=gcc
DIR_INCLUDE = include
CFLAGS = --std=gnu99 -I. -g -L/usr/local/lib -I/usr/local/include -lreadline
DEPS = $(wildcard $(DIR_INCLUDE)/**/*.h)
OUTPUT = target
TARGET = xfs
SRC = $(filter-out test_%.c, $(wildcard *.c))
TEST_SRC = $(filter test_%.c, $(wildcard *.c))
OBJS = $(patsubst %.c,${OUTPUT}/%.o, $(notdir ${SRC}))
TEST_OBJS = $(filter-out $(OUTPUT)/main.o, $(OBJS))
TEST_TARGET = $(patsubst %.c, $(TEST_OUTPUT)/%, $(TEST_SRC))
TEST_OUTPUT = $(OUTPUT)/test

all: all_before build_target

test: all_before test_before test_generic

build_target: $(OUTPUT)/$(TARGET)

all_before:
	-test -d $(OUTPUT) || mkdir $(OUTPUT)

$(OUTPUT)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $(CFLAGS) $<

$(OUTPUT)/$(TARGET): $(OBJS)
	$(CC) -o $@ $(CFLAGS) $(OBJS)

test_before:
	-test -d $(TEST_OUTPUT) && rm $(TEST_OUTPUT)/test_* || mkdir $(TEST_OUTPUT)

test_generic: $(TEST_TARGET)

$(TEST_TARGET): $(TEST_OUTPUT)/%: %.c $(TEST_OBJS)
	$(CC) -o $@ $(CFLAGS) -w $< $(TEST_OBJS) && $@

clean:
	rm -rf $(OUTPUT)
