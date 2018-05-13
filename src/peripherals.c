#include "peripherals.h"

/* Timer Peripherals */

/*
 * 'Start Timer'
 * This method starts a new 'counter' timer, which triggers
 * an 'UPDATE' interrupt every time that it triggers.
 * It should trigger every N seconds, where:
 * N = (period) / (core clock frequency / prescaler)
 */
void start_timer(TIM_TypeDef *TIMx,
                 uint16_t prescaler,
                 uint16_t period,
                 uint8_t  with_interrupt) {
  // Initialize the 'TIMx' timer peripheral. (TODO: F303 support)
  // Start by making sure the timer's 'counter' is off.
  TIMx->CR1 &= ~(TIM_CR1_CEN);
  // Next, reset the peripheral. (This is where a HAL can help)
  if (TIMx == TIM2) {
    RCC->APB1RSTR |=  (RCC_APB1RSTR_TIM2RST);
    RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM2RST);
  }
  else if (TIMx == TIM3) {
    RCC->APB1RSTR |=  (RCC_APB1RSTR_TIM3RST);
    RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM3RST);
  }
  // Set clock division to 1; the timer triggers every N events.
  // Also set the counter to count up.
  TIMx->CR1  &= ~(TIM_CR1_DIR |
                  TIM_CR1_CKD);
  // Set the 'prescaler'; the timer 'ticks' every N clock cycles.
  TIMx->PSC   =  prescaler;
  // Set 'auto-reload'; this is the timer's period in 'ticks'.
  TIMx->ARR   =  period;
  // Set the repetition counter to 0; I dunno, unused for now.
  TIMx->RCR   =  0;
  // Before enabling the counter, trigger an 'update' event to
  // make sure that the chip registers the 'prescaler' settings.
  TIMx->EGR  |=  (TIM_EGR_UG);
  if (with_interrupt) {
    // Enable the 'update' timer event/interrupt.
    TIMx->DIER |=  (TIM_DIER_UIE);
  }
  // Finally, enable the timer.
  TIMx->CR1  |=  (TIM_CR1_CEN);
}

/*
 * Stop a running timer on a given Timer peripheral.
 */
void stop_timer(TIM_TypeDef *TIMx) {
  // Turn off the timer's 'counter'.
  TIMx->CR1 &= ~(TIM_CR1_CEN);
  // Clear the 'pending update interrupt' flag.
  TIMx->SR  &= ~(TIM_SR_UIF);
}

/*
 * Initialize the I2C peripheral with a given timing value.
 */
void i2c_initialize(I2C_TypeDef *I2Cx,
                    uint32_t timing_value) {
  // Initialize the I2C peripheral.
  // First, disable the peripheral.
  I2Cx->CR1     &= ~(I2C_CR1_PE);
  // Clear some 'CR1' bits.
  I2Cx->CR1     &= ~( I2C_CR1_DNF    |
                      I2C_CR1_ANFOFF |
                      I2C_CR1_SMBHEN |
                      I2C_CR1_SMBDEN );
  // Clear some 'CR2' bits.
  I2Cx->CR2     &= ~( I2C_CR2_RD_WRN  |
                      I2C_CR2_NACK    |
                      I2C_CR2_RELOAD  |
                      I2C_CR2_AUTOEND );
  // Clear all 'ICR' flags.
  I2Cx->ICR     |=  ( I2C_ICR_ADDRCF   |
                      I2C_ICR_NACKCF   |
                      I2C_ICR_STOPCF   |
                      I2C_ICR_BERRCF   |
                      I2C_ICR_ARLOCF   |
                      I2C_ICR_OVRCF    |
                      I2C_ICR_PECCF    |
                      I2C_ICR_TIMOUTCF |
                      I2C_ICR_ALERTCF  );
  // Configure I2C timing.
  // Reset all but the reserved bits.
  I2Cx->TIMINGR &=  (0x0F000000);
  // (100KHz @48MHz core clock, according to an application note)
  I2Cx->TIMINGR |=  (timing_value);
  // Enable the peripheral. (PE = 'Peripheral Enable')
  I2Cx->CR1     |=  I2C_CR1_PE;
}

/*
 * Set the I2C address of the device to communicate with.
 */
inline void i2c_set_addr(I2C_TypeDef *I2Cx,
                  uint8_t addr) {
  I2Cx->CR2 &= ~(I2C_CR2_SADD);
  I2Cx->CR2 |=  (addr << I2C_CR2_SADD_Pos);
}

/*
 * Send a 'start' condition on the I2C bus.
 */
inline void i2c_start(I2C_TypeDef *I2Cx) {
  // Send 'Start' condition, and wait for acknowledge.
  I2Cx->CR2 |=  (I2C_CR2_START);
  while ((I2Cx->CR2 & I2C_CR2_START)) {}
}

/*
 * Send a 'stop' condition on the I2C bus.
 */
inline void i2c_stop(I2C_TypeDef *I2Cx) {
  // Send 'Stop' condition, and wait for acknowledge.
  I2Cx->CR2 |=  (I2C_CR2_STOP);
  while ((I2Cx->CR2 & I2C_CR2_STOP)) {}
  // Reset the ICR ('Interrupt Clear Register') event flag.
  I2Cx->ICR |=  (I2C_ICR_STOPCF);
  while ((I2Cx->ICR & I2C_ICR_STOPCF)) {}
}

/*
 * Set the number of bytes to send/receive over the I2C bus.
 */
inline void i2c_set_num_bytes(I2C_TypeDef *I2Cx,
                              uint8_t nbytes) {
  I2Cx->CR2 &= ~(I2C_CR2_NBYTES);
  I2Cx->CR2 |=  (nbytes << I2C_CR2_NBYTES_Pos);
}

/*
 * Write a byte of data on the I2C bus.
 */
inline void i2c_write_byte(I2C_TypeDef *I2Cx,
                           uint8_t dat) {
  // Transmit a byte of data, and wait for it to send.
  I2Cx->TXDR = (I2Cx->TXDR & 0xFFFFFF00) | dat;
  while (!(I2Cx->ISR & (I2C_ISR_TXIS |
                        I2C_ISR_TC |
                        I2C_ISR_TCR))) {}
}

/*
 * Read a byte of data on the I2C bus.
 */
inline uint8_t i2c_read_byte(I2C_TypeDef *I2Cx) {
  // Wait for a byte of data to be available, then read it.
  while (!(I2Cx->ISR & I2C_ISR_RXNE)) {}
  return (I2Cx->RXDR & 0xFF);
}

/*
 * Write a single command byte over I2C.
 */
void i2c_write_command(I2C_TypeDef *I2Cx,
                       uint8_t cmd) {
  // On the I2C bus, the first byte of the transmission
  // indicates D/C; 0x00 for 'Command', 0x40 for 'Data'.
  i2c_set_num_bytes(I2Cx, 2);
  i2c_start(I2Cx);
  i2c_write_byte(I2Cx, 0x00);
  i2c_write_byte(I2Cx, cmd);
  i2c_stop(I2Cx);
}

/*
 * Write a single data byte over I2C.
 */
void i2c_write_data_byte(I2C_TypeDef *I2Cx,
                         uint8_t dat) {
  // On the I2C bus, the first byte of the transmission
  // indicates D/C; 0x00 for 'Command', 0x40 for 'Data'.
  i2c_set_num_bytes(I2Cx, 2);
  i2c_start(I2Cx);
  i2c_write_byte(I2Cx, 0x40);
  i2c_write_byte(I2Cx, dat);
  i2c_stop(I2Cx);
}

inline void i2c_stream_framebuffer(I2C_TypeDef *I2Cx) {
  // Stream the entire framebuffer in one transmission.
  // 1 byte for 'Data/Command', 1024 bytes for the FB.
  // The NBYTES register can only hold 127 max, so we'll
  // need to reload that every page or two.
  uint8_t nbytes_i = 0;
  uint8_t buf_i = 0;
  uint16_t buf_prog = 0;
  // Also, set the 'RELOAD' flag.
  I2Cx->CR2 |= I2C_CR2_RELOAD;
  i2c_set_num_bytes(I2Cx, 127);
  i2c_start(I2Cx);
  // 'Data'
  i2c_write_byte(I2Cx, 0x40);
  // The first 126 bytes.
  for (nbytes_i = 0; nbytes_i < 126; ++nbytes_i) {
    i2c_write_byte(I2Cx, oled_fb[nbytes_i]);
  }
  buf_prog += 126;
  // The next 127 bytes, 7 times.
  for (buf_i = 0; buf_i < 7; ++buf_i) {
    i2c_set_num_bytes(I2Cx, 127);
    for (nbytes_i = 0; nbytes_i < 127; ++nbytes_i) {
      i2c_write_byte(I2Cx, oled_fb[buf_prog + nbytes_i]);
    }
    buf_prog += 127;
  }
  // Finally, the last 9 bytes.
  i2c_set_num_bytes(I2Cx, 9);
  for (nbytes_i = 0; nbytes_i < 9; ++nbytes_i) {
    i2c_write_byte(I2Cx, oled_fb[buf_prog + nbytes_i]);
  }
  i2c_stop(I2Cx);
  // Un-set the 'RELOAD' flag.
  I2Cx->CR2 |= I2C_CR2_RELOAD;
}
