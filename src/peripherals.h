#ifndef _VVC_PERIPHS_H
#define _VVC_PERIPHS_H

#include "global.h"

/* Timer Peripherals */
void stop_timer(TIM_TypeDef *TIMx);
void start_timer(TIM_TypeDef *TIMx,
                 uint16_t prescaler,
                 uint16_t period,
                 uint8_t  with_interrupt);

/* I2C Peripheral */
void i2c_initialize(I2C_TypeDef *I2Cx,
                    uint32_t timing_value);
void i2c_set_addr(I2C_TypeDef *I2Cx,
                  uint8_t addr);
void i2c_start(I2C_TypeDef *I2Cx);
void i2c_stop(I2C_TypeDef *I2Cx);
void i2c_set_num_bytes(I2C_TypeDef *I2Cx,
                       uint8_t nbytes);
void i2c_write_byte(I2C_TypeDef *I2Cx,
                    uint8_t dat);
uint8_t i2c_read_byte(I2C_TypeDef *I2Cx);
void i2c_write_command(I2C_TypeDef *I2Cx,
                       uint8_t cmd);
void i2c_write_data_byte(I2C_TypeDef *I2Cx,
                         uint8_t dat);
void i2c_stream_framebuffer(I2C_TypeDef *I2Cx);

#endif
