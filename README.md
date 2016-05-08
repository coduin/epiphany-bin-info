# Extract binary information for the Epiphany

This program is a tool that extracts information on code placement and binary size for Epiphany coprocessor binaries. It is similar to `readelf` and `objdump` but it is tailored to the Epiphany platform because it makes a distinction between local memory and external memory and will show the size of every C function in the binary. This allows the user to find which functions might be too large to store in local memory.  Issue `$ make` from the command-line to build the tool. Issue

    epiphany-bin-info -h

for information on how to use the tool.

### Example output

	$ ./epiphany-bin-info e_memorybench.elf 
	Total size in local memory:    0x00003854 = 14 KB
	Total size in external memory: 0x0000a318 = 40 KB
	
	$ ./epiphany-bin-info -s e_memorybench.elf 
	Total size in local memory:    0x00003854 = 14 KB
	Total size in external memory: 0x0000a318 = 40 KB
	
	Symbol table contains 318 symbols.
	Showing only global symbols. Highlighting symbol sizes larger than 128 Bytes (local) or 1024 Bytes (external).
	VALUE        SIZE TYPE    SEC NAME
	0x00000000      4 FUNC      1 _start
	0x00000028     40 OBJECT    2 e_group_config
	0x00000050      8 OBJECT    3 e_emem_config
	0x00000110     24 OBJECT    6 _dma_copy_descriptor_
	0x00000128      0 NOTYPE   10 _init
	0x00000150    552 FUNC     11 main
	0x00000378    278 FUNC     11 _epiphany_start
	0x00000598     74 FUNC     11 transfer
	0x000005e4     82 FUNC     11 reverse_transfer
	0x00000638    214 FUNC     11 dma_copy
	0x00000710    228 FUNC     11 dma_transfer
	0x000007f4    152 FUNC     11 check_buffers
	0x0000088c    130 FUNC     11 get_remote_addr
	0x00000910   3360 FUNC     11 measure
	0x00001630     30 FUNC     11 bsp_end
	0x00001650     14 FUNC     11 bsp_nprocs
	0x00001660     14 FUNC     11 bsp_pid
	0x00001670     16 FUNC     11 ebsp_host_time
	0x00001680    344 FUNC     11 bsp_sync
	0x000017d8     26 FUNC     11 ebsp_barrier
	0x000017f4     62 FUNC     11 ebsp_host_sync
	0x00001834     30 FUNC     11 _write_syncstate
	0x00001854     82 FUNC     11 _int_isr
	0x000018a8    598 FUNC     11 ebsp_memcpy
	0x00001b00    122 FUNC     11 _prepare_descriptor
	0x00001b7c    234 FUNC     11 ebsp_dma_push
	0x00001c68    150 FUNC     11 _dma_interrupt
	0x00001d00     10 FUNC     11 ebsp_dma_wait
	0x00001d0c     48 FUNC     11 ebsp_raw_time
	0x00001d3c     26 FUNC     11 e_coreid_from_coords
	0x00001d58    108 FUNC     11 e_get_global_address
	0x00001dc4    176 FUNC     11 e_dma_copy
	0x00001e74    164 FUNC     11 e_dma_start
	0x00001f18     18 FUNC     11 e_irq_attach
	0x00001f2c     20 FUNC     11 e_irq_global_mask
	0x00001f40    118 FUNC     11 e_irq_mask
	0x00001fb8    206 FUNC     11 e_barrier
	0x00002088    314 FUNC     11 e_barrier_init
	0x000021c4     78 FUNC     11 e_mutex_lock
	0x00002214     30 FUNC     11 e_mutex_unlock
	0x00002234    112 FUNC     11 e_reg_read
	0x000022a4    142 FUNC     11 e_reg_write
	0x00002334      8 FUNC     11 e_get_coreid
	0x0000233c     76 FUNC     11 e_dma_busy
	0x00002388    334 FUNC     11 __modsi3
	0x000024d8    390 FUNC     11 __divsi3
	0x00002660    386 FUNC     11 __udivsi3
	0x000027e8    320 FUNC     11 __umodsi3
	0x00002928     78 FUNC     11 _sbrk
	0x000029bc      0 NOTYPE   12 _fini
	0x000029e4      0 OBJECT   14 __DTOR_END__
	0x000029f0      0 OBJECT   16 __dso_handle
	0x000029f8     32 OBJECT   16 dma_data_size
	0x00002a18      4 OBJECT   17 samples
	0x00002a18      0 OBJECT   16 __TMC_END__
	0x00002be8      0 NOTYPE   18 _edata
	0x00002be8      0 NOTYPE   18 __bss_start
	0x00002bf0      4 OBJECT   18 _PathLocale
	0x00002bf4      4 OBJECT   18 __mlocale_changed
	0x00002bf8      4 OBJECT   18 __nlocale_changed
	0x00002c00      4 OBJECT   18 __malloc_top_pad
	0x00002c08     40 OBJECT   18 __malloc_current_mallinfo
	0x00002c30      4 OBJECT   18 __malloc_max_sbrked_mem
	0x00002c34      4 OBJECT   18 __malloc_max_total_mem
	0x00002c40   2048 OBJECT   18 local_buffer
	0x00003440      4 OBJECT   18 p
	0x00003444      4 OBJECT   18 n
	0x00003448   1028 OBJECT   18 timings
	0x0000384c      4 OBJECT   18 extmem_buffer
	0x00003850    200 OBJECT   18 coredata
	0x00003918      4 OBJECT   18 errno
	0x00003920      0 NOTYPE   18 _end
	0x00003920      0 NOTYPE   18 end
	0x00007ff0      0 NOTYPE   29 __stack
	0x8e000000     58 FUNC      7 exit
	0x8e00003c      4 OBJECT    7 _global_impure_ptr
	0x8e000040    228 FUNC      7 _vsnprintf_r
	0x8e000124     48 FUNC      7 vsnprintf
	0x8e000154    322 FUNC      7 __call_exitprocs
	0x8e000298   8950 FUNC      7 _svfprintf_r
	0x8e0028e8   6220 FUNC      7 _dtoa_r
	0x8e004134     96 FUNC      7 _setlocale_r
	0x8e004194     12 FUNC      7 __locale_charset
	0x8e0041a0     16 FUNC      7 __locale_mb_cur_max
	0x8e0041b0     12 FUNC      7 __locale_msgcharset
	0x8e0041bc      6 FUNC      7 __locale_cjk_lang
	0x8e0041c4     12 FUNC      7 _localeconv_r
	0x8e0041d0     24 FUNC      7 setlocale
	0x8e0041e8     12 FUNC      7 localeconv
	0x8e0041f4   1836 FUNC      7 _malloc_r
	0x8e004920    194 FUNC      7 memchr
	0x8e0049e4    188 FUNC      7 memcpy
	0x8e004aa0      4 FUNC      7 __malloc_lock
	0x8e004aa4      4 FUNC      7 __malloc_unlock
	0x8e004aa8    124 FUNC      7 _Balloc
	0x8e004b24     22 FUNC      7 _Bfree
	0x8e004b3c    300 FUNC      7 __multadd
	0x8e004c68    268 FUNC      7 __s2b
	0x8e004d74     96 FUNC      7 __hi0bits
	0x8e004dd4    120 FUNC      7 __lo0bits
	0x8e004e4c     52 FUNC      7 __i2b
	0x8e004e80    522 FUNC      7 __multiply
	0x8e00508c    232 FUNC      7 __pow5mult
	0x8e005174    256 FUNC      7 __lshift
	0x8e005274     72 FUNC      7 __mcmp
	0x8e0052bc    410 FUNC      7 __mdiff
	0x8e005458     28 FUNC      7 __ulp
	0x8e005474    274 FUNC      7 __b2d
	0x8e005588    204 FUNC      7 __d2b
	0x8e005654    126 FUNC      7 __ratio
	0x8e0056d4    104 FUNC      7 _mprec_log10
	0x8e00573c     88 FUNC      7 __copybits
	0x8e005794    110 FUNC      7 __any_on
	0x8e005818    200 OBJECT    7 __mprec_tens
	0x8e0058e0     40 OBJECT    7 __mprec_tinytens
	0x8e005908     40 OBJECT    7 __mprec_bigtens
	0x8e005930     78 FUNC      7 _sbrk_r
	0x8e005980    126 FUNC      7 __fpclassifyd
	0x8e005a00    154 FUNC      7 strcmp
	0x8e005a9c    128 FUNC      7 strlen
	0x8e005b1c    444 FUNC      7 __ssprint_r
	0x8e005cd8   4378 FUNC      7 _svfiprintf_r
	0x8e006f80    222 FUNC      7 _calloc_r
	0x8e007060    248 FUNC      7 _malloc_trim_r
	0x8e007158    736 FUNC      7 _free_r
	0x8e007438    220 FUNC      7 memmove
	0x8e007514    218 FUNC      7 memset
	0x8e0075f0   1250 FUNC      7 _realloc_r
	0x8e007ad4     54 FUNC      7 cleanup_glue
	0x8e007b0c    232 FUNC      7 _reclaim_reent
	0x8e007bf4     14 FUNC      7 __errno
	0x8e007c08      4 OBJECT    8 _impure_ptr
	0x8e008078      4 OBJECT    8 __mb_cur_max
	0x8e0080d8   1032 OBJECT    8 __malloc_av_
	0x8e0084e0      4 OBJECT    8 __malloc_sbrk_base
	0x8e0084e4      4 OBJECT    8 __malloc_trim_threshold
	0x8e0084e8      0 NOTYPE    9 _exit
	0x8e00884c    100 FUNC      9 __adddf3
	0x8e0088b0    112 FUNC      9 __subdf3
	0x8e008920    690 FUNC      9 __muldf3
	0x8e008bd4    424 FUNC      9 __divdf3
	0x8e008d7c    124 FUNC      9 __eqdf2
	0x8e008df8    124 FUNC      9 __nedf2
	0x8e008e74    130 FUNC      9 __gtdf2
	0x8e008ef8    130 FUNC      9 __gedf2
	0x8e008f7c    124 FUNC      9 __ltdf2
	0x8e008ff8    172 FUNC      9 __floatsidf
	0x8e0090a4    164 FUNC      9 __fixdfsi
	0x8e009148     24 OBJECT    9 __thenan_df
	0x8e009160    160 FUNC      9 __muldi3
	0x8e009200     90 FUNC      9 __clzsi2
	0x8e00925c    492 FUNC      9 __pack_d
	0x8e009448    226 FUNC      9 __unpack_d
	0x8e00952c    252 FUNC      9 __fpcmp_parts_d
	0x8e009628    256 OBJECT    9 __clz_tab
	0x8e009728    628 FUNC     30 bsp_begin
	0x8e00999c    134 FUNC     30 bsp_time
	0x8e009a24     96 FUNC     30 ebsp_send_string
	0x8e009a84    196 FUNC     30 bsp_abort
	0x8e009b48    184 FUNC     30 ebsp_message
	0x8e009c00    330 FUNC     30 _malloc
	0x8e009d4c    124 FUNC     30 _free
	0x8e009dc8    332 FUNC     30 _init_malloc_state
	0x8e009f14    252 FUNC     30 _init_local_malloc
	0x8e00a010    102 FUNC     30 ebsp_ext_malloc
	0x8e00a078    222 FUNC     30 ebsp_malloc
	0x8e00a158    370 FUNC     30 ebsp_free
	0x8e00a2d0     71 OBJECT   31 err_allocation
	0x8f000000      0 NOTYPE   18 __heap_start
	0x00000000      0 NOTYPE  ABS __CORE_NUM_
	0x00000004      0 NOTYPE  ABS __NUM_ROWS_IN_CHIP_
	0x00000004      0 NOTYPE  ABS __NUM_COLS_IN_CHIP_
	0x00000008      0 NOTYPE  ABS __FIRST_CORE_COL_
	0x00000008      0 NOTYPE  ABS __CORE_COL_
	0x00000020      0 NOTYPE  ABS __CORE_ROW_
	0x00000020      0 NOTYPE  ABS __FIRST_CORE_ROW_
	0x00001000      0 NOTYPE  ABS __HALF_BANK_SIZE_
	0x00007ff0      0 NOTYPE  ABS __stack_start_
	0x00080000      0 NOTYPE  ABS __HEAP_SIZE_FOR_CORE_
	0x00100000      0 NOTYPE  ABS __PROG_SIZE_FOR_CORE_
	0x8e000000      0 NOTYPE  ABS __SHARED_DRAM_
	0x8f07fffc      0 NOTYPE  ABS __heap_end
	
