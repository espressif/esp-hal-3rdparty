#ifndef __RVRUNTIME_FRAMES_H__
#define __RVRUNTIME_FRAMES_H__

/* Align a value up to nearest n-byte boundary, where n is a power of 2. */
#define ALIGNUP(n, val) (((val) + (n)-1) & -(n))

#ifdef STRUCT_BEGIN
#undef STRUCT_BEGIN
#undef STRUCT_FIELD
#undef STRUCT_AFIELD
#undef STRUCT_END
#endif

#if defined(_ASMLANGUAGE) || defined(__ASSEMBLER__)

#define STRUCT_BEGIN            .pushsection .text; .struct 0
#define STRUCT_FIELD(ctype,size,asname,name)    asname: .space  size
#define STRUCT_AFIELD(ctype,size,asname,name,n) asname: .space  (size)*(n)
#define STRUCT_END(sname)       sname##Size:; .popsection

#else

#define STRUCT_BEGIN            typedef struct {
#define STRUCT_FIELD(ctype,size,asname,name)    ctype   name;
#define STRUCT_AFIELD(ctype,size,asname,name,n) ctype   name[n];
#define STRUCT_END(sname)       } sname;

#endif

/*
-------------------------------------------------------------------------------
   INTERRUPT/EXCEPTION STACK FRAME FOR A EXCEPTION OR NESTED INTERRUPT
-------------------------------------------------------------------------------
*/
STRUCT_BEGIN
STRUCT_FIELD (long, 4, RV_STK_MEPC,			mepc)		/* Machine Exception Program Counter */
STRUCT_FIELD (long, 4, RV_STK_RA,			ra)			/* Return address */
STRUCT_FIELD (long, 4, RV_STK_SP,			sp)			/* Stack pointer */
STRUCT_FIELD (long, 4, RV_STK_GP,			gp)			/* Global pointer */
STRUCT_FIELD (long, 4, RV_STK_TP,			tp)			/* Thread pointer */
STRUCT_FIELD (long, 4, RV_STK_T0,			t0)			/* Temporary/alternate link register */
STRUCT_FIELD (long, 4, RV_STK_T1,			t1)			/* t1-2: Temporaries */
STRUCT_FIELD (long, 4, RV_STK_T2,			t2)
STRUCT_FIELD (long, 4, RV_STK_S0,			s0)			/* Saved register/frame pointer */
STRUCT_FIELD (long, 4, RV_STK_S1,			s1)			/* Saved register */
STRUCT_FIELD (long, 4, RV_STK_A0,			a0)			/* a0-1: Function arguments/return address */
STRUCT_FIELD (long, 4, RV_STK_A1,			a1)
STRUCT_FIELD (long, 4, RV_STK_A2,			a2)			/* a2-7: Function arguments */
STRUCT_FIELD (long, 4, RV_STK_A3,			a3)
STRUCT_FIELD (long, 4, RV_STK_A4,			a4)
STRUCT_FIELD (long, 4, RV_STK_A5,			a5)
STRUCT_FIELD (long, 4, RV_STK_A6,			a6)
STRUCT_FIELD (long, 4, RV_STK_A7,			a7)
STRUCT_FIELD (long, 4, RV_STK_S2,			s2)			/* s2-11: Saved registers */
STRUCT_FIELD (long, 4, RV_STK_S3,			s3)
STRUCT_FIELD (long, 4, RV_STK_S4,			s4)
STRUCT_FIELD (long, 4, RV_STK_S5,			s5)
STRUCT_FIELD (long, 4, RV_STK_S6,			s6)
STRUCT_FIELD (long, 4, RV_STK_S7,			s7)
STRUCT_FIELD (long, 4, RV_STK_S8,			s8)
STRUCT_FIELD (long, 4, RV_STK_S9,			s9)
STRUCT_FIELD (long, 4, RV_STK_S10,			s10)
STRUCT_FIELD (long, 4, RV_STK_S11,			s11)
STRUCT_FIELD (long, 4, RV_STK_T3,			t3)			/* t3-6: Temporaries */
STRUCT_FIELD (long, 4, RV_STK_T4,			t4)
STRUCT_FIELD (long, 4, RV_STK_T5,			t5)
STRUCT_FIELD (long, 4, RV_STK_T6,			t6)
STRUCT_FIELD (long, 4, RV_STK_MSTATUS,		mstatus)	/* Machine Status */
STRUCT_FIELD (long, 4, RV_STK_MTVEC,        mtvec)		/* Machine Trap-Vector Base Address */
STRUCT_FIELD (long, 4, RV_STK_MCAUSE,		mcause)		/* Machine Trap Cause */
STRUCT_FIELD (long, 4, RV_STK_PCCRS,        pccrs)		/* Performance Counter Counter Registers */
STRUCT_FIELD (long, 4, RV_STK_PCER,			pcer)		/* Performance Counter Enable */
STRUCT_FIELD (long, 4, RV_STK_PCMR,			pcmr)		/* Performance Counter Mode */
STRUCT_FIELD (long, 4, RV_STK_HWLP,			hwlp)		/* Hardware Loop Registers */
STRUCT_FIELD (long, 4, RV_STK_PRIVLV,		privlv)		/* Privilege Level */
STRUCT_FIELD (long, 4, RV_STK_UHARTID,		uhartid)	/* Hardware Thread ID */
STRUCT_FIELD (long, 4, RV_STK_MHARTID,		mhartid)	/* Hardware Thread ID */
STRUCT_FIELD (long, 4, RV_STK_DCSR,			dcsr)		/* Debug Control and Status */
STRUCT_FIELD (long, 4, RV_STK_DPC,			dpc)		/* Debug PC */
STRUCT_FIELD (long, 4, RV_STK_INTC_THRESH,  intc_thresh)
STRUCT_FIELD (long, 4, RV_STK_RESERVED0,	reserved0)
STRUCT_END(RvExcFrame)

#if defined(_ASMLANGUAGE) || defined(__ASSEMBLER__)
# define RV_STK_SZ1		RvExcFrameSize
#else
# define RV_STK_SZ1		sizeof(RvExcFrame)
#endif

/*
 * This is the frame size.
 */
#define RV_STK_FRMSZ	(ALIGNUP(0x10, RV_STK_SZ1))

#endif /* #ifndef __RVRUNTIME_FRAMES_H__ */
