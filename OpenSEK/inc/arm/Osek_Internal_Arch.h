/* Copyright 2008, 2009, Mariano Cerdeiro
 *
 * This file is part of OpenSEK.
 *
 * OpenSEK is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *             
 * Linking OpenSEK statically or dynamically with other modules is making a
 * combined work based on OpenSEK. Thus, the terms and conditions of the GNU
 * General Public License cover the whole combination.
 *
 * In addition, as a special exception, the copyright holders of OpenSEK give
 * you permission to combine OpenSEK program with free software programs or
 * libraries that are released under the GNU LGPL and with independent modules
 * that communicate with OpenSEK solely through the OpenSEK defined interface. 
 * You may copy and distribute such a system following the terms of the GNU GPL
 * for OpenSEK and the licenses of the other code concerned, provided that you
 * include the source code of that other code when and as the GNU GPL requires
 * distribution of source code.
 *
 * Note that people who make modified versions of OpenSEK are not obligated to
 * grant this special exception for their modified versions; it is their choice
 * whether to do so. The GNU General Public License gives permission to release
 * a modified version without this exception; this exception also makes it
 * possible to release a modified version which carries forward this exception.
 * 
 * OpenSEK is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenSEK. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _OSEK_INTERNAL_ARCH_H_
#define _OSEK_INTERNAL_ARCH_H_
/** \brief OpenSEK Internal Architecture Dependent Header File
 **
 ** \file arm/Osek_Internal_Arch.h
 ** \arch ARM
 **/

/** \addtogroup OpenSEK
 ** @{ */
/** \addtogroup OpenSEK_Internal
 ** @{ */


/*
 * Initials     Name
 * ---------------------------
 * MaCe			 Mariano Cerdeiro
 */

/*
 * modification history (new versions first)
 * -----------------------------------------------------------
 * v0.1.1 20090221 MaCe port this file to ARM7 architecture
 * v0.1.0 20081116 MaCe	initial version
 */  

/*==================[inclusions]=============================================*/

/*==================[macros]=================================================*/
/** This define makes the Osek_Internal.h file to include the
 ** Osek_Internal_Arch_Cpu file which is not standard for all architectures 
 **/
#define OSEK_INLCUDE_INTERNAL_ARCH_CPU

/** \brief Interrupt Secure Start Macro
 **
 ** This macro shall be used to disable the interrupts
 **/
#define IntSecure_Start() SuspendAllInterrupts()

/** \brief Interrupt Secure End Macro
 **
 ** This macro shall be used to restore the interrupts
 **/
#define IntSecure_End() ResumeAllInterrupts()

/** \brief osekpause
 **
 **/
#define osekpause()			\
	{								\
		PreCallService();		\
		/** TODO	*/				\
		PostCallService();	\
	}

/** \brief Call to an other Task
 **
 ** This function jmps to the indicated task.
 **/
#define CallTask(task)																			\
	{																									\
		Osek_OldTaskPtr_Arch = (void*)TasksConst[GetRunningTask()].TaskContext;	\
		SetRunningTask(task);																	\
		Osek_NewTaskPtr_Arch = (void*)TasksConst[task].TaskContext;					\
																										\
		__asm__ __volatile__ (																	\
			/* save the R0 reg on the stack */												\
			"STMDB	SP!, {R0}															\n\t"	\
			/* get the Task Pointer */															\
			"LDR R0, =Osek_OldTaskPtr_Arch											\n\t"	\
			/* load the task pointer */														\
			"LDR		R0, [R0]																\n\t"	\
			/* save all registers R1 - R15 without inc R0 */							\			"STMIB	R0,{R1-R15}^														\n\t"	\
			"ADD PC, PC, #4	/* avoid skiping jmp to new task */				\n\t" \
			"NOP	/* do not remove this nop */										\n\t"	\
			/* here is the return addresse for this task */								\
			"ADD PC, PC, #16 /* skip jmp to new task when comming back */	\n\t" \
			/* get R0 */																			\
			"LDMIA	SP!, {R1}															\n\t"	\
			/* push R0 (value is in R1) to */												\
			/*	Osek_TaskPtr_Arch (value in R0) */											\
			"STR R1, [R0]																	\n\t"	\
																										\
			/* get new context */																\
			/* load Task Pointer */																\
			"LDR R0, =Osek_NewTaskPtr_Arch											\n\t"	\
			/* load memory location of the pointer */										\
			"LDR R0, [R0]																	\n\t"	\
			/* get context */																		\
			"LDMIA R0, {R0-R15}															\n\t"	\
			/* decrement sp */																	\
			"ADD SP, SP, #4																\n\t"	\
		);																								\
	}

/* \
	__asm__ __volatile (\
		"LDR R0, =0x01010101 \n\t"	\
		"LDR R1, =0x11111111 \n\t"	\
		"LDR R2, =0x21212121 \n\t"	\
		"LDR R3, =0x31313131 \n\t"	\
		"LDR R4, =0x41414141 \n\t"	\
		"LDR R5, =0x51515151 \n\t"	\
		"LDR R6, =0x61616161 \n\t"	\
		"LDR R7, =0x71717171 \n\t"	\
		"LDR R8, =0x81818181 \n\t"	\
		"LDR R9, =0x91919191 \n\t"	\
		"LDR R10, =0xA1A1A1A1 \n\t"	\
		"LDR R11, =0xB1B1B1B1 \n\t"	\
		"LDR R12, =0xC1C1C1C1 \n\t"	\
	); \ */


/** \brief Jmp to an other Task
 **
 ** This function jmps to the indicated task.
 **/
#define JmpTask(task)															\
	{																					\
	Osek_NewTaskPtr_Arch = (void*)TasksConst[task].TaskContext;		\
																						\
	__asm__ __volatile__ (														\
		/* load Task Pointer */													\
		"LDR R0, =Osek_NewTaskPtr_Arch								\n\t"	\
		/* load memory location of the pointer */							\
		"LDR R0, [R0]														\n\t"	\
		/* get context */															\
		"LDMIA R0, {R0-R15}												\n\t"	\
	);																					\
	}

/** \brief Save context */
#define SaveContext(task) 														\
	{																					\
	Osek_OldTaskPtr_Arch = (void*)TasksConst[task].TaskContext;		\
																						\
	__asm__ __volatile__ (														\
		/* save the R0 reg on the stack */									\
		"STMDB SP!, {R0}													\n\t"	\
		/* get the Task Pointer */												\
		"LDR R0, =Osek_OldTaskPtr_Arch								\n\t"	\
		/* load the task pointer */											\
		"LDR R0, [R0]														\n\t"	\
		/* decrement stack pointer */											\
		"ADD SP, SP, #4													\n\t"	\
		/* save all registers R1 - R15 without inc R0 */				\
		"STMIB R0,{R1-R15}^												\n\t"	\
		/* increment stack pointer back */									\
		"SUB SP, SP, #4													\n\t"	\
		/* get PC back */															\
		"LDR R1,[R0,#60]													\n\t"	\
		/* increment the right value */										\
		"ADD R1, R1, #16													\n\t"	\
		/* save it back */														\
		"STR R1,[R0,#60]													\n\t"	\
		/* get R0 */																\
		"LDMIA SP!, {R1}													\n\t"	\
		/* push R0 (value is in R1) to */									\
		/*	Osek_TaskPtr_Arch (value in R0) */								\
		"STR R1, [R0]														\n\t"	\
		/* saved PC points here */												\
	);																					\
	}

/** \brief Set the entry point for a task */
#define SetEntryPoint(task)							\
	{															\
		/* init entry point */							\
		TasksConst[task].TaskContext->reg_r15 = 	\
			(uint32) TasksConst[task].EntryPoint;	\
	}

/** \brief */
#define ResetStack(task)											\
	{																		\
		/* init stack */												\
		TasksConst[task].TaskContext->reg_r13 = 				\
			(uint32)TasksConst[task].StackPtr + 				\
			TasksConst[task].StackSize;							\
   }

#define ISR_NMI      0
#define ISR_CTR      1
#define ISR_CANRX    2
#define ISR_CANTX    3

#define EnableOSInterrupts()															\
	{																							\
		/** TODO */																			\
	}

#define EnableInterrupts()		\
	{									\
		/** TODO */					\
	}

/** \brief Get Counter Actual Value
 **
 ** This macro returns the actual value of the counter
 **
 ** \param[in] CounterID id of the counter to be readed
 ** \return Actual value of the counter
 **/
#define GetCounter_Arch(CounterID) (CountersVar[CounterID].Time)

/** \brief Pre ISR Macro
 **
 ** This macro is called every time that an ISR Cat 2 is started
 **/
#define PreIsr2_Arch(isr)

/** \brief Post ISR Macro
 **
 ** This macro is called every time that an ISR Cat 2 is finished
 **/
#define PostIsr2_Arch(isr)

#define SavePosixStack() \
   {                                   \
      /* save actual win esp */        \
      __asm__ __volatile__ ("movl %%esp, %%eax; movl %%eax, %0;" : "=g" (PosixStack) : : "eax"); \
   }

/** \brief Pre Call Service
 **
 ** This macro shall be called before calling any posix system service
 **/
#define PreCallService()

/** \brief Post Call Service
 **
 ** This macro shall be called after calling any posix system service
 **/
#define PostCallService()


/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/
/** \brief Osek New Task Pointer for ARM Architecture
 **
 ** This variable content the pointer to the new task.
 **/
extern void* Osek_NewTaskPtr_Arch;

/** \brief Osek Old Task Pointer for ARM Architecture
 **
 ** This variable content the pointer to the old task.
 **/
extern void* Osek_OldTaskPtr_Arch;

/*==================[external functions declaration]=========================*/
/** \brief IRQ Interupt
 **/
void IRQ_Routine (void)   __attribute__ ((interrupt("IRQ")));

/** \brief FIQ Interupt
 **
 ** This function implements the Fast Interruption
 **/
void FIQ_Routine (void)   __attribute__ ((interrupt("FIQ")));

/** \brief SWI Interrupt
 **/
void SWI_Routine (void)   __attribute__ ((interrupt("SWI")));

/** \brief UNDEF Instruction Interrupt
 **/
void UNDEF_Routine (void) __attribute__ ((interrupt("UNDEF")));

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/*==================[end of file]============================================*/
#endif /* #ifndef _OSEK_INTERNAL_ARCH_H_ */

