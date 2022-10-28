ENTRY(__SN_ENTRY_POINT)

MEMORY
{
	ram (rwx) : ORIGIN = 0x80010000, LENGTH = 2M - 64K
}

SECTIONS
{
	.hdr : ALIGN(8) {
		KEEP(*(.hdr .hdr.*))
	} > ram

	.text : ALIGN(8) {
		__exe_start__ = .;
		__text_start__ = .;
		*(.text .text.*)
		__text_end__ = .;
		__text_size__ = __text_start__ - __text_end__;
	} > ram

	.rodata : ALIGN(8) {
		__rdata_start__ = .;
		*(.rdata .rdata.* .rodata .rodata.*)
		__rdata_end__ = .;
		__rdata_size__ = __rdata_end__ - __rdata_start__;
	} > ram

	.ctors : ALIGN(8) {
		__ctors_start__ = .;
		KEEP(*(.ctors))
		KEEP(*(SORT(.ctors.*)))
		__ctors_end__ = .;
		__ctors_size__ = __ctors_end__ - __ctors_start__;
	} > ram

	.dtors : ALIGN(8) {
		__dtors_start__ = .;
		KEEP(*(.dtors))
		KEEP(*(SORT(.dtors.*)))
		__dtors_end__ = .;
		__dtors_size__ = __dtors_end__ - __dtors_start__;
	} > ram

	.data : ALIGN(8) {
		__sdata_start__ = .;
		*(.sdata .sdata.*)
		__sdata_end__ = .;
		__sdata_size__ = __sdata_end__ - __sdata_start__;
		__data_start__ = .;
		*(.data .data.*)
		SORT(CONSTRUCTORS)
		. = ALIGN(2048);
		__data_end__ = .;
		__exe_end__ = .;
		__data_size__ = __data_end__ - __data_start__;
	} > ram

	__exe_size__ = __exe_end__ - __exe_start__;

	.sbss : ALIGN(64) {
		__sbss_start__ = .;
		*(.sbss .sbss.*)
		__sbss_end__ = .;
		__sbss_size__ = __sbss_end__ - __sbss_start__;
	} > ram

	.bss : ALIGN(64) {
		__bss_start__ = .;
		*(.bss .bss.*)
		*(COMMON)
		. = ALIGN(64);
		__bss_end__ = .;
		__bss_size__ = __bss_end__ - __bss_start__;
	} > ram
}
