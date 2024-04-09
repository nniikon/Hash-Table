SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRCS))

all: $(BUILD_DIR) $(OBJS)

$(BUILD_DIR)/%.o: %.cpp
	@$(GXX) $^ $(CFLAGS) -msse4.2 -c -o $@
	nasm hash_CRC32.asm -f elf64 -o ../build/hash_CRC32.o

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
