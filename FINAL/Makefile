AS=arm-none-eabi-as -mcpu=arm926ej-s
LD=arm-none-eabi-ld
CC=arm-none-eabi-gcc -mcpu=arm926ej-s
OBJCOPY=arm-none-eabi-objcopy

debug: CFLAGS :=  -g -Wall -fno-stack-protector
release: CFLAGS := -O2 --pipe -march=native -fno-stack-protector

TARGET := t.elf

GLOBAL_INCLUDES := -I./type.h
KERNEL_INCLUDES := -I./kernel/include
FS_INCLUDES := -I./fs/include
DRIVER_INCLUDES := -I./driver/include

KERNEL_SRCS := ./kernel/src/*.c
KERNEL_ASM_SRCS := ./kernel/src/*.s
FS_SRCS := ./fs/src/*.c
DRIVER_SRCS := ./driver/src/*.c

KERNEL_OBJS := $(KERNEL_SRCS:.c=.o) 
KERNEL_ASM_OBJS := $(KERNEL_ASM_SRCS:.s=.o)
FS_OBJS := $(FS_SRCS:.c=.o)
DRIVER_OBJS := $(DRIVER_SRCS:.c=.o)

.PHONY: clean

debug : $(TARGET)
	@echo Debug build finished

release : $(TARGET)
	@echo Release build finished

$(DRIVER_OBJS): $(DRIVER_SRCS)
	$(CC) $(CFLAGS) $(DRIVER_INCLUDES) $(GLOBAL_INCLUDES) -c $(DRIVER_SRCS) -o $(DRIVER_OBJS)

$(FS_OBJS): $(FS_SRCS)
	$(CC) $(CFLAGS) $(FS_INCLUDES) $(GLOBAL_INCLUDES) -c $(FS_SRCS) -o $(FS_OBJS)

$(KERNEL_ASM_OBJS) : $(KERNEL_ASM_SRCS)
	$(AS) $(KERNEL_INCLUDES) $(GLOBAL_INCLUDES) $(KERNEL_ASM_SRCS) -o $(KERNEL_ASM_OBJS)

$(KERNEL_OBJS): $(KERNEL_SRCS)
	$(CC) $(CFLAGS) $(KERNEL_INCLUDES) $(GLOBAL_INCLUDES) $(KERNEL_SRCS) -o $(KERNEL_OBJS)

$(TARGET): $(KERNEL_OBJS) $(FS_OBJS) $(DRIVER_OBJS) $(KERNEL_ASM_OBJS)
	$(LD) -T ./kernel/src/t.ld -Ttext=0x10000 $(FS_OBJS) $(DRIVER_OBJS) $(KERNEL_OBJS) $(KERNEL_ASM_OBJS) -o $(TARGET)
	$(OBJCOPY) -O binary $(TARGET) t.bin

# .c.o:
# 	$(CC) $(CFLAGS) -c $< -o $@

# .c.s:
# 	$(AS) -s $< -o $@

clean:
	rm -f $(TARGET) $(KERNEL_OBJS) $(FS_OBJS) $(DRIVER_OBJS)