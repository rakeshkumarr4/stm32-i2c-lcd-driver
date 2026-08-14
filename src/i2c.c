#include "lcd.h"
#include "i2c.h"
#include "stm32f4xx.h"

/*
 * Stores the current I2C address of the LCD backpack.
 * This value is set once during LCD initialization.
 */
static uint8_t LCD_address = 0x23;

/*
 * Simple delay helper used during LCD startup initialization.
 * It is not a precise timer; it just provides enough time for the LCD
 * controller to complete power-up and command timing.
 */
void delay(volatile uint32_t count) {
    while (count--)
    {
        __NOP();
    }
}

/*
 * Writes one byte to the target I2C device.
 * The STM32 I2C peripheral must generate START, send the address, then transfer
 * the data byte. The STOP condition is asserted once the transfer is complete.
 */
I2C_Status_t I2C_WriteByte(uint8_t address, uint8_t data) {
  uint32_t timeout = 1000000;

  /* Step 1: generate START condition */
  I2C1->CR1 |= I2C_CR1_START;

  /* Wait until START bit is sent and the bus is ready for address transmission */
  while (!(I2C1->SR1 & I2C_SR1_SB)) {
    if (--timeout == 0) {
      I2C1->CR1 |= I2C_CR1_STOP;
      return I2C_ERROR;
    }
  }

  /* Step 2: send the 7-bit device address plus write bit */
  I2C1->DR = (address << 1);

  /* Wait until address is acknowledged and the slave address phase is complete */
  timeout = 1000000;
  while (!(I2C1->SR1 & I2C_SR1_ADDR)) {
    if (--timeout == 0) {
      I2C1->CR1 |= I2C_CR1_STOP;
      return I2C_ERROR;
    }
  }

  /* Clear the ADDR flag by reading SR1 then SR2 */
  volatile uint32_t temp = I2C1->SR1;
  temp = I2C1->SR2;

  /* Step 3: write the data byte */
  I2C1->DR = data;

  /*
   * Step 4: wait for EV8_2. In a master transmitter, the STOP should be
   * generated when either TXE or BTF is set at the end of the transfer.
   */
  timeout = 1000000;
  while (!(I2C1->SR1 & I2C_SR1_TXE) && !(I2C1->SR1 & I2C_SR1_BTF)) {
    if (--timeout == 0) {
      I2C1->CR1 |= I2C_CR1_STOP;
      return I2C_ERROR;
    }
  }

  /* End the transaction with STOP condition */
  I2C1->CR1 |= I2C_CR1_STOP;

  return I2C_OK;
}

/*
 * Initializes the LCD over I2C.
 * This function performs the startup sequence required by the HD44780 controller
 * when connected through an I2C backpack.
 */
void I2C_LCD_Init (uint8_t address, uint8_t num_of_lines, uint8_t num_of_columns) {
    LCD_address = address;

    /* Wait for stable power-up before sending commands */
    delay (400000);

    /* Enable the backlight first */
    hal_backlight_on();
    delay (200000);

    /* Start the Arduino-style LCD init sequence for HD44780 */
    hal_write_nibble(LCD_FUNCTION_RESET);
    delay (400000);
    hal_write_nibble(LCD_FUNCTION_RESET);
    delay (100000);
    hal_write_nibble(LCD_FUNCTION_RESET);
    delay (100000);
    hal_write_nibble(LCD_FUNCTION);
    delay (100000);

    /* Configure the LCD driver */
    LCD_Init(num_of_lines, num_of_columns);

    /* Set 2-line mode and enable the display controller */
    uint8_t cmd = LCD_FUNCTION | LCD_FUNCTION_2LINES;
    hal_write_command(cmd);
    hal_write_command(LCD_ON_CTRL);
    hal_write_command(LCD_CLR);
    delay(200000);

    /* Set entry mode: move cursor to the right after each character */
    hal_write_command(LCD_ENTRY_MODE | LCD_ENTRY_INC);
    hal_write_command(LCD_ON_CTRL | LCD_ON_DISPLAY);
}

/*
 * Sends one data byte to the LCD.
 * The PCF8574 backpack uses a byte format where the high nibble carries the LCD
 * data bits and the low bits control RS, E and backlight.
 */
void hal_write_data(uint8_t data) {
    /* Send high nibble with RS=1 (data mode) */
    uint8_t byte = (MASK_RS | (1U << SHIFT_BACKLIGHT) | (((data >> 4) & 0x0f) << SHIFT_DATA));
    I2C_WriteByte (LCD_address, byte | MASK_E);
    I2C_WriteByte (LCD_address, byte);

    /* Send low nibble with RS=1 */
    byte = (MASK_RS | (1U << SHIFT_BACKLIGHT) | (((data) & 0x0f) << SHIFT_DATA));
    I2C_WriteByte (LCD_address, byte | MASK_E);
    I2C_WriteByte (LCD_address, byte);
}

/*
 * Sends one command byte to the LCD.
 * RS is low for commands, and the rest of the bits follow the same nibble format.
*/
void hal_write_command(uint8_t data) {
    /* Send high nibble with RS=0 (command mode) */
    uint8_t byte = ((1U << SHIFT_BACKLIGHT) | (((data >> 4) & 0x0f) << SHIFT_DATA));
    I2C_WriteByte (LCD_address, byte | MASK_E);
    I2C_WriteByte (LCD_address, byte);

    /* Send low nibble with RS=0 */
    byte = ((1U << SHIFT_BACKLIGHT) | (((data) & 0x0f) << SHIFT_DATA));
    I2C_WriteByte (LCD_address, byte | MASK_E);
    I2C_WriteByte (LCD_address, byte);
}

/*
 * Sends only the upper nibble initialization command required by the LCD startup.
 * This is used before the full 8-bit initialization sequence is completed.
 */
void hal_write_nibble(uint8_t data) {
    uint8_t byte = (((data >> 4) & 0x0f) << SHIFT_DATA);
    I2C_WriteByte (LCD_address, byte | MASK_E);
    I2C_WriteByte (LCD_address, byte);
}

/*
 * Turns the backpack backlight on by writing the backlight bit.
 */
void hal_backlight_on(void) {
    I2C_WriteByte (LCD_address, (1U << SHIFT_BACKLIGHT));
}

/*
 * Turns the backpack backlight off.
 */
void hal_backlight_off(void) {
    I2C_WriteByte (LCD_address, 0U);
}
