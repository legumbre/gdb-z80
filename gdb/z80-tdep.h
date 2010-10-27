/* Header for target-dependent code for Z80 (T80), for GDB

   This file is NOT part of GDB. */


/* 2010, created by Leonardo Etcheverry, letcheve@fing.edu.uy */

/* Constants */
enum
  {
    Z80_NUM_REGS = 12 + 1 /*SP*/ + 1 /*PC*/,
    Z80_NUM_REG_BYTES = 12 + 1 + 2 /*SP*/ + 2 /*PC*/,
    Z80_PC_REGNUM = 13,
    Z80_SP_REGNUM = 12,
    Z80_PSEUDO_PC_REGNUM = 13,
    Z80_NUM_PSEUDO_REGS = 1,
  };

struct gdbarch_tdep
{
  int call_length;
  /* Type for void.  */
  struct type *void_type;
  /* Type for a function returning void.  */
  struct type *func_void_type;
  /* Type for a pointer to a function.  Used for the type of PC.  */
  struct type *pc_type;
};

static struct gdbarch *
z80_gdbarch_init (struct gdbarch_info info, struct gdbarch_list *arches);

void
_initialize_z80_tdep (void);



static CORE_ADDR
z80_skip_prologue (struct gdbarch *gdbarch, CORE_ADDR pc);

static const char *
z80_register_name (struct gdbarch *gdbarch, int regnum);

static struct type *
z80_register_type (struct gdbarch *gdbarch, int reg_nr);

static void
z80_pseudo_register_read (struct gdbarch *gdbarch, struct regcache *regcache,
                          int regnum, gdb_byte *buf);

static void
z80_pseudo_register_write (struct gdbarch *gdbarch, struct regcache *regcache,
                           int regnum, const gdb_byte *buf);

static enum return_value_convention
z80_return_value (struct gdbarch *gdbarch, struct type *func_type,
		  struct type *valtype, struct regcache *regcache,
		  gdb_byte *readbuf, const gdb_byte *writebuf);

static const unsigned char *
z80_breakpoint_from_pc (struct gdbarch *gdbarch, CORE_ADDR * pcptr, int *lenptr);

static CORE_ADDR
z80_push_dummy_call (struct gdbarch *gdbarch, struct value *function,
                     struct regcache *regcache, CORE_ADDR bp_addr,
                     int nargs, struct value **args, CORE_ADDR sp,
                     int struct_return, CORE_ADDR struct_addr);
