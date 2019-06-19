CC=gcc
CFLAGS = --std=gnu99
DIR_INCLUDE = include
DEPS = $(wildcard $(DIR_INCLUDE)/*.h)  
OUTPUT = target
TARGET = xfs
SRC = $(wildcard *.c)  
OBJS = $(patsubst %.c,${OUTPUT}/%.o,$(notdir ${SRC}))

all: all_before build_target

build_target: $(OUTPUT)/$(TARGET)

all_before:
	-test -d $(OUTPUT) || mkdir $(OUTPUT)

$(OUTPUT)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $(CFLAGS) $<

$(OUTPUT)/$(TARGET): $(OBJS)
	$(CC) -o $@ $(CFLAGS) $(OBJS)

clean:
	rm -rf $(OUTPUT)
