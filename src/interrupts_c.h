#ifndef _VVC_INTERRUPTS_C_H
#define _VVC_INTERRUPTS_C_H

#include "global.h"

#include "peripherals.h"
#include "util_c.h"

// C-language hardware interrupt method signatures.
// Different chips have different NVIC definitions,
// so we should define each line's functionality in
// its own method which is common across chips.
// TODO: Should these be declared as explicitly inline?
void EXTI0_line_interrupt(void);
void EXTI1_line_interrupt(void);
void EXTI2_line_interrupt(void);
void EXTI3_line_interrupt(void);
void EXTI4_line_interrupt(void);
void EXTI5_line_interrupt(void);
void EXTI6_line_interrupt(void);
void EXTI7_line_interrupt(void);
void EXTI8_line_interrupt(void);
void EXTI9_line_interrupt(void);
void EXTI10_line_interrupt(void);
void EXTI11_line_interrupt(void);
void EXTI12_line_interrupt(void);
void EXTI13_line_interrupt(void);
void EXTI14_line_interrupt(void);
void EXTI15_line_interrupt(void);

#ifdef VVC_F0
// STM32F0xx EXTI lines.
// EXTI handler for interrupt lines 0-1.
void EXTI0_1_IRQ_handler(void);
// EXTI handler for interrupt lines 2-3.
void EXTI2_3_IRQ_handler(void);
// EXTI handler for interrupt lines 4-15.
void EXTI4_15_IRQ_handler(void);
#elif VVC_F3
// STM32F3xx(?) EXTI lines.
// EXTI handler for interrupt line 0.
// (Unused)
// EXTI handler for interrupt line 1.
// (Unused)
// EXTI handler for interrupt line 2.
void EXTI2_touchsense_IRQ_handler(void);
// EXTI handler for interrupt line 3.
void EXTI3_IRQ_handler(void);
// EXTI handler for interrupt line 4.
void EXTI4_IRQ_handler(void);
// EXTI handler for interrupt lines 5-9.
void EXTI5_9_IRQ_handler(void);
// EXTI handler for interrupt lines 10-15.
// (Unused)
#endif

// Handlers common to all supported lines of chip.
void TIM2_IRQ_handler(void);
void TIM16_IRQ_handler(void);

#endif
