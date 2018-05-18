#include "main.h"

/**
 * Main program.
 */
int main(void) {
  // Initial clock setup.
  #ifdef VVC_F0
    // Reset the Flash 'Access Control Register', and
    // then set 1 wait-state and enable the prefetch buffer.
    FLASH->ACR &= ~(0x0000001F);
    FLASH->ACR |=  (FLASH_ACR_LATENCY |
                    FLASH_ACR_PRFTBE);
    // Configure the PLL to (HSI / 2) * 12 = 48MHz.
    // Use a PLLMUL of 0xA for *12, and keep PLLSRC at 0
    // to use (HSI / PREDIV) as the core source. HSI = 8MHz.
    RCC->CFGR  &= ~(RCC_CFGR_PLLMUL |
                    RCC_CFGR_PLLSRC);
    RCC->CFGR  |=  (RCC_CFGR_PLLSRC_HSI_DIV2 |
                    RCC_CFGR_PLLMUL12);
    // Turn the PLL on and wait for it to be ready.
    RCC->CR    |=  (RCC_CR_PLLON);
    while (!(RCC->CR & RCC_CR_PLLRDY)) {};
    // Select the PLL as the system clock source.
    RCC->CFGR  &= ~(RCC_CFGR_SW);
    RCC->CFGR  |=  (RCC_CFGR_SW_PLL);
    while (!(RCC->CFGR & RCC_CFGR_SWS_PLL)) {};
  #elif VVC_F3
    // TODO
  #endif

  // Define starting values for global variables.
  uled_state = 0;
  game_state = GAME_STATE_MAIN_MENU;
  main_menu_state = MAIN_MENU_STATE_START;
  should_tick = 0;
  state_changed = 1;
  fast_tick_timer_on = 0;
  left_right_fast_tick = 0;
  tetris_score = 0;
  tetris_level = 0;
  game_tick_prescaler = 1024;
  game_tick_period = 46785;
  cur_block_type = TBRICK_I;
  next_block_type = TBRICK_I;
  cur_block_x = 4;
  cur_block_y = -1;
  cur_block_r = 0;
  // Empty the tetris grid, to start.
  uint8_t grid_x_i = 0;
  uint8_t grid_y_i = 0;
  for (grid_x_i = 0; grid_x_i < 10; ++grid_x_i) {
    for (grid_y_i = 0; grid_y_i < 20; ++grid_y_i) {
      tetris_grid[grid_x_i][grid_y_i] = TGRID_EMPTY;
    }
  }

  // Enable the GPIOA clock (buttons on pins A2-A7,
  // user LED on pin A12).
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
  // Enable the GPIOB clock (I2C1 used on pins B6/B7,
  // buzzer on pin B0).
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
  // Enable the TIM2 clock.
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  // Enable the TIM3 clock.
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
  // Enable the TIM16 clock.
  RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;
  // Enable the I2C1 clock.
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
  // Enable the SYSCFG clock for hardware interrupts.
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

  // Start the TIM14 clock to count rapidly.
  // This will be a rudimentary PRNG.
  start_timer(TIM3, 0, 0xFFFF, 0);

  // Setup GPIO pins A6, A7, A8, A9, B0, and B1 as inputs
  // with pullups, low-speed.
  GPIOA->MODER   &= ~(GPIO_MODER_MODER6 |
                      GPIO_MODER_MODER7 |
                      GPIO_MODER_MODER8 |
                      GPIO_MODER_MODER9);
  GPIOB->MODER   &= ~(GPIO_MODER_MODER0  |
                      GPIO_MODER_MODER1);
  GPIOA->PUPDR   &= ~(GPIO_PUPDR_PUPDR6 |
                      GPIO_PUPDR_PUPDR7 |
                      GPIO_PUPDR_PUPDR8 |
                      GPIO_PUPDR_PUPDR9);
  GPIOB->PUPDR   &= ~(GPIO_PUPDR_PUPDR0  |
                      GPIO_PUPDR_PUPDR1);
  GPIOA->PUPDR   |=  ((1 << GPIO_PUPDR_PUPDR6_Pos) |
                      (1 << GPIO_PUPDR_PUPDR7_Pos) |
                      (1 << GPIO_PUPDR_PUPDR8_Pos) |
                      (1 << GPIO_PUPDR_PUPDR9_Pos));
  GPIOB->PUPDR   |=  ((1 << GPIO_PUPDR_PUPDR0_Pos) |
                      (1 << GPIO_PUPDR_PUPDR1_Pos));

  // Setup GPIO pins A10, A11, A12, and A15 as push-pull output,
  // no pupdr, 10MHz max speed.
  GPIOA->MODER   &= ~(GPIO_MODER_MODER10 |
                      GPIO_MODER_MODER11 |
                      GPIO_MODER_MODER12 |
                      GPIO_MODER_MODER15);
  GPIOA->MODER   |=  (1 << GPIO_MODER_MODER10_Pos |
                      1 << GPIO_MODER_MODER11_Pos |
                      1 << GPIO_MODER_MODER12_Pos |
                      1 << GPIO_MODER_MODER15_Pos);
  GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEEDR10 |
                      GPIO_OSPEEDR_OSPEEDR11 |
                      GPIO_OSPEEDR_OSPEEDR12 |
                      GPIO_OSPEEDR_OSPEEDR15);
  GPIOA->OSPEEDR |=  (1 << GPIO_OSPEEDR_OSPEEDR10_Pos |
                      1 << GPIO_OSPEEDR_OSPEEDR11_Pos |
                      1 << GPIO_OSPEEDR_OSPEEDR12_Pos |
                      1 << GPIO_OSPEEDR_OSPEEDR15_Pos);
  GPIOA->OTYPER  &= ~(GPIO_OTYPER_OT_10 |
                      GPIO_OTYPER_OT_11 |
                      GPIO_OTYPER_OT_12 |
                      GPIO_OTYPER_OT_15);
  GPIOA->PUPDR   &= ~(GPIO_PUPDR_PUPDR10 |
                      GPIO_PUPDR_PUPDR11 |
                      GPIO_PUPDR_PUPDR12 |
                      GPIO_PUPDR_PUPDR15);
  // Setup GPIO pins B3, B4, B5 as push-pull output,
  // no pupdr, 50MHz max speed.
  GPIOB->MODER   &= ~(GPIO_MODER_MODER3 |
                      GPIO_MODER_MODER4 |
                      GPIO_MODER_MODER5);
  GPIOB->MODER   |=  (1 << GPIO_MODER_MODER3_Pos |
                      1 << GPIO_MODER_MODER4_Pos |
                      1 << GPIO_MODER_MODER5_Pos);
  GPIOB->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEEDR3 |
                      GPIO_OSPEEDR_OSPEEDR4 |
                      GPIO_OSPEEDR_OSPEEDR5);
  GPIOB->OSPEEDR |=  (0x3 << GPIO_OSPEEDR_OSPEEDR3_Pos |
                      0x3 << GPIO_OSPEEDR_OSPEEDR4_Pos |
                      0x3 << GPIO_OSPEEDR_OSPEEDR5_Pos);
  GPIOB->OTYPER  &= ~(GPIO_OTYPER_OT_3 |
                      GPIO_OTYPER_OT_4 |
                      GPIO_OTYPER_OT_5);
  GPIOB->PUPDR   &= ~(GPIO_PUPDR_PUPDR3 |
                      GPIO_PUPDR_PUPDR4 |
                      GPIO_PUPDR_PUPDR5);

  // Initialize the SSD1331 OLED display.
  GPIOA->ODR &= ~(1 << PA_CS);
  GPIOB->ODR |=  (1 << PB_DC);
  GPIOA->ODR &= ~(1 << PA_RST);
  delay_ms(150);
  GPIOA->ODR |=  (1 << PA_RST);
  delay_ms(150);
  ssd1331_start_sequence();

  // Setup hardware interrupts on the EXTI lines associated
  // with the 6 button inputs.
  // Pins B0, B1 use the EXTI0_1 interrupt.
  // Pins A6, A7, A8, and A9 use the EXTI4_15 interrupt.
  // Map EXTI lines to the GPIO port.
  SYSCFG->EXTICR[0] &= ~(SYSCFG_EXTICR1_EXTI0);
  SYSCFG->EXTICR[0] |=  (SYSCFG_EXTICR1_EXTI0_PB);
  SYSCFG->EXTICR[0] &= ~(SYSCFG_EXTICR1_EXTI1);
  SYSCFG->EXTICR[0] |=  (SYSCFG_EXTICR1_EXTI1_PB);
  SYSCFG->EXTICR[1] &= ~(SYSCFG_EXTICR2_EXTI6);
  SYSCFG->EXTICR[1] |=  (SYSCFG_EXTICR2_EXTI6_PA);
  SYSCFG->EXTICR[1] &= ~(SYSCFG_EXTICR2_EXTI7);
  SYSCFG->EXTICR[1] |=  (SYSCFG_EXTICR2_EXTI7_PA);
  SYSCFG->EXTICR[1] &= ~(SYSCFG_EXTICR3_EXTI8);
  SYSCFG->EXTICR[1] |=  (SYSCFG_EXTICR3_EXTI8_PA);
  SYSCFG->EXTICR[1] &= ~(SYSCFG_EXTICR3_EXTI9);
  SYSCFG->EXTICR[1] |=  (SYSCFG_EXTICR3_EXTI9_PA);
  // Setup the EXTI interrupt lines as 'falling edge' interrupts.
  EXTI->IMR  |=  (EXTI_IMR_MR0);
  EXTI->RTSR &= ~(EXTI_RTSR_TR0);
  EXTI->FTSR |=  (EXTI_FTSR_TR0);
  EXTI->IMR  |=  (EXTI_IMR_MR1);
  EXTI->RTSR &= ~(EXTI_RTSR_TR1);
  EXTI->FTSR |=  (EXTI_FTSR_TR1);
  EXTI->IMR  |=  (EXTI_IMR_MR6);
  EXTI->RTSR &= ~(EXTI_RTSR_TR6);
  EXTI->FTSR |=  (EXTI_FTSR_TR6);
  EXTI->IMR  |=  (EXTI_IMR_MR7);
  EXTI->RTSR &= ~(EXTI_RTSR_TR7);
  EXTI->FTSR |=  (EXTI_FTSR_TR7);
  EXTI->IMR  |=  (EXTI_IMR_MR8);
  EXTI->RTSR &= ~(EXTI_RTSR_TR8);
  EXTI->FTSR |=  (EXTI_FTSR_TR8);
  EXTI->IMR  |=  (EXTI_IMR_MR9);
  EXTI->RTSR &= ~(EXTI_RTSR_TR9);
  EXTI->FTSR |=  (EXTI_FTSR_TR9);

  // The HAL 'cortex' libraries basically just call these
  // core functions for NVIC stuff, anyways:
  #ifdef VVC_F0
    NVIC_SetPriority(EXTI0_1_IRQn, 0x03);
    NVIC_EnableIRQ(EXTI0_1_IRQn);
    NVIC_SetPriority(EXTI4_15_IRQn, 0x03);
    NVIC_EnableIRQ(EXTI4_15_IRQn);
  #elif VVC_F3
    // On Cortex-M4 cores, we need to set an NVIC priority
    // grouping and subpriorities as well as normal priorities:
    // 0x07: 0 Pri / 4 SubPri
    // 0x06: 1 Pri / 3 SubPri
    // 0x05: 2 Pri / 2 SubPri
    // 0x04: 3 Pri / 1 SubPri
    // 0x03: 4 Pri / 0 SubPri
    // Use 2 bits for 'priority' and 2 bits for 'subpriority'.
    NVIC_SetPriorityGrouping(0x05);
    uint32_t exti_pri_encoding = NVIC_EncodePriority(0x05, 0x03, 0x03);
    // (TODO)
    NVIC_SetPriority(EXTI9_5_IRQn, exti_pri_encoding);
    NVIC_EnableIRQ(EXTI9_5_IRQn);
  #endif

  // Enable the NVIC interrupt for TIM2 and TIM16.
  // (Timer peripheral initialized and used elsewhere)
  NVIC_SetPriority(TIM2_IRQn, 0x03);
  NVIC_EnableIRQ(TIM2_IRQn);
  NVIC_SetPriority(TIM16_IRQn, 0x03);
  NVIC_EnableIRQ(TIM16_IRQn);

  while (1) {
    // Tick the game state if necessary.
    if (should_tick) {
      tetris_game_tick();
      should_tick = 0;
      state_changed = 1;
    }

    if (state_changed) {
      state_changed = 0;
      // Draw the current frame based on the game's state.
      if (game_state == GAME_STATE_MAIN_MENU) {
        draw_main_menu();
      }
      else if (game_state == GAME_STATE_IN_GAME) {
        draw_tetris_game();
      }
      else if (game_state == GAME_STATE_PAUSED) {
        // (TODO)
      }
      else if (game_state == GAME_STATE_GAME_OVER) {
        draw_game_over();
      }
      else {
        oled_draw_rect(0, 0, 128, 64, 0, 1);
      }
      // Communicate the framebuffer to the OLED screen.
      sspi_stream_framebuffer();
    }

    // Set the onboard LED if the variable is set.
    if (uled_state) {
      GPIOA->ODR |=  (GPIO_ODR_11);
    }
    else {
      GPIOA->ODR &= ~(GPIO_ODR_11);
    }
  }
  return 0;
}
