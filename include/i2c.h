#ifndef I2C_H
#define I2C_H

#include <stdint.h>

/*
 * Public I2C status codes used by the LCD driver layer.
 * I2C_OK means the byte transfer completed successfully.
 * I2C_ERROR means the transfer timed out or failed to acknowledge.
 */
typedef enum {
    I2C_OK,
    I2C_ERROR
} I2C_Status_t;

/*
 * Writes one byte to the target I2C device address.
 * Used internally by the LCD driver to send command/data bytes.
 */
I2C_Status_t I2C_WriteByte(uint8_t address, uint8_t data);

/*
 * Low-level LCD transfer helpers.
 * These functions package LCD commands/data into the PCF8574-style byte format.
 */
void hal_write_data(uint8_t data);
void hal_write_command(uint8_t data);
void hal_write_nibble(uint8_t data);

/*
 * Initializes the LCD module over I2C.
 * address: I2C address of the backpack (for example 0x27 or 0x23)
 * num_of_lines: number of display rows
 * num_of_columns: number of columns per row
 */
void I2C_LCD_Init (uint8_t address, uint8_t num_of_lines, uint8_t num_of_columns);

/*
 * Controls the LCD backlight on the I2C backpack.
 */
void hal_backlight_on(void);
void hal_backlight_off(void);

/*
 * Bit mapping for the PCF8574-style backpack register:
 * bit 0 = RS
 * bit 1 = RW (not used for write mode)
 * bit 2 = E
 * bit 3 = backlight enable
 * bits 4..7 = LCD data nibble
 */
#define MASK_RS				0x01
#define MASK_RW				0x02
#define MASK_E				0x04
#define SHIFT_BACKLIGHT		3
#define SHIFT_DATA			4

#endif
