#include "interrupts_c.h"

// C-language hardware interrupt method definitions.
/*
 * Each EXTI line between 0-15 can map to a GPIO pin.
 * The EXTI line number is the same as the pin number, and
 * each pin number can only have its interrupt active on
 * ONE (1) of its GPIO banks. So for example, you can have
 * an interrupt on pin A0 (EXTI line 0) and pin B1 (EXTI line 1)
 * at the same time, but not on pins A1 (EXTI line 1)
 * and B1 (EXTI line 1), since they share the same interrupt line.
 */
// Common definitions for each line, independent of
// available NVIC interrupts on any specific chip.
inline void EXTI0_line_interrupt(void) {
  // 'Down' button.
  if (game_state == GAME_STATE_IN_GAME) {
    // Drop the block by one grid coordinate if able.
    should_tick = 1;
    if (!fast_tick_timer_on) {
      fast_tick_timer_on = 1;
      // Start the timer for continuing to drop bricks.
      start_timer(TIM16, FAST_DROP_TIM_PRE, FAST_DROP_TIM_ARR, 1);
    }
  }
}

inline void EXTI1_line_interrupt(void) {
  // 'Right' button.
  if (game_state == GAME_STATE_IN_GAME) {
    // Move the brick right, if possible.
    if (!check_brick_pos(cur_block_x+1, cur_block_y)) {
      cur_block_x += 1;
      state_changed = 1;
      if (!fast_tick_timer_on) {
        fast_tick_timer_on = 1;
        // Start the timer for continuing to drop bricks.
        start_timer(TIM16, FAST_DROP_TIM_PRE, FAST_DROP_TIM_ARR, 1);
      }
    }
  }
}

inline void EXTI2_line_interrupt(void) {
}

inline void EXTI3_line_interrupt(void) {
}

inline void EXTI4_line_interrupt(void) {
}

inline void EXTI5_line_interrupt(void) {
}

inline void EXTI6_line_interrupt(void) {
  // 'Left' button.
  if (game_state == GAME_STATE_IN_GAME) {
    // Move the brick left, if possible.
    if (!check_brick_pos(cur_block_x-1, cur_block_y)) {
      cur_block_x -= 1;
      state_changed = 1;
      if (!fast_tick_timer_on) {
        fast_tick_timer_on = 1;
        // Start the timer for continuing to drop bricks.
        start_timer(TIM16, FAST_DROP_TIM_PRE, FAST_DROP_TIM_ARR, 1);
      }
    }
  }
}

inline void EXTI7_line_interrupt(void) {
  // 'Up' button.
  if (game_state == GAME_STATE_IN_GAME) {
    // For now, 'Up' pauses the game.
    game_state = GAME_STATE_PAUSED;
    stop_timer(TIM2);
  }
  else if (game_state == GAME_STATE_PAUSED) {
    game_state = GAME_STATE_IN_GAME;
    start_timer(TIM2, game_tick_prescaler, game_tick_period, 1);
    state_changed = 1;
  }
}

inline void EXTI8_line_interrupt(void) {
  // 'B' button.
  if (game_state == GAME_STATE_MAIN_MENU) {
  }
  else if (game_state == GAME_STATE_IN_GAME) {
    // Rotate the brick clockwise, if able.
    if (!check_brick_rot((cur_block_r + 3) % 4)) {
      cur_block_r = (cur_block_r + 3) % 4;
      if (cur_block_r < 0) { cur_block_r = -cur_block_r; }
      state_changed = 1;
    }
  }
  else if (game_state == GAME_STATE_PAUSED) {
  }
  else if (game_state == GAME_STATE_GAME_OVER) {
    // Either the 'A' or 'B' button returns to the
    // main menu from a 'Game Over' screen.
    game_state = GAME_STATE_MAIN_MENU;
    main_menu_state = MAIN_MENU_STATE_START;
    uled_state = 0;
    stop_timer(TIM2);
    stop_timer(TIM16);
    reset_game_state();
    state_changed = 1;
  }
}

inline void EXTI9_line_interrupt(void) {
  // 'A' button.
  if (game_state == GAME_STATE_MAIN_MENU) {
    if (main_menu_state == MAIN_MENU_STATE_START) {
      // Start a new game!
      game_state = GAME_STATE_IN_GAME;
      uled_state = 0;
      // Set a PRNG-based first block type.
      uint8_t new_block_type = TIM3->CNT & 0x7;
      // (Valid block types are between [0:6])
      while (new_block_type == 7) { new_block_type = TIM3->CNT & 0x7; }
      cur_block_type = new_block_type;
      start_timer(TIM2, game_tick_prescaler, game_tick_period, 1);
      state_changed = 1;
    }
  }
  else if (game_state == GAME_STATE_IN_GAME) {
    // Rotate the brick counter-clockwise, if able.
    if (!check_brick_rot((cur_block_r + 1) % 4)) {
      cur_block_r = (cur_block_r + 1) % 4;
      state_changed = 1;
    }
  }
  else if (game_state == GAME_STATE_PAUSED) {
  }
  else if (game_state == GAME_STATE_GAME_OVER) {
    // Either the 'A' or 'B' button returns to the
    // main menu from a 'Game Over' screen.
    game_state = GAME_STATE_MAIN_MENU;
    main_menu_state = MAIN_MENU_STATE_START;
    uled_state = 0;
    stop_timer(TIM2);
    stop_timer(TIM16);
    reset_game_state();
    state_changed = 1;
  }
}

inline void EXTI10_line_interrupt(void) {
  // (Unused)
}

inline void EXTI11_line_interrupt(void) {
  // (Unused)
}

inline void EXTI12_line_interrupt(void) {
  // (Unused)
}

inline void EXTI13_line_interrupt(void) {
  // (Unused)
}

inline void EXTI14_line_interrupt(void) {
  // (Unused)
}

inline void EXTI15_line_interrupt(void) {
  // (Unused)
}

#ifdef VVC_F0
// STM32F0xx EXTI lines.
/*
 * EXTI0_1: Handle interrupt lines 0 and 1.
 */
void EXTI0_1_IRQ_handler(void) {
if (EXTI->PR & EXTI_PR_PR0) {
  EXTI->PR |= EXTI_PR_PR0;
  EXTI0_line_interrupt();
}
if (EXTI->PR & EXTI_PR_PR1) {
  EXTI->PR |= EXTI_PR_PR1;
  EXTI1_line_interrupt();
}
return;
}

/*
 * EXTI2_3: Handle interrupt lines 2 and 3.
 */
void EXTI2_3_IRQ_handler(void) {
if (EXTI->PR & EXTI_PR_PR2) {
  EXTI->PR |= EXTI_PR_PR2;
  EXTI2_line_interrupt();
}
if (EXTI->PR & EXTI_PR_PR3) {
  EXTI->PR |= EXTI_PR_PR3;
  EXTI3_line_interrupt();
}
return;
}

/*
 * EXTI4_15: Handle interrupt lines between [4:15], inclusive.
 */
void EXTI4_15_IRQ_handler(void) {
if (EXTI->PR & EXTI_PR_PR4) {
  EXTI->PR |= EXTI_PR_PR4;
  EXTI4_line_interrupt();
}
if (EXTI->PR & EXTI_PR_PR5) {
  EXTI->PR |= EXTI_PR_PR5;
  EXTI5_line_interrupt();
}
if (EXTI->PR & EXTI_PR_PR6) {
  EXTI->PR |= EXTI_PR_PR6;
  EXTI6_line_interrupt();
}
if (EXTI->PR & EXTI_PR_PR7) {
  EXTI->PR |= EXTI_PR_PR7;
  EXTI7_line_interrupt();
}
if (EXTI->PR & EXTI_PR_PR8) {
  EXTI->PR |= EXTI_PR_PR8;
  EXTI8_line_interrupt();
}
if (EXTI->PR & EXTI_PR_PR9) {
  EXTI->PR |= EXTI_PR_PR9;
  EXTI9_line_interrupt();
}
if (EXTI->PR & EXTI_PR_PR10) {
  EXTI->PR |= EXTI_PR_PR10;
  EXTI10_line_interrupt();
}
if (EXTI->PR & EXTI_PR_PR11) {
  EXTI->PR |= EXTI_PR_PR11;
  EXTI11_line_interrupt();
}
if (EXTI->PR & EXTI_PR_PR12) {
  EXTI->PR |= EXTI_PR_PR12;
  EXTI12_line_interrupt();
}
if (EXTI->PR & EXTI_PR_PR13) {
  EXTI->PR |= EXTI_PR_PR13;
  EXTI13_line_interrupt();
}
if (EXTI->PR & EXTI_PR_PR14) {
  EXTI->PR |= EXTI_PR_PR14;
  EXTI14_line_interrupt();
}
if (EXTI->PR & EXTI_PR_PR15) {
  EXTI->PR |= EXTI_PR_PR15;
  EXTI15_line_interrupt();
}
return;
}


#elif VVC_F3
// STM32F3xx(?) EXTI lines.
void EXTI2_touchsense_IRQ_handler(void) {
if (EXTI->PR & EXTI_PR_PR2) {
  EXTI->PR |= EXTI_PR_PR2;
  EXTI2_line_interrupt();
}
return;
}

void EXTI3_IRQ_handler(void) {
if (EXTI->PR & EXTI_PR_PR3) {
  EXTI->PR |= EXTI_PR_PR3;
  EXTI3_line_interrupt();
}
return;
}

void EXTI4_IRQ_handler(void) {
if (EXTI->PR & EXTI_PR_PR4) {
  EXTI->PR |= EXTI_PR_PR4;
  EXTI4_line_interrupt();
}
return;
}

void EXTI5_9_IRQ_handler(void) {
if (EXTI->PR & EXTI_PR_PR5) {
  EXTI->PR |= EXTI_PR_PR5;
  EXTI5_line_interrupt();
}
if (EXTI->PR & EXTI_PR_PR6) {
  EXTI->PR |= EXTI_PR_PR6;
  EXTI6_line_interrupt();
}
if (EXTI->PR & EXTI_PR_PR7) {
  EXTI->PR |= EXTI_PR_PR7;
  EXTI7_line_interrupt();
}
return;
}

#endif

// Interrupts common to all supported chips.
void TIM2_IRQ_handler(void) {
  // Handle a timer 'update' interrupt event
  if (TIM2->SR & TIM_SR_UIF) {
    TIM2->SR &= ~(TIM_SR_UIF);
    uled_state = !uled_state;
    if (game_state == GAME_STATE_IN_GAME) {
      should_tick = 1;
    }
  }
}

void TIM16_IRQ_handler(void) {
  // Handle a timer 'update' interrupt event
  if (TIM16->SR & TIM_SR_UIF) {
    TIM16->SR &= ~(TIM_SR_UIF);
    // Stop the timer if no relevant buttons are pressed.
    if ((GPIOB->IDR & GPIO_IDR_0) &&
        (GPIOB->IDR & GPIO_IDR_1) &&
        (GPIOA->IDR & GPIO_IDR_6)) {
      fast_tick_timer_on = 0;
      left_right_fast_tick = 0;
      stop_timer(TIM16);
    }
    else if (game_state == GAME_STATE_IN_GAME) {
      // If the game isn't paused, move the brick appropriately.
      left_right_fast_tick += 1;
      if (left_right_fast_tick >= 3) {
        if (!(GPIOB->IDR & GPIO_IDR_1)) {
          // Move the brick right, if possible.
          if (!check_brick_pos(cur_block_x+1, cur_block_y)) {
            cur_block_x += 1;
            state_changed = 1;
          }
        }
        if (!(GPIOA->IDR & GPIO_IDR_6)) {
          // Move the brick left, if possible.
          if (!check_brick_pos(cur_block_x-1, cur_block_y)) {
            cur_block_x -= 1;
            state_changed = 1;
          }
        }
        left_right_fast_tick = 0;
      }
      if (!(GPIOB->IDR & GPIO_IDR_0)) {
        // Drop the brick if necessary.
        should_tick = 1;
      }
    }
  }
}
