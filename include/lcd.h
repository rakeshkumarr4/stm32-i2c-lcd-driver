#ifndef LCD_H
#define LCD_H

#include <stdint.h>
#include <stdbool.h>

/*
 * Public LCD API.
 * These functions are meant to be called by the application code.
 * They abstract the lower-level I2C bus and the HD44780 command protocol.
 */

/*
 * Initializes the LCD state structure.
 * num_of_lines: number of display rows, for example 2 for a 16x2 LCD
 * num_of_columns: number of columns per row, for example 16
 */
void LCD_Init (uint8_t num_of_lines, uint8_t num_of_columns);

/*
 * Clears the display and moves the cursor back to home position.
 */
void clear (void);

/*
 * Shows the cursor on the display.
 */
void show_cursor (void);

/*
 * Hides the cursor from the display.
 */
void hide_cursor (void);

/*
 * Enables the blinking cursor mode.
 */
void blink_cursor_on (void);

/*
 * Disables the blinking cursor mode.
 */
void blink_cursor_off (void);

/*
 * Turns the display on.
 */
void display_on (void);

/*
 * Turns the display off.
 */
void display_off (void);

/*
 * Turns the LCD backlight on.
 */
void backlight_on (void);

/*
 * Turns the LCD backlight off.
 */
void backlight_off (void);

/*
 * Moves the cursor to the given row and column.
 * cursor_x: column index, starting from 0
 * cursor_y: row index, starting from 0
 */
void move_to (uint8_t cursor_x, uint8_t cursor_y);

/*
 * Writes one character to the current cursor position. Wraps to next row automatically.
 * Handles the '\n' newline character by moving to the next row.
 */
void LCD_putchar (char c);

/*
 * Writes a null-terminated string to the display using LCD_putchar.
 */
void LCD_putstr (const char* s);

/*
 * Converts an integer to text and prints it on the LCD.
 */
void LCD_putint(int value);

/*
 * HD44780 instruction definitions.
 * These are the low-level display controller commands.
 */
#define LCD_CLR 0x01              // DB0: clear display
#define LCD_HOME 0x02             // DB1: return to home position

#define LCD_ENTRY_MODE 0x04       // DB2: set entry mode
#define LCD_ENTRY_INC 0x02        // --DB1: increment
#define LCD_ENTRY_SHIFT 0x01      // --DB0: shift

#define LCD_ON_CTRL 0x08          // DB3: turn lcd/cursor on
#define LCD_ON_DISPLAY 0x04       // --DB2: turn display on
#define LCD_ON_CURSOR 0x02        // --DB1: turn cursor on
#define LCD_ON_BLINK 0x01         // --DB0: blinking cursor

#define LCD_MOVE 0x10             // DB4: move cursor/display
#define LCD_MOVE_DISP 0x08        // --DB3: move display (0-> move cursor)
#define LCD_MOVE_RIGHT 0x04       // --DB2: move right (0-> left)

#define LCD_FUNCTION 0x20         // DB5: function set
#define LCD_FUNCTION_8BIT 0x10    // --DB4: set 8BIT mode (0->4BIT mode)
#define LCD_FUNCTION_2LINES 0x08  // --DB3: two lines (0->one line)
#define LCD_FUNCTION_10DOTS 0x04  // --DB2: 5x10 font (0->5x7 font)
#define LCD_FUNCTION_RESET 0x30   // initialization command used at startup

#define LCD_CGRAM 0x40            // DB6: set CG RAM address
#define LCD_DDRAM 0x80            // DB7: set DD RAM address

#define LCD_RS_CMD 0
#define LCD_RS_DATA 1

#define LCD_RW_WRITE 0
#define LCD_RW_READ 1

#endif
