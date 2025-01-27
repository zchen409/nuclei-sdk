/*
 * Copyright (c) 2009-2018 Arm Limited. All rights reserved.
 * Copyright (c) 2019 Nuclei Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/******************************************************************************
 * @file     system_demosoc.c
 * @brief    NMSIS Nuclei Core Device Peripheral Access Layer Source File for
 *           Nuclei Demo SoC which support Nuclei N/NX class cores
 * @version  V1.00
 * @date     22. Nov 2019
 ******************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include "nuclei_sdk_hal.h"

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
/* ToDo: add here your necessary defines for device initialization
         following is an example for different system frequencies */
#ifndef SYSTEM_CLOCK
#define SYSTEM_CLOCK    (80000000UL)
#endif

/**
 * \defgroup  NMSIS_Core_SystemConfig       System Device Configuration
 * \brief Functions for system and clock setup available in system_<device>.c.
 * \details
 * Nuclei provides a template file **system_Device.c** that must be adapted by
 * the silicon vendor to match their actual device. As a <b>minimum requirement</b>,
 * this file must provide:
 *  -  A device-specific system configuration function, \ref SystemInit.
 *  -  A global variable that contains the system frequency, \ref SystemCoreClock.
 *  -  A global eclic configuration initialization, \ref ECLIC_Init.
 *  -  Global c library \ref _init and \ref _fini functions called right before calling main function.
 *  -  Vendor customized interrupt, exception and nmi handling code, see \ref NMSIS_Core_IntExcNMI_Handling
 *
 * The file configures the device and, typically, initializes the oscillator (PLL) that is part
 * of the microcontroller device. This file might export other functions or variables that provide
 * a more flexible configuration of the microcontroller system.
 *
 * And this file also provided common interrupt, exception and NMI exception handling framework template,
 * Silicon vendor can customize these template code as they want.
 *
 * \note Please pay special attention to the static variable \c SystemCoreClock. This variable might be
 * used throughout the whole system initialization and runtime to calculate frequency/time related values.
 * Thus one must assure that the variable always reflects the actual system clock speed.
 *
 * \attention
 * Be aware that a value stored to \c SystemCoreClock during low level initialization (i.e. \c SystemInit()) might get
 * overwritten by C libray startup code and/or .bss section initialization.
 * Thus its highly recommended to call \ref SystemCoreClockUpdate at the beginning of the user \c main() routine.
 *
 * @{
 */

/*----------------------------------------------------------------------------
  System Core Clock Variable
 *----------------------------------------------------------------------------*/
/* ToDo: initialize SystemCoreClock with the system core clock frequency value
         achieved after system intitialization.
         This means system core clock frequency after call to SystemInit() */
/**
 * \brief      Variable to hold the system core clock value
 * \details
 * Holds the system core clock, which is the system clock frequency supplied to the SysTick
 * timer and the processor core clock. This variable can be used by debuggers to query the
 * frequency of the debug timer or to configure the trace clock speed.
 *
 * \attention
 * Compilers must be configured to avoid removing this variable in case the application
 * program is not using it. Debugging systems require the variable to be physically
 * present in memory so that it can be examined to configure the debugger.
 */
volatile uint32_t SystemCoreClock = SYSTEM_CLOCK;  /* System Clock Frequency (Core Clock) */

/*----------------------------------------------------------------------------
  Clock functions
 *----------------------------------------------------------------------------*/

/**
 * \brief      Function to update the variable \ref SystemCoreClock
 * \details
 * Updates the variable \ref SystemCoreClock and must be called whenever the core clock is changed
 * during program execution. The function evaluates the clock register settings and calculates
 * the current core clock.
 */
void SystemCoreClockUpdate(void)             /* Get Core Clock Frequency */
{
    /* ToDo: add code to calculate the system frequency based upon the current
     *    register settings.
     * Note: This function can be used to retrieve the system core clock frequeny
     *    after user changed register settings.
     */
    SystemCoreClock = SYSTEM_CLOCK;
}

/**
 * \brief      Function to Initialize the system.
 * \details
 * Initializes the microcontroller system. Typically, this function configures the
 * oscillator (PLL) that is part of the microcontroller device. For systems
 * with a variable clock speed, it updates the variable \ref SystemCoreClock.
 * SystemInit is called from the file <b>startup<i>_device</i></b>.
 */
void SystemInit(void)
{
    /* ToDo: add code to initialize the system
     * Warn: do not use global variables because this function is called before
     * reaching pre-main. RW section maybe overwritten afterwards.
     */
    SystemCoreClock = SYSTEM_CLOCK;
}

/**
 * \defgroup  NMSIS_Core_IntExcNMI_Handling   Interrupt and Exception and NMI Handling
 * \brief Functions for interrupt, exception and nmi handle available in system_<device>.c.
 * \details
 * Nuclei provide a template for interrupt, exception and NMI handling. Silicon Vendor could adapat according
 * to their requirement. Silicon vendor could implement interface for different exception code and
 * replace current implementation.
 *
 * @{
 */
/** \brief Max exception handler number, don't include the NMI(0xFFF) one */
#define MAX_SYSTEM_EXCEPTION_NUM        12
/**
 * \brief      Store the exception handlers for each exception ID
 * \note
 * - This SystemExceptionHandlers are used to store all the handlers for all
 * the exception codes Nuclei N/NX core provided.
 * - Exception code 0 - 11, totally 12 exceptions are mapped to SystemExceptionHandlers[0:11]
 * - Exception for NMI is also re-routed to exception handling(exception code 0xFFF) in startup code configuration, the handler itself is mapped to SystemExceptionHandlers[MAX_SYSTEM_EXCEPTION_NUM]
 */
static unsigned long SystemExceptionHandlers[MAX_SYSTEM_EXCEPTION_NUM + 1];

/**
 * \brief      Exception Handler Function Typedef
 * \note
 * This typedef is only used internal in this system_<Device>.c file.
 * It is used to do type conversion for registered exception handler before calling it.
 */
typedef void (*EXC_HANDLER)(unsigned long mcause, unsigned long sp);

/**
 * \brief      System Default Exception Handler
 * \details
 * This function provided a default exception and NMI handling code for all exception ids.
 * By default, It will just print some information for debug, Vendor can customize it according to its requirements.
 */
static void system_default_exception_handler(unsigned long mcause, unsigned long sp)
{
    /* TODO: Uncomment this if you have implement printf function */
    printf("MCAUSE : 0x%lx\r\n", mcause);
    printf("MDCAUSE: 0x%lx\r\n", __RV_CSR_READ(CSR_MDCAUSE));
    printf("MEPC   : 0x%lx\r\n", __RV_CSR_READ(CSR_MEPC));
    printf("MTVAL  : 0x%lx\r\n", __RV_CSR_READ(CSR_MTVAL));
    Exception_DumpFrame(sp);
#if defined(SIMULATION_MODE)
    // directly exit if in SIMULATION
    extern void simulation_exit(int status);
    simulation_exit(1);
#else
    while (1);
#endif
}

/**
 * \brief      Initialize all the default core exception handlers
 * \details
 * The core exception handler for each exception id will be initialized to \ref system_default_exception_handler.
 * \note
 * Called in \ref _init function, used to initialize default exception handlers for all exception IDs
 */
static void Exception_Init(void)
{
    for (int i = 0; i < MAX_SYSTEM_EXCEPTION_NUM + 1; i++) {
        SystemExceptionHandlers[i] = (unsigned long)system_default_exception_handler;
    }
}

/**
 * \brief      Dump Exception Frame
 * \details
 * This function provided feature to dump exception frame stored in stack.
 */
void Exception_DumpFrame(unsigned long sp)
{
    EXC_Frame_Type *exc_frame = (EXC_Frame_Type *)sp;

#ifndef __riscv_32e
    printf("ra: 0x%lx, tp: 0x%lx, t0: 0x%lx, t1: 0x%lx, t2: 0x%lx, t3: 0x%lx, t4: 0x%lx, t5: 0x%lx, t6: 0x%lx\n" \
           "a0: 0x%lx, a1: 0x%lx, a2: 0x%lx, a3: 0x%lx, a4: 0x%lx, a5: 0x%lx, a6: 0x%lx, a7: 0x%lx\n" \
           "mcause: 0x%lx, mepc: 0x%lx, msubm: 0x%lx\n", exc_frame->ra, exc_frame->tp, exc_frame->t0, \
           exc_frame->t1, exc_frame->t2, exc_frame->t3, exc_frame->t4, exc_frame->t5, exc_frame->t6, \
           exc_frame->a0, exc_frame->a1, exc_frame->a2, exc_frame->a3, exc_frame->a4, exc_frame->a5, \
           exc_frame->a6, exc_frame->a7, exc_frame->mcause, exc_frame->mepc, exc_frame->msubm);
#else
    printf("ra: 0x%lx, tp: 0x%lx, t0: 0x%lx, t1: 0x%lx, t2: 0x%lx\n" \
           "a0: 0x%lx, a1: 0x%lx, a2: 0x%lx, a3: 0x%lx, a4: 0x%lx, a5: 0x%lx\n" \
           "mcause: 0x%lx, mepc: 0x%lx, msubm: 0x%lx\n", exc_frame->ra, exc_frame->tp, exc_frame->t0, \
           exc_frame->t1, exc_frame->t2, exc_frame->a0, exc_frame->a1, exc_frame->a2, exc_frame->a3, \
           exc_frame->a4, exc_frame->a5, exc_frame->mcause, exc_frame->mepc, exc_frame->msubm);
#endif
}

/**
 * \brief       Register an exception handler for exception code EXCn
 * \details
 * * For EXCn < \ref MAX_SYSTEM_EXCEPTION_NUM, it will be registered into SystemExceptionHandlers[EXCn-1].
 * * For EXCn == NMI_EXCn, it will be registered into SystemExceptionHandlers[MAX_SYSTEM_EXCEPTION_NUM].
 * \param   EXCn    See \ref EXCn_Type
 * \param   exc_handler     The exception handler for this exception code EXCn
 */
void Exception_Register_EXC(uint32_t EXCn, unsigned long exc_handler)
{
    if ((EXCn < MAX_SYSTEM_EXCEPTION_NUM) && (EXCn >= 0)) {
        SystemExceptionHandlers[EXCn] = exc_handler;
    } else if (EXCn == NMI_EXCn) {
        SystemExceptionHandlers[MAX_SYSTEM_EXCEPTION_NUM] = exc_handler;
    }
}

/**
 * \brief       Get current exception handler for exception code EXCn
 * \details
 * * For EXCn < \ref MAX_SYSTEM_EXCEPTION_NUM, it will return SystemExceptionHandlers[EXCn-1].
 * * For EXCn == NMI_EXCn, it will return SystemExceptionHandlers[MAX_SYSTEM_EXCEPTION_NUM].
 * \param   EXCn    See \ref EXCn_Type
 * \return  Current exception handler for exception code EXCn, if not found, return 0.
 */
unsigned long Exception_Get_EXC(uint32_t EXCn)
{
    if ((EXCn < MAX_SYSTEM_EXCEPTION_NUM) && (EXCn >= 0)) {
        return SystemExceptionHandlers[EXCn];
    } else if (EXCn == NMI_EXCn) {
        return SystemExceptionHandlers[MAX_SYSTEM_EXCEPTION_NUM];
    } else {
        return 0;
    }
}

/**
 * \brief      Common NMI and Exception handler entry
 * \details
 * This function provided a command entry for NMI and exception. Silicon Vendor could modify
 * this template implementation according to requirement.
 * \remarks
 * - RISCV provided common entry for all types of exception. This is proposed code template
 *   for exception entry function, Silicon Vendor could modify the implementation.
 * - For the core_exception_handler template, we provided exception register function \ref Exception_Register_EXC
 *   which can help developer to register your exception handler for specific exception number.
 */
uint32_t core_exception_handler(unsigned long mcause, unsigned long sp)
{
    uint32_t EXCn = (uint32_t)(mcause & 0X00000fff);
    EXC_HANDLER exc_handler;

    if ((EXCn < MAX_SYSTEM_EXCEPTION_NUM) && (EXCn >= 0)) {
        exc_handler = (EXC_HANDLER)SystemExceptionHandlers[EXCn];
    } else if (EXCn == NMI_EXCn) {
        exc_handler = (EXC_HANDLER)SystemExceptionHandlers[MAX_SYSTEM_EXCEPTION_NUM];
    } else {
        exc_handler = (EXC_HANDLER)system_default_exception_handler;
    }
    if (exc_handler != NULL) {
        exc_handler(mcause, sp);
    }
    return 0;
}
/** @} */ /* End of Doxygen Group NMSIS_Core_ExceptionAndNMI */

/** Banner Print for Nuclei SDK */
void SystemBannerPrint(void)
{
#if defined(NUCLEI_BANNER) && (NUCLEI_BANNER == 1)
    printf("Nuclei SDK Build Time: %s, %s\r\n", __DATE__, __TIME__);
#ifdef DOWNLOAD_MODE_STRING
    printf("Download Mode: %s\r\n", DOWNLOAD_MODE_STRING);
#endif
    printf("CPU Frequency %u Hz\r\n", (unsigned int)SystemCoreClock);
#endif
}

/**
 * \brief initialize eclic config
 * \details
 * ECLIC needs be initialized after boot up,
 * Vendor could also change the initialization
 * configuration.
 */
void ECLIC_Init(void)
{
    /* Global Configuration about MTH and NLBits.
     * TODO: Please adapt it according to your system requirement.
     * This function is called in _init function */
    ECLIC_SetMth(0);
    ECLIC_SetCfgNlbits(__ECLIC_INTCTLBITS);
}

/**
 * \brief  Initialize a specific IRQ and register the handler
 * \details
 * This function set vector mode, trigger mode and polarity, interrupt level and priority,
 * assign handler for specific IRQn.
 * \param [in]  IRQn        NMI interrupt handler address
 * \param [in]  shv         \ref ECLIC_NON_VECTOR_INTERRUPT means non-vector mode, and \ref ECLIC_VECTOR_INTERRUPT is vector mode
 * \param [in]  trig_mode   see \ref ECLIC_TRIGGER_Type
 * \param [in]  lvl         interupt level
 * \param [in]  priority    interrupt priority
 * \param [in]  handler     interrupt handler, if NULL, handler will not be installed
 * \return       -1 means invalid input parameter. 0 means successful.
 * \remarks
 * - This function use to configure specific eclic interrupt and register its interrupt handler and enable its interrupt.
 * - If the vector table is placed in read-only section(FLASHXIP mode), handler could not be installed
 */
int32_t ECLIC_Register_IRQ(IRQn_Type IRQn, uint8_t shv, ECLIC_TRIGGER_Type trig_mode, uint8_t lvl, uint8_t priority, void* handler)
{
    if ((IRQn > SOC_INT_MAX) || (shv > ECLIC_VECTOR_INTERRUPT) \
        || (trig_mode > ECLIC_NEGTIVE_EDGE_TRIGGER)) {
        return -1;
    }

    /* set interrupt vector mode */
    ECLIC_SetShvIRQ(IRQn, shv);
    /* set interrupt trigger mode and polarity */
    ECLIC_SetTrigIRQ(IRQn, trig_mode);
    /* set interrupt level */
    ECLIC_SetLevelIRQ(IRQn, lvl);
    /* set interrupt priority */
    ECLIC_SetPriorityIRQ(IRQn, priority);
    if (handler != NULL) {
        /* set interrupt handler entry to vector table */
        ECLIC_SetVector(IRQn, (rv_csr_t)handler);
    }
    /* enable interrupt */
    ECLIC_EnableIRQ(IRQn);
    return 0;
}
/** @} */ /* End of Doxygen Group NMSIS_Core_ExceptionAndNMI */

volatile IRegion_Info_Type SystemIRegionInfo;
static void _get_iregion_info(IRegion_Info_Type *iregion)
{
    unsigned long mcfg_info;
    if (iregion == NULL) {
        return;
    }
    mcfg_info = __RV_CSR_READ(CSR_MCFG_INFO);
    if (mcfg_info & (1<<16)) { // IRegion Info present
        iregion->iregion_base = (__RV_CSR_READ(CSR_MIRGB_INFO) >> 10) << 10;
        iregion->eclic_base = iregion->iregion_base + 0x20000;
        iregion->systimer_base = iregion->iregion_base + 0x30000;
        iregion->smp_base = iregion->iregion_base + 0x40000;
        iregion->idu_base = iregion->iregion_base + 0x50000;
    } else {
        iregion->eclic_base = 0x0C000000UL;
        iregion->systimer_base = 0x02000000UL;
    }
}

/**
 * \brief Synchronize all harts
 * \details
 * This function is used to synchronize all the harts,
 * especially to wait the boot hart finish initialization of
 * data section, bss section and c runtines initialization
 * This function must be placed in .init section, since
 * section initialization is not ready, global variable
 * and static variable should be avoid to use in this function,
 * and avoid to call other functions
 */
#define CLINT_MSIP(base, hartid)    (*(volatile uint32_t *)((uintptr_t)((base) + ((hartid) * 4))))
#define SMP_CTRLREG(base, ofs)      (*(volatile uint32_t *)((uintptr_t)((base) + (ofs))))

__attribute__((section(".init"))) void __sync_harts(void)
{
// Only do synchronize when SMP_CPU_CNT is defined and number > 0
#if defined(SMP_CPU_CNT) && (SMP_CPU_CNT > 1)
    unsigned long hartid = __RV_CSR_READ(CSR_MHARTID);
    unsigned long clint_base, irgb_base, smp_base;
    unsigned long mcfg_info;

    mcfg_info = __RV_CSR_READ(CSR_MCFG_INFO);
    if (mcfg_info & (1<<16)) { // IRegion Info present
        // clint base = system timer base + 0x1000
        irgb_base = (__RV_CSR_READ(CSR_MIRGB_INFO) >> 10) << 10;
        clint_base = irgb_base + 0x30000 + 0x1000;
        smp_base = irgb_base + 0x40000; 
    } else {
        // system timer base for demosoc is 0x02000000
        clint_base = 0x02000000 + 0x1000;
        smp_base = (__RV_CSR_READ(CSR_MSMPCFG_INFO) >> 4) << 4;
    }
    // Enable SMP and L2
    SMP_CTRLREG(smp_base, 0xc) = 0xFFFFFFFF;
    SMP_CTRLREG(smp_base, 0x10) = 0x1;
    __SMP_RWMB();
    
    // pre-condition: interrupt must be disabled, this is done before calling this function
    if (hartid == 0) { // boot hart
        // clear msip pending
        for (int i = 0; i < SMP_CPU_CNT; i ++) {
            CLINT_MSIP(clint_base, i) = 0;
        }
        __SMP_RWMB();
    } else {
        // Set machine software interrupt pending to 1
        CLINT_MSIP(clint_base, hartid) = 1;
        __SMP_RWMB();
        // wait for pending bit cleared by boot hart
        while (CLINT_MSIP(clint_base, hartid) == 1);
    }
#endif
}

/**
 * \brief early init function before main
 * \details
 * This function is executed right before main function.
 * For RISC-V gnu toolchain, _init function might not be called
 * by __libc_init_array function, so we defined a new function
 * to do initialization
 */
void _premain_init(void)
{
    // TODO to make it possible for configurable boot hartid
    unsigned long hartid = __RV_CSR_READ(CSR_MHARTID);

    if (hartid == 0) { // only done in boot hart
        // IREGION INFO MUST BE SET BEFORE ANY PREMAIN INIT STEPS
        _get_iregion_info((IRegion_Info_Type *)(&SystemIRegionInfo));
    }
    /* TODO: Add your own initialization code here, called before main */
    // This code located in RUNMODE_CONTROL ifdef endif block just for internal usage
    // No need to use in your code
#ifdef RUNMODE_CONTROL
#if defined(RUNMODE_ILM_EN) && RUNMODE_ILM_EN == 0
    __RV_CSR_CLEAR(CSR_MILM_CTL, MILM_CTL_ILM_EN);
#endif
#if defined(RUNMODE_DLM_EN) && RUNMODE_DLM_EN == 0
    __RV_CSR_CLEAR(CSR_MDLM_CTL, MDLM_CTL_DLM_EN);
#endif
#endif

    /* __ICACHE_PRESENT and __DCACHE_PRESENT are defined in demosoc.h */
#if defined(__ICACHE_PRESENT) && __ICACHE_PRESENT == 1
    EnableICache();
#endif
#if defined(__DCACHE_PRESENT) && __DCACHE_PRESENT == 1
    EnableDCache();
#endif

    /* Do fence and fence.i to make sure previous ilm/dlm/icache/dcache control done */
    __RWMB();
    __FENCE_I();

    if (hartid == 0) { // only required for boot hartid
        SystemCoreClock = get_cpu_freq();
        gpio_iof_config(GPIO, IOF0_UART0_MASK, IOF_SEL_0);
        uart_init(SOC_DEBUG_UART, 115200);
        /* Display banner after UART initialized */
        SystemBannerPrint();
        /* Initialize exception default handlers */
        Exception_Init();
        /* ECLIC initialization, mainly MTH and NLBIT */
        ECLIC_Init();
#ifdef RUNMODE_CONTROL
        printf("Current RUNMODE=%s, ilm:%d, dlm %d, icache %d, dcache %d, ccm %d\n", \
            RUNMODE_STRING, RUNMODE_ILM_EN, RUNMODE_DLM_EN, \
            RUNMODE_IC_EN, RUNMODE_DC_EN, RUNMODE_CCM_EN);
        printf("CSR: MILM_CTL 0x%x, MDLM_CTL 0x%x, MCACHE_CTL 0x%x\n", \
            __RV_CSR_READ(CSR_MILM_CTL), __RV_CSR_READ(CSR_MDLM_CTL), \
            __RV_CSR_READ(CSR_MCACHE_CTL));
#endif
    }
}

/**
 * \brief finish function after main
 * \param [in]  status     status code return from main
 * \details
 * This function is executed right after main function.
 * For RISC-V gnu toolchain, _fini function might not be called
 * by __libc_fini_array function, so we defined a new function
 * to do initialization
 */
void _postmain_fini(int status)
{
    /* TODO: Add your own finishing code here, called after main */
#if defined(SIMULATION_MODE)
    extern void simulation_exit(int status);
    simulation_exit(status);
#endif
}

/**
 * \brief _init function called in __libc_init_array()
 * \details
 * This `__libc_init_array()` function is called during startup code,
 * user need to implement this function, otherwise when link it will
 * error init.c:(.text.__libc_init_array+0x26): undefined reference to `_init'
 * \note
 * Please use \ref _premain_init function now
 */
void _init(void)
{
    /* Don't put any code here, please use _premain_init now */
}

/**
 * \brief _fini function called in __libc_fini_array()
 * \details
 * This `__libc_fini_array()` function is called when exit main.
 * user need to implement this function, otherwise when link it will
 * error fini.c:(.text.__libc_fini_array+0x28): undefined reference to `_fini'
 * \note
 * Please use \ref _postmain_fini function now
 */
void _fini(void)
{
    /* Don't put any code here, please use _postmain_fini now */
}

/** @} */ /* End of Doxygen Group NMSIS_Core_SystemAndClock */
