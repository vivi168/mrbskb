TOOLCHAIN := mipsel-none-elf-

PSYQ := /home/vbihl/PSYQSDK

ifeq ($(PSYQ),)
$(error PSYQ SDK path is not set)
endif

ifeq ($(MRUBY),)
$(error MRUBY path is not set)
endif

ELF := MAIN.ELF
EXE := MAIN.EXE
ISO := sokoban.bin

CC := $(TOOLCHAIN)gcc
LD := $(TOOLCHAIN)gcc
OBJCOPY := $(TOOLCHAIN)objcopy

INC := -I$(PSYQ)/include -I$(MRUBY)/include -I$(MRUBY)/build/playstation/include

LIB := \
	$(MRUBY)/build/playstation/lib/libmruby.a \
	$(MRUBY)/build/playstation/lib/libmruby_core.a \
	$(PSYQ)/lib/libcard.a \
	$(PSYQ)/lib/libpress.a \
	$(PSYQ)/lib/libgpu.a \
	$(PSYQ)/lib/libgs.a \
	$(PSYQ)/lib/libgte.a \
	$(PSYQ)/lib/libcd.a \
	$(PSYQ)/lib/libetc.a \
	$(PSYQ)/lib/libsn.a \
	$(PSYQ)/lib/libsnd.a \
	$(PSYQ)/lib/libspu.a \
	$(PSYQ)/lib/libmath.a \
	$(PSYQ)/lib/libcomb.a \
	$(PSYQ)/lib/libcard.a \
	$(PSYQ)/lib/libtap.a \
	$(PSYQ)/lib/libsio.a \
	$(PSYQ)/lib/libpad.a \
	$(PSYQ)/lib/libc2.a \
	$(PSYQ)/lib/libapi.a \
	$(PSYQ)/lib/extra.a

CFLAGS := -g -G0 -ffreestanding -nostdlib -Wall -Wundef -Werror-implicit-function-declaration -Wwrite-strings $(INC)
CFLAGS += -DMRB_NO_FLOAT -DMRB_NO_STDIO
LDFLAGS := -g -T linker.ld
MRUBY_LDFLAGS = -L$(PSYQ)/lib -L$(MRUBY)/build/playstation/lib

SRC := level.c lvllut.c main.c input.c io.c renderer.c sokoban_rb.c

OBJ := $(SRC:.c=.o)

all: $(ISO)

sokoban_rb.c: sokoban.rb
	mrbc sokoban.rb
	ruby bin2c.rb sokoban.mrb

$(ISO): $(EXE)
	mkpsxiso -y mkpsxiso.xml

$(EXE): $(ELF)
	$(OBJCOPY) -O binary $< $@

$(ELF): $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $^ $(MRUBY_LDFLAGS) $(LIB) -lgcc

.c.o: $(SRC)
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f $(ELF) $(EXE) $(OBJ) $(ISO)

install:
	cp sokoban.bin /mnt/c/Users/vbihl/Desktop/PSX_EXAMPLES

.PHONY: all clean install
