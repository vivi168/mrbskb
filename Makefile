TOOLCHAIN := mipsel-none-elf-

ifeq ($(PSYQ_PATH),)
$(error PSYQ_PATH is not set)
endif

ifeq ($(MRUBY_PATH),)
$(error MRUBY_PATH is not set)
endif

ELF := MAIN.ELF
EXE := MAIN.EXE
ISO := sokoban.bin

CC := $(TOOLCHAIN)gcc
LD := $(TOOLCHAIN)gcc
OBJCOPY := $(TOOLCHAIN)objcopy

INC := -I$(PSYQ_PATH)/include -I$(MRUBY_PATH)/include -I$(MRUBY_PATH)/build/playstation/include

LIB := \
	$(MRUBY_PATH)/build/playstation/lib/libmruby.a \
	$(MRUBY_PATH)/build/playstation/lib/libmruby_core.a \
	$(PSYQ_PATH)/lib/libcard.a \
	$(PSYQ_PATH)/lib/libpress.a \
	$(PSYQ_PATH)/lib/libgpu.a \
	$(PSYQ_PATH)/lib/libgs.a \
	$(PSYQ_PATH)/lib/libgte.a \
	$(PSYQ_PATH)/lib/libcd.a \
	$(PSYQ_PATH)/lib/libetc.a \
	$(PSYQ_PATH)/lib/libsn.a \
	$(PSYQ_PATH)/lib/libsnd.a \
	$(PSYQ_PATH)/lib/libspu.a \
	$(PSYQ_PATH)/lib/libmath.a \
	$(PSYQ_PATH)/lib/libcomb.a \
	$(PSYQ_PATH)/lib/libcard.a \
	$(PSYQ_PATH)/lib/libtap.a \
	$(PSYQ_PATH)/lib/libsio.a \
	$(PSYQ_PATH)/lib/libpad.a \
	$(PSYQ_PATH)/lib/libc2.a \
	$(PSYQ_PATH)/lib/libapi.a \
	$(PSYQ_PATH)/lib/extra.a

CFLAGS := -g -O2 -G0 -ffreestanding -nostdlib -mno-unaligned-access -Wall -Wundef -Werror-implicit-function-declaration -Wwrite-strings $(INC)
CFLAGS += -DMRB_NO_FLOAT -DMRB_NO_STDIO
LDFLAGS := -g -T linker.ld
MRUBY_LDFLAGS = -L$(PSYQ_PATH)/lib -L$(MRUBY_PATH)/build/playstation/lib

SRC := main.c input.c io.c renderer.c sokoban_rb.c

OBJ := $(SRC:.c=.o)

all: $(ISO)

sokoban.mrb: sokoban.rb
	$(MRUBY_PATH)/build/host/bin/mrbc sokoban.rb
	ruby bin2c.rb sokoban.mrb

$(ISO): sokoban.mrb $(EXE)
	mkpsxiso -y mkpsxiso.xml

$(EXE): $(ELF)
	$(OBJCOPY) -O binary $< $@

$(ELF): $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $^ $(MRUBY_LDFLAGS) $(LIB) -lgcc

.c.o: $(SRC)
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f $(ELF) $(EXE) $(OBJ) $(ISO) sokoban.cue sokoban.mrb sokoban_rb.h sokoban_rb.c

install:
	cp sokoban.bin /mnt/c/Users/vbihl/Desktop/PSX_EXAMPLES

.PHONY: all clean install
