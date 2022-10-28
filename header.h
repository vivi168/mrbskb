extern unsigned long __SN_ENTRY_POINT;
extern unsigned long __exe_start__;
extern unsigned long __exe_size__;

typedef struct psx_header {
	char sig[8];
	char pad1[8];
	unsigned long pc;
	unsigned long gp;
	unsigned long load_addr;
	unsigned long load_size;
	char pad2[2016];
} psx_header;

const psx_header hdr __attribute__((section(".hdr"))) = {
	.sig = "PS-X EXE",
	.pc = (unsigned long)&__SN_ENTRY_POINT,
	.load_addr = (unsigned long)&__exe_start__,
	.load_size = (unsigned long)&__exe_size__,
};
