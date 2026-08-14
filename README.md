# stm32-i2c-lcd-driver
Lightweight STM32 library for HD44780-compatible character LCDs using an I2C backpack (PCF8574-style expander).

This driver is intended for 16x2, 20x4, and other HD44780-compatible character displays connected through a backpack module over I2C. It provides basic text output, cursor control, clearing, and integer printing.

Check the example/main.c file for a working implementation.

## Repository layout

stm32-i2c-lcd-driver/
- README.md
- inc/
    - lcd.h
    - i2c.h
- src/
    - lcd.c
    - i2c.c
- examples/
    - main.c

# Features

- Cursor display and hiding
- Blink control
- Clear screen
- Cursor movement
- String output
- Integer output
- Backlight control
- Works with STM32 microcontrollers using direct register access

# Supported hardware

This library is designed for:
- HD44780-compatible character LCDs
- I2C backpack modules using PCF8574-style wiring
- STM32 MCUs with direct register access

Example of supported display types:
- 16x2 LCD
- 20x4 LCD

Notes:
- The library uses direct register access for STM32.
- This is a low-level driver and is intended for embedded use.

Check your exact backpack module because some modules use:
- 0x23
- 0x27
- 0x3F
- or another I2C address
Use a proper I2C scanner if necessary to detect the address.

Initialization:
Call:
I2C_LCD_Init(address, rows, columns);

API:
- void LCD_Init(uint8_t num_of_lines, uint8_t num_of_columns);
- void clear(void);
- void show_cursor(void);
- void hide_cursor(void);
- void blink_cursor_on(void);
- void blink_cursor_off(void);
- void display_on(void);
- void display_off(void);
- void backlight_on(void);
- void backlight_off(void);
- void move_to(uint8_t cursor_x, uint8_t cursor_y);
- void LCD_putchar(char c);
- void LCD_putstr(const char *s);
- void LCD_putint(int value);

Author:
Rakesh Kumar Ramalingam / rakeshkumarr4
