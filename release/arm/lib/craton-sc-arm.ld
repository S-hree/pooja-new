OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)

STARTUP(craton-crt0.o)

MEMORY
{
  ARM_CODE : ORIGIN = 0x50000040, LENGTH = 8M
  ARM_DATA : ORIGIN = 0x50800000, LENGTH = 112M
  SHARED : ORIGIN = 0x53800000, LENGTH = 8M
  GP_SRAM : ORIGIN = 0x41140800, LENGTH = 94K
}

__use_calculated_tsf = 0;

SECTIONS
{
  .code : {
    _stext = ABSOLUTE(.);
    *(.text)
    *(.text.*)
	__init_array_start = ABSOLUTE(.);
	*(.init_array)
	__init_array_end = ABSOLUTE(.);
    *(.rodata*)
    *(.stub)
    *(.ARM.extab*)
  } >ARM_CODE

  __exidx_start = .;
  .ARM.exidx : { *(.ARM.exidx*) } >ARM_CODE
  __exidx_end = .;
  _etext = .;
  .data    :
  {
    _sdata = ABSOLUTE(.);
    *(.data)
    *(.data.*);
    *(.gnu.linkonce.d*)
    .   =  ALIGN(4) ;
    _edata = ABSOLUTE(.);
    /*
     * Boot parameters: values of addr&entries will be evaluated early
     * at OS entry point crt0.S
     * Keep .init section below .data section to avoid data corruption
     */
    *(init.boot_param);
  } >ARM_DATA AT>ARM_CODE
  __data_loadaddr = LOADADDR(.data);

  __bss_start__ = .;
  .sbss      :
  {
    *(.dynsbss)
    *(.sbss)
    *(.sbss.*)
    *(.scommon)
  } >ARM_DATA
  .bss       :
  {
   *(.dynbss)
   *(.bss)
   *(.bss.*)
   *(COMMON)
   /* Align here to ensure that the .bss section occupies space up to
      _end.  Align after .bss to ensure correct alignment even if the
      .bss section disappears because there are no input sections.  */
   . = ALIGN(32 / 8);
  } >ARM_DATA
  . = ALIGN(32 / 8);

  PROVIDE (end = .);

  .stack     :
  {
    _init_stack_bottom = ABSOLUTE(.) ;
    .   =  ALIGN(8) ;
    .   += 2048;
    _init_sp =  . - 16 ;
    _init_stack_top = ABSOLUTE(.) ;

    _stack_bottom = ABSOLUTE(.) ;

    /* Allocate room for stack. This must be big enough for the IRQ, FIQ, and
       SYS stack if nested interrupts are enabled.  */
    .   =  ALIGN(8) ;
    .   += 2048;
    _sp =  . - 16 ;
    _stack_top = ABSOLUTE(.) ;
  } >ARM_DATA

  /* The heap section must be the last section in ARM_DATA region. */
  .heap :
  {
    _heap_start = ABSOLUTE(.) ;
    . = ALIGN(4) ;
    /* The heap size will be set to the size remaining in the ARM_DATA region.
       ORIGIN(ARM_DATA) = Start of ARM_DATA region.
       LENGTH(ARM_DATA) = Size of the ARM_DATA region. */
    . = ORIGIN(ARM_DATA) + LENGTH(ARM_DATA);
    _heap_end = ABSOLUTE(.) ;
  } >ARM_DATA

  _bss_end__ = . ; __bss_end__ = . ;

  .gp_sram_user_shm (NOLOAD):
   {

     _gp_sram_start = ABSOLUTE(.) ;
     _gp_sram_user_shared_data = (ORIGIN(GP_SRAM) + 2k);
     *(.gp_sram_user_shm)
     _gp_sram_end = .;
   } > GP_SRAM

   .gp_sram_log (NOLOAD):
   {
     _gp_sram_log_start = .;
     *(.gp_sram_log)
     _gp_sram_log_end = .;
   } > GP_SRAM

   .gp_sram_user_shared_exception (NOLOAD):
   {
     _gp_sram_user_shared_exception_data_start = .;
     *(.gp_sram_user_shared_exception)
     _gp_sram_user_shared_exception_data_end = .;
   } > GP_SRAM

   .gp_sram_user_shared_timer (NOLOAD):
   {
     _gp_sram_user_shared_timer_data_start = .;
     *(.gp_sram_timer)
     _gp_sram_user_shared_timer_data_end = .;
   } > GP_SRAM

   .gp_sram_user_shared_boot_params (NOLOAD):
   {
     _gp_sram_user_shared_boot_params_start = .;
     *(.gp_boot_params)
     _gp_sram_user_shared_boot_params_end = .;
   } > GP_SRAM

   .gp_sram_user_shared_mibconf_params (NOLOAD):
   {
     .gp_sram_user_shared_mibconf_params_start = .;
     *(.gp_mibconf_params)
     .gp_sram_user_shared_mibconf_params_end = .;
   } > GP_SRAM

   .gp_sram_reset_reason (NOLOAD):
   {
     _gp_sram_reset_reason_start = .;
     *(.gp_sram_reset_reason)
     _gp_sram_reset_reason_end = .;
   } > GP_SRAM
   
   /*
    * Powerup reason.
    * Set by Watchdog Exception Handlers. Checked and cleared
    * after powerup.
    * Starts 32 bytes before end of SRAM, length 16 bytes
    */
   .gp_sram_powerup_reason (NOLOAD):
   {
     _gp_sram_powerup_reason_start = (ORIGIN(GP_SRAM) + LENGTH(GP_SRAM) - 32);
     /* Must be empty */
     _gp_sram_powerup_reason_end = (_gp_sram_powerup_reason_start + 16);
   } > GP_SRAM

   /* Saving area for CMOCKA test.   16 bytes */
   /* Currently for WD resrtart test verification */
   .gp_sram_cmocka_bss (NOLOAD):
   {
     _gp_sram_cmocka_start = .;
     /* Must be empty */
     _gp_sram_cmocka_end = (ORIGIN(GP_SRAM) + LENGTH(GP_SRAM));
   } > GP_SRAM
  .internal_sram (NOLOAD): { *(.internal_sram) } > GP_SRAM


  /* Shared arc control was moved so it will be located before imq.control.
   * This was done due to issues with CMOCKA testing (AT-4488)
   * If moved it should be kept in front of imq.control. */
  .shared.arc.control (NOLOAD): { *(.shared.arc.control) } > SHARED

  .imq.control (NOLOAD):
  {
    *(.imq.control)
  } >SHARED

  .imq.data (NOLOAD):
  {
    _imq_data_start = .;
    . += 4M;
    _imq_data_end = .;
  } >SHARED

  /* System info presented via CLI or on Exception command */
  .system_info (NOLOAD):
  {
    *(.system_info)
  } > SHARED


  _end = .; __end__ = . ;
}
