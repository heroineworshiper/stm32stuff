/**************************************************************************//**
 * @file     startup_ARMCM4.s
 * @brief    CMSIS Cortex-M4 Core Device Startup File
 *           for CM4 Device Series
 * @version  V1.04
 * @date     14. January 2011
 *------- <<< Use Configuration Wizard in Context Menu >>> ------------------
 *
 ******************************************************************************/

/*****************************************************************************/
/* Version: CodeSourcery Sourcery G++ Lite (with CS3)                        */
/*****************************************************************************/


/*
// <h> Stack Configuration
//   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
// </h>
*/

  	.syntax unified
  	.cpu cortex-m4
  	.fpu softvfp
    .thumb

	
    .equ    Stack_Size, 0x00000400
    .section ".stack", "w"
    .align  3
    .globl  __cs3_stack_mem
    .globl  __cs3_stack_size
__cs3_stack_mem:
    .if     Stack_Size
    .space  Stack_Size
    .endif
    .size   __cs3_stack_mem,  . - __cs3_stack_mem
    .set    __cs3_stack_size, . - __cs3_stack_mem


/*
// <h> Heap Configuration
//   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
// </h>
*/

    .equ    Heap_Size,  0x00020000
    
    .section ".heap", "w"
    .align  3
    .globl  __cs3_heap_start
    .globl  __cs3_heap_end
__cs3_heap_start:
    .if     Heap_Size
    .space  Heap_Size
    .endif
__cs3_heap_end:


/* Vector Table */

    .section ".cs3.interrupt_vector"
    .globl  __cs3_interrupt_vector_cortex_m
    .type   __cs3_interrupt_vector_cortex_m, %object

__cs3_interrupt_vector_cortex_m:
    .long   __cs3_stack                 /* Top of Stack                 */
    .long   __cs3_reset                 /* Reset Handler                */
    .long   NMI_Handler                 /* NMI Handler                  */
    .long   HardFault_Handler           /* Hard Fault Handler           */
    .long   MemManage_Handler           /* MPU Fault Handler            */
    .long   BusFault_Handler            /* Bus Fault Handler            */
    .long   UsageFault_Handler          /* Usage Fault Handler          */
    .long   0                           /* Reserved                     */
    .long   0                           /* Reserved                     */
    .long   0                           /* Reserved                     */
    .long   0                           /* Reserved                     */
    .long   SVC_Handler                 /* SVCall Handler               */
    .long   DebugMon_Handler            /* Debug Monitor Handler        */
    .long   0                           /* Reserved                     */
    .long   PendSV_Handler              /* PendSV Handler               */
    .long   SysTick_Handler             /* SysTick Handler              */

    /* External Interrupts */
/*    .long   DEF_IRQHandler        */      /*  0: Default                  */
/*  External Interrupts copied from startup_stm32f4xx.s */
    .long     WWDG_IRQHandler                   /* Window WatchDog                                        */
    .long     PVD_IRQHandler                    /* PVD through EXTI Line detection                        */
    .long     TAMP_STAMP_IRQHandler             /* Tamper and TimeStamps through the EXTI line            */
    .long     RTC_WKUP_IRQHandler               /* RTC Wakeup through the EXTI line                       */
    .long     FLASH_IRQHandler                  /* FLASH                                           */
    .long     RCC_IRQHandler                    /* RCC                                             */
    .long     EXTI0_IRQHandler                  /* EXTI Line0                                             */
    .long     EXTI1_IRQHandler                  /* EXTI Line1                                             */
    .long     EXTI2_IRQHandler                  /* EXTI Line2                                             */
    .long     EXTI3_IRQHandler                  /* EXTI Line3                                             */
    .long     EXTI4_IRQHandler                  /* EXTI Line4                                             */
    .long     DMA1_Stream0_IRQHandler           /* DMA1 Stream 0                                   */
    .long     DMA1_Stream1_IRQHandler           /* DMA1 Stream 1                                   */
    .long     DMA1_Stream2_IRQHandler           /* DMA1 Stream 2                                   */
    .long     DMA1_Stream3_IRQHandler           /* DMA1 Stream 3                                   */
    .long     DMA1_Stream4_IRQHandler           /* DMA1 Stream 4                                   */
    .long     DMA1_Stream5_IRQHandler           /* DMA1 Stream 5                                   */
    .long     DMA1_Stream6_IRQHandler           /* DMA1 Stream 6                                   */
    .long     ADC_IRQHandler                    /* ADC1, ADC2 and ADC3s                            */
    .long     CAN1_TX_IRQHandler                /* CAN1 TX                                                */
    .long     CAN1_RX0_IRQHandler               /* CAN1 RX0                                               */
    .long     CAN1_RX1_IRQHandler               /* CAN1 RX1                                               */
    .long     CAN1_SCE_IRQHandler               /* CAN1 SCE                                               */
    .long     EXTI9_5_IRQHandler                /* External Line[9:5]s                                    */
    .long     TIM1_BRK_TIM9_IRQHandler          /* TIM1 Break and TIM9                   */
    .long     TIM1_UP_TIM10_IRQHandler          /* TIM1 Update and TIM10                 */
    .long     TIM1_TRG_COM_TIM11_IRQHandler     /* TIM1 Trigger and Commutation and TIM11*/
    .long     TIM1_CC_IRQHandler                /* TIM1 Capture Compare                                   */
    .long     TIM2_IRQHandler                   /* TIM2                                            */
    .long     TIM3_IRQHandler                   /* TIM3                                            */
    .long     TIM4_IRQHandler                   /* TIM4                                            */
    .long     I2C1_EV_IRQHandler                /* I2C1 Event                                             */
    .long     I2C1_ER_IRQHandler                /* I2C1 Error                                             */
    .long     I2C2_EV_IRQHandler                /* I2C2 Event                                             */
    .long     I2C2_ER_IRQHandler                /* I2C2 Error                                               */
    .long     SPI1_IRQHandler                   /* SPI1                                            */
    .long     SPI2_IRQHandler                   /* SPI2                                            */
    .long     USART1_IRQHandler                 /* USART1                                          */
    .long     USART2_IRQHandler                 /* USART2                                          */
    .long     USART3_IRQHandler                 /* USART3                                          */
    .long     EXTI15_10_IRQHandler              /* External Line[15:10]s                                  */
    .long     RTC_Alarm_IRQHandler              /* RTC Alarm (A and B) through EXTI Line                  */
    .long     OTG_FS_WKUP_IRQHandler            /* USB OTG FS Wakeup through EXTI line                        */
    .long     TIM8_BRK_TIM12_IRQHandler         /* TIM8 Break and TIM12                  */
    .long     TIM8_UP_TIM13_IRQHandler          /* TIM8 Update and TIM13                 */
    .long     TIM8_TRG_COM_TIM14_IRQHandler     /* TIM8 Trigger and Commutation and TIM14*/
    .long     TIM8_CC_IRQHandler                /* TIM8 Capture Compare                                   */
    .long     DMA1_Stream7_IRQHandler           /* DMA1 Stream7                                           */
    .long     FSMC_IRQHandler                   /* FSMC                                            */
    .long     SDIO_IRQHandler                   /* SDIO                                            */
    .long     TIM5_IRQHandler                   /* TIM5                                            */
    .long     SPI3_IRQHandler                   /* SPI3                                            */
    .long     UART4_IRQHandler                  /* UART4                                           */
    .long     UART5_IRQHandler                  /* UART5                                           */
    .long     TIM6_DAC_IRQHandler               /* TIM6 and DAC1&2 underrun errors                   */
    .long     TIM7_IRQHandler                   /* TIM7                   */
    .long     DMA2_Stream0_IRQHandler           /* DMA2 Stream 0                                   */
    .long     DMA2_Stream1_IRQHandler           /* DMA2 Stream 1                                   */
    .long     DMA2_Stream2_IRQHandler           /* DMA2 Stream 2                                   */
    .long     DMA2_Stream3_IRQHandler           /* DMA2 Stream 3                                   */
    .long     DMA2_Stream4_IRQHandler           /* DMA2 Stream 4                                   */
    .long     ETH_IRQHandler                    /* Ethernet                                        */
    .long     ETH_WKUP_IRQHandler               /* Ethernet Wakeup through EXTI line                      */
    .long     CAN2_TX_IRQHandler                /* CAN2 TX                                                */
    .long     CAN2_RX0_IRQHandler               /* CAN2 RX0                                               */
    .long     CAN2_RX1_IRQHandler               /* CAN2 RX1                                               */
    .long     CAN2_SCE_IRQHandler               /* CAN2 SCE                                               */
    .long     OTG_FS_IRQHandler                 /* USB OTG FS                                      */
    .long     DMA2_Stream5_IRQHandler           /* DMA2 Stream 5                                   */
    .long     DMA2_Stream6_IRQHandler           /* DMA2 Stream 6                                   */
    .long     DMA2_Stream7_IRQHandler           /* DMA2 Stream 7                                   */
    .long     USART6_IRQHandler                 /* USART6                                           */
    .long     I2C3_EV_IRQHandler                /* I2C3 event                                             */
    .long     I2C3_ER_IRQHandler                /* I2C3 error                                             */
    .long     OTG_HS_EP1_OUT_IRQHandler         /* USB OTG HS End Point 1 Out                      */
    .long     OTG_HS_EP1_IN_IRQHandler          /* USB OTG HS End Point 1 In                       */
    .long     OTG_HS_WKUP_IRQHandler            /* USB OTG HS Wakeup through EXTI                         */
    .long     OTG_HS_IRQHandler                 /* USB OTG HS                                      */
    .long     DCMI_IRQHandler                   /* DCMI                                            */
    .long     CRYP_IRQHandler                   /* CRYP crypto                                     */
    .long     HASH_RNG_IRQHandler               /* Hash and Rng*/
    .long     FPU_IRQHandler                    /* FPU*/


    .size   __cs3_interrupt_vector_cortex_m, . - __cs3_interrupt_vector_cortex_m




/* Reset Handler */

    .section .cs3.reset,"x",%progbits
    .globl  __cs3_reset_cortex_m
    .type   __cs3_reset_cortex_m, %function
__cs3_reset_cortex_m:
    .fnstart

/* set stack pointer */
    LDR     R0, =__cs3_stack;
    MOV     sp, R0




/* Copy the data segment initializers from flash to SRAM */  
  	movs  r1, #0
  	b  LoopCopyDataInit

CopyDataInit:
  	ldr  r3, =_sidata
 	ldr  r3, [r3, r1]
  	str  r3, [r0, r1]
  	adds  r1, r1, #4
    
LoopCopyDataInit:
  	ldr  r0, =__cs3_region_start_ram
  	ldr  r3, =_edata
  	adds  r2, r0, r1
  	cmp  r2, r3
  	bcc  CopyDataInit



  	ldr  r2, =__bss_start__
  	b  LoopFillZerobss
/* Zero fill the bss segment. */  
FillZerobss:
  	movs  r3, #0
  	str  r3, [r2], #4
    
LoopFillZerobss:
  	ldr  r3, = __bss_end__
  	cmp  r2, r3
  	bcc  FillZerobss

/* main needs to be linked close to this for the branch to work */
    B main




    .pool
    .cantunwind
    .fnend
    .size   __cs3_reset_cortex_m,.-__cs3_reset_cortex_m

    .section ".text"

/* Exception Handlers */

    .weak   NMI_Handler
    .type   NMI_Handler, %function
NMI_Handler:
    B       .
    .size   NMI_Handler, . - NMI_Handler

    .weak   HardFault_Handler
    .type   HardFault_Handler, %function
HardFault_Handler:
    B       .
    .size   HardFault_Handler, . - HardFault_Handler

    .weak   MemManage_Handler
    .type   MemManage_Handler, %function
MemManage_Handler:
    B       .
    .size   MemManage_Handler, . - MemManage_Handler

    .weak   BusFault_Handler
    .type   BusFault_Handler, %function
BusFault_Handler:
    B       .
    .size   BusFault_Handler, . - BusFault_Handler

    .weak   UsageFault_Handler
    .type   UsageFault_Handler, %function
UsageFault_Handler:
    B       .
    .size   UsageFault_Handler, . - UsageFault_Handler

    .weak   SVC_Handler
    .type   SVC_Handler, %function
SVC_Handler:
    B       .
    .size   SVC_Handler, . - SVC_Handler

    .weak   DebugMon_Handler
    .type   DebugMon_Handler, %function
DebugMon_Handler:
    B       .
    .size   DebugMon_Handler, . - DebugMon_Handler

    .weak   PendSV_Handler
    .type   PendSV_Handler, %function
PendSV_Handler:
    B       .
    .size   PendSV_Handler, . - PendSV_Handler

    .weak   SysTick_Handler
    .type   SysTick_Handler, %function
SysTick_Handler:
    B       .
    .size   SysTick_Handler, . - SysTick_Handler


/* IRQ Handlers */

    .globl  Default_Handler
    .type   Default_Handler, %function
Default_Handler:
    B       .
    .size   Default_Handler, . - Default_Handler

    .macro  IRQ handler
/* cause the linker to replace it */
    .weak   \handler
    .set    \handler, Default_Handler
    .endm

    IRQ     DEF_IRQHandler
    IRQ     WWDG_IRQHandler
    IRQ     PVD_IRQHandler  				  
    IRQ     TAMP_STAMP_IRQHandler			  
    IRQ     RTC_WKUP_IRQHandler 			  
    IRQ     FLASH_IRQHandler				  
    IRQ     RCC_IRQHandler  				  
    IRQ     EXTI0_IRQHandler				  
    IRQ     EXTI1_IRQHandler				  
    IRQ     EXTI2_IRQHandler				  
    IRQ     EXTI3_IRQHandler				  
    IRQ     EXTI4_IRQHandler				  
    IRQ     DMA1_Stream0_IRQHandler 		  
    IRQ     DMA1_Stream1_IRQHandler 		  
    IRQ     DMA1_Stream2_IRQHandler 		  
    IRQ     DMA1_Stream3_IRQHandler 		  
    IRQ     DMA1_Stream4_IRQHandler 		  
    IRQ     DMA1_Stream5_IRQHandler 		  
    IRQ     DMA1_Stream6_IRQHandler 		  
    IRQ     ADC_IRQHandler  				  
    IRQ     CAN1_TX_IRQHandler  			  
    IRQ     CAN1_RX0_IRQHandler 			  
    IRQ     CAN1_RX1_IRQHandler 			  
    IRQ     CAN1_SCE_IRQHandler 			  
    IRQ     EXTI9_5_IRQHandler  			  
    IRQ     TIM1_BRK_TIM9_IRQHandler		  
    IRQ     TIM1_UP_TIM10_IRQHandler		  
    IRQ     TIM1_TRG_COM_TIM11_IRQHandler	  
    IRQ     TIM1_CC_IRQHandler  			  
    IRQ     TIM2_IRQHandler 				  
    IRQ     TIM3_IRQHandler 				  
    IRQ     TIM4_IRQHandler 				  
    IRQ     I2C1_EV_IRQHandler  			  
    IRQ     I2C1_ER_IRQHandler  			  
    IRQ     I2C2_EV_IRQHandler  			  
    IRQ     I2C2_ER_IRQHandler  			  
    IRQ     SPI1_IRQHandler 				  
    IRQ     SPI2_IRQHandler 				  
    IRQ     USART1_IRQHandler				  
    IRQ     USART2_IRQHandler				  
    IRQ     USART3_IRQHandler				  
    IRQ     EXTI15_10_IRQHandler			  
    IRQ     RTC_Alarm_IRQHandler			  
    IRQ     OTG_FS_WKUP_IRQHandler  		  
    IRQ     TIM8_BRK_TIM12_IRQHandler		  
    IRQ     TIM8_UP_TIM13_IRQHandler		  
    IRQ     TIM8_TRG_COM_TIM14_IRQHandler	  
    IRQ     TIM8_CC_IRQHandler  			  
    IRQ     DMA1_Stream7_IRQHandler 		  
    IRQ     FSMC_IRQHandler 				  
    IRQ     SDIO_IRQHandler 				  
    IRQ     TIM5_IRQHandler 				  
    IRQ     SPI3_IRQHandler 				  
    IRQ     UART4_IRQHandler				  
    IRQ     UART5_IRQHandler				  
    IRQ     TIM6_DAC_IRQHandler 			  
    IRQ     TIM7_IRQHandler 				  
    IRQ     DMA2_Stream0_IRQHandler 		  
    IRQ     DMA2_Stream1_IRQHandler 		  
    IRQ     DMA2_Stream2_IRQHandler 		  
    IRQ     DMA2_Stream3_IRQHandler 		  
    IRQ     DMA2_Stream4_IRQHandler 		  
    IRQ     ETH_IRQHandler  				  
    IRQ     ETH_WKUP_IRQHandler 			  
    IRQ     CAN2_TX_IRQHandler  			  
    IRQ     CAN2_RX0_IRQHandler 			  
    IRQ     CAN2_RX1_IRQHandler 			  
    IRQ     CAN2_SCE_IRQHandler 			  
    IRQ     OTG_FS_IRQHandler				  
    IRQ     DMA2_Stream5_IRQHandler 		  
    IRQ     DMA2_Stream6_IRQHandler 		  
    IRQ     DMA2_Stream7_IRQHandler 		  
	IRQ		USART6_IRQHandler
    IRQ     I2C3_EV_IRQHandler  			  
    IRQ     I2C3_ER_IRQHandler  			  
    IRQ     OTG_HS_EP1_OUT_IRQHandler		  
    IRQ     OTG_HS_EP1_IN_IRQHandler		  
    IRQ     OTG_HS_WKUP_IRQHandler  		  
    IRQ     OTG_HS_IRQHandler				  
    IRQ     DCMI_IRQHandler 				  
    IRQ     CRYP_IRQHandler 				  
    IRQ     HASH_RNG_IRQHandler 			  
    IRQ     FPU_IRQHandler  				  
    .end
