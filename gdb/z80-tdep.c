/* Target-dependent code for Z80 (T80), for GDB

   This file is NOT part of GDB. */


/* Created by Leonardo Etcheverry, letcheve@fing.edu.uy */

#include "defs.h"
#include "frame.h"
#include "frame-unwind.h"
#include "frame-base.h"
#include "trad-frame.h"
#include "gdbcmd.h"
#include "gdbcore.h"
#include "gdbtypes.h"
#include "inferior.h"
#include "symfile.h"
#include "arch-utils.h"
#include "regcache.h"
#include "gdb_string.h"
#include "dis-asm.h"
#include "z80-tdep.h"

/* Constants */

extern initialize_file_ftype _initialize_z80_tdep;

static const struct frame_unwind z80_frame_unwind =
{
  NORMAL_FRAME,
  z80_frame_this_id,
  z80_frame_prev_register,
  NULL,
  default_frame_sniffer
};

/* Initialize the gdbarch struct for the Z80 arch */

static struct gdbarch *
z80_gdbarch_init (struct gdbarch_info info, struct gdbarch_list *arches)
{
  struct gdbarch *gdbarch;
  struct gdbarch_tdep *tdep;
  struct gdbarch_list *best_arch;
  
  tdep    = XMALLOC (struct gdbarch_tdep);
  gdbarch = gdbarch_alloc (&info, tdep);

  // unsigned long mach = gdbarch_bfd_arch_info (arch)->mach;

  tdep->void_type = arch_type (gdbarch, TYPE_CODE_VOID, 1, "void");
  tdep->func_void_type = make_function_type (tdep->void_type, NULL);
  tdep->pc_type = arch_type (gdbarch, TYPE_CODE_PTR, 2, NULL);
  TYPE_TARGET_TYPE (tdep->pc_type) = tdep->func_void_type;
  TYPE_UNSIGNED (tdep->pc_type) = 1;

  set_gdbarch_short_bit (gdbarch, TARGET_CHAR_BIT);
  set_gdbarch_int_bit (gdbarch, 2 * TARGET_CHAR_BIT);
  set_gdbarch_long_bit (gdbarch, 4 * TARGET_CHAR_BIT);
  set_gdbarch_ptr_bit (gdbarch, 2 * TARGET_CHAR_BIT);
  set_gdbarch_addr_bit (gdbarch, 16);

  /* TODO: what if I don't want to deal with floats? */
  set_gdbarch_float_bit (gdbarch, 4 * TARGET_CHAR_BIT);
  set_gdbarch_double_bit (gdbarch, 4 * TARGET_CHAR_BIT);
  set_gdbarch_long_double_bit (gdbarch, 4 * TARGET_CHAR_BIT);
  set_gdbarch_float_format (gdbarch, floatformats_ieee_single);
  set_gdbarch_double_format (gdbarch, floatformats_ieee_single);
  set_gdbarch_long_double_format (gdbarch, floatformats_ieee_single);
  
  set_gdbarch_num_regs (gdbarch, Z80_NUM_REGS);
  set_gdbarch_sp_regnum (gdbarch, Z80_SP_REGNUM);
  set_gdbarch_pc_regnum (gdbarch, Z80_PC_REGNUM);

  set_gdbarch_register_name (gdbarch, z80_register_name);
  set_gdbarch_register_type (gdbarch, z80_register_type);

  /* TODO: can I use  this for IO? */
  set_gdbarch_num_pseudo_regs (gdbarch, Z80_NUM_PSEUDO_REGS);
  set_gdbarch_pseudo_register_read (gdbarch, z80_pseudo_register_read);
  set_gdbarch_pseudo_register_write (gdbarch, z80_pseudo_register_write);

  set_gdbarch_return_value (gdbarch, z80_return_value);
  set_gdbarch_print_insn (gdbarch, print_insn_z80);

  set_gdbarch_push_dummy_call (gdbarch, z80_push_dummy_call);

  set_gdbarch_skip_prologue (gdbarch, z80_skip_prologue);
  set_gdbarch_inner_than (gdbarch, core_addr_lessthan); // falling stack
  set_gdbarch_unwind_pc (gdbarch, z80_unwind_pc);

  set_gdbarch_breakpoint_from_pc (gdbarch, z80_breakpoint_from_pc);

  frame_unwind_append_unwinder (gdbarch, &z80_frame_unwind);

  
  return gdbarch;
}

void
_initialize_z80_tdep (void)
{
  register_gdbarch_init (bfd_arch_z80, z80_gdbarch_init);
}





static CORE_ADDR
z80_skip_prologue (struct gdbarch *gdbarch, CORE_ADDR pc)
{
  CORE_ADDR func_addr, func_end;

  if (!find_pc_partial_function (pc, NULL, &func_addr, &func_end))
    return pc;
  else
    {
      printf("LLL: find_pc_partial_function found something\n");
      return pc;
    }

  // LLL: avr example

//   CORE_ADDR func_addr, func_end;
//   CORE_ADDR post_prologue_pc;
// 
//   /* See what the symbol table says */
// 
//   if (!find_pc_partial_function (pc, NULL, &func_addr, &func_end))
//     return pc;
// 
//   post_prologue_pc = skip_prologue_using_sal (gdbarch, func_addr);
//   if (post_prologue_pc != 0)
//     return max (pc, post_prologue_pc);
// 
//   {
//     CORE_ADDR prologue_end = pc;
//     struct avr_unwind_cache info = {0};
//     struct trad_frame_saved_reg saved_regs[AVR_NUM_REGS];
// 
//     info.saved_regs = saved_regs;
//     
//     /* Need to run the prologue scanner to figure out if the function has a
//        prologue and possibly skip over moving arguments passed via registers
//        to other registers.  */
//     
//     prologue_end = z80_scan_prologue (gdbarch, func_addr, func_end, &info);
//     
//     if (info.prologue_type != AVR_PROLOGUE_NONE)
//       return prologue_end;
//   }
// 
//   /* Either we didn't find the start of this function (nothing we can do),
//      or there's no line info, or the line after the prologue is after
//      the end of the function (there probably isn't a prologue). */
// 
//   return pc;
}

/* Register number to name mapping */

static const char *
z80_register_name (struct gdbarch *gdbarch, int regnum)
{

  //TODO: move this to gdbarch_tdep? (see  i386)
//  static const char * const register_names[] = {
//    "a", "b", "c", "d", "e", "h", "l", "ix", "iy", "i", "r", "f", "sp", "pc"
//  };

  // taken from qemu-z80 target reg mapping
  static const char * const register_names[] = {
    "a", "f", "bc", "de", "hl", "ix", "iy", "sp", "i", "r",
    "ax", "fx", "bcx", "dex", "hlx", "pc"
  };


  if (regnum <0 || (regnum >= (sizeof (register_names) / sizeof (*register_names))))
    return NULL;

  return register_names[regnum];
}

/* Return the GDB type object for the data type
   of data in register N.  */

static struct type *
z80_register_type (struct gdbarch *gdbarch, int reg_nr)
{
#define R_A     0
#define R_F     1

#define R_BC    2
#define R_DE    3
#define R_HL    4
#define R_IX    5
#define R_IY    6
#define R_SP    7

#define R_I     8
#define R_R     9

#define R_AX    10
#define R_FX    11
#define R_BCX   12
#define R_DEX   13
#define R_HLX   14

  switch (reg_nr)
    {
    case R_A:
    case R_F:
    case R_I:
    case R_R:
    case R_AX:
    case R_FX:
      return builtin_type (gdbarch)->builtin_uint8;
    default:
      return builtin_type (gdbarch)->builtin_uint16;
    }

  if ((reg_nr == Z80_PC_REGNUM)        || 
      (reg_nr == Z80_PSEUDO_PC_REGNUM) ||
      (reg_nr == Z80_SP_REGNUM))
    return gdbarch_tdep (gdbarch)->pc_type; 




  
  return builtin_type (gdbarch)->builtin_uint8;
}

static void
z80_pseudo_register_read (struct gdbarch *gdbarch, struct regcache *regcache,
                          int regnum, gdb_byte *buf)
{
  ULONGEST val;

  switch (regnum)
    {
    case Z80_PSEUDO_PC_REGNUM:
      regcache_raw_read_unsigned (regcache, Z80_PC_REGNUM, &val);
      // val >>= 1;
      store_unsigned_integer (buf, 2, gdbarch_byte_order (gdbarch), val);
      break;
    default:
      internal_error (__FILE__, __LINE__, _("invalid regnum"));
    }
}


static void
z80_pseudo_register_write (struct gdbarch *gdbarch, struct regcache *regcache,
                           int regnum, const gdb_byte *buf)
{
  ULONGEST val;

  switch (regnum)
    {
    case Z80_PSEUDO_PC_REGNUM:
      val = extract_unsigned_integer (buf, 2, gdbarch_byte_order (gdbarch));
      // val <<= 1;
      regcache_raw_write_unsigned (regcache, Z80_PC_REGNUM, val);
      break;
    default:
      internal_error (__FILE__, __LINE__, _("invalid regnum"));
    }
}

static enum return_value_convention
z80_return_value (struct gdbarch *gdbarch, struct type *func_type,
		  struct type *valtype, struct regcache *regcache,
		  gdb_byte *readbuf, const gdb_byte *writebuf)
{
  //TODO: choose one
  return RETURN_VALUE_REGISTER_CONVENTION;
}


static const unsigned char *
z80_breakpoint_from_pc (struct gdbarch *gdbarch, CORE_ADDR * pcptr, int *lenptr)
{
  printf("entered %s\n", __FUNCTION__);
  static const unsigned char z80_break_insn [] = {0xCF}; /* TODO!!: Bytes de inst RST 08 */
  *lenptr = sizeof (z80_break_insn);

  return z80_break_insn;
}

static CORE_ADDR
z80_push_dummy_call (struct gdbarch *gdbarch, struct value *function,
                     struct regcache *regcache, CORE_ADDR bp_addr,
                     int nargs, struct value **args, CORE_ADDR sp,
                     int struct_return, CORE_ADDR struct_addr)
{
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
  int i;
  unsigned char buf[3];
  int call_length = gdbarch_tdep (gdbarch)->call_length;
  //  CORE_ADDR return_pc = z80_convert_iaddr_to_raw (bp_addr);
  // int regnum = Z80_ARGN_REGNUM;
  struct stack_item *si = NULL;

  return bp_addr; //TODO: fix!!!
}

static CORE_ADDR
z80_unwind_pc (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  CORE_ADDR pc;
  
  gdb_byte buf[2];
  frame_unwind_register(next_frame, Z80_PC_REGNUM, buf);
  pc = extract_typed_address(buf, builtin_type (gdbarch)->builtin_func_ptr);
  return pc;
}

static void
z80_frame_this_id (struct frame_info *next_frame, void **this_cache,
		       struct frame_id *this_id)
{

  printf("entered %s\n", __FUNCTION__);
  return;
//   struct z80_frame_cache *cache =
//     z80_frame_cache (next_frame, this_cache);
// 
//   /* This marks the outermost frame.  */
//   if (cache->base == 0)
//     return;
// 
//   (*this_id) = frame_id_build (cache->base, cache->pc);
}


static struct value *
z80_frame_prev_register (struct frame_info *this_frame,
				 void **this_cache, int regnum)
{
  printf("entered %s\n", __FUNCTION__);
  if (*this_cache)
    return *this_cache;

  return NULL;

//   struct z80_frame_cache *cache =
//     z80_frame_cache (this_frame, this_cache);
// 
//   if (cache->frameless_p)
//     {
//       return trad_frame_get_prev_register (this_frame, cache->saved_regs, regnum);
//     }
//   else
//     return trad_frame_get_prev_register (this_frame, cache->saved_regs,
//					 regnum);

}
