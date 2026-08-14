#include "lcd.h"
#include "i2c.h"

/*
 * LCD state structure.
 * This keeps track of the LCD size and current cursor position.
 */
typedef struct {
    uint8_t num_of_lines;
    uint8_t num_of_columns;
    uint8_t cursor_x;
    uint8_t cursor_y;
    bool implied_newline;
    bool backlight;
} LCD_t;

/*
 * The library keeps one global LCD state object.
 * This is used by all display control functions.
 */
LCD_t lcd;

/*
 * Initializes the LCD driver state.
 * Called once when the display is connected and configured.
 */
void LCD_Init (uint8_t num_of_lines, uint8_t num_of_columns) {
    lcd.num_of_lines = num_of_lines;
    lcd.num_of_columns = num_of_columns;
    lcd.cursor_x = 0;
    lcd.cursor_y = 0;
    lcd.backlight = true;
    lcd.implied_newline = false;
}

/*
 * Clears the display and moves the cursor back to the top-left position.
 */
void clear (void) {
    hal_write_command(LCD_CLR);
    hal_write_command(LCD_HOME);
    lcd.cursor_x = 0;
    lcd.cursor_y = 0;
}

/*
 * Enables the display cursor.
 */
void show_cursor (void) {
    hal_write_command(LCD_ON_CTRL | LCD_ON_DISPLAY | LCD_ON_CURSOR);
}

/*
 * Hides the cursor.
 */
void hide_cursor (void) {
    hal_write_command(LCD_ON_CTRL | LCD_ON_DISPLAY);
}

/*
 * Turns on the blinking cursor.
 */
void blink_cursor_on (void) {
    hal_write_command(LCD_ON_CTRL | LCD_ON_DISPLAY | LCD_ON_CURSOR | LCD_ON_BLINK);
}

/*
 * Turns off the blinking cursor.
 */
void blink_cursor_off (void) {
    hal_write_command(LCD_ON_CTRL | LCD_ON_DISPLAY | LCD_ON_CURSOR);
}

/*
 * Turns the LCD display on.
 */
void display_on (void) {
    hal_write_command(LCD_ON_CTRL | LCD_ON_DISPLAY);
}

/*
 * Turns the LCD display off.
 */
void display_off (void) {
    hal_write_command(LCD_ON_CTRL);
}

/*
 * Turns the backlight on.
 */
void backlight_on (void) {
    lcd.backlight = true;
    hal_backlight_on();
}

/*
 * Turns the backlight off.
 */
void backlight_off (void) {
    lcd.backlight = false;
    hal_backlight_off();
}

/*
 * Moves the cursor to the selected row and column.
 * This function calculates the actual DDRAM address for the HD44780 controller.
 */
void move_to (uint8_t cursor_x, uint8_t cursor_y) {
    lcd.cursor_x = cursor_x;
    lcd.cursor_y = cursor_y;

    uint8_t addr = cursor_x & 0x3f;
    if (cursor_y & 1) addr += 0x40;
    if (cursor_y & 2) addr += lcd.num_of_columns;
    hal_write_command(LCD_DDRAM | addr);
}

/*
 * Writes one character to the LCD at the current cursor location and handles line wrapping and newline.
 */
void LCD_putchar (char c) {
    if (c == '\n') {
        /* If the driver is not already in an implied newline state, move to the end of the row. */
        if (!lcd.implied_newline) lcd.cursor_x = lcd.num_of_columns;
    } else {
        /* Write the character to the display */
        hal_write_data((unsigned char)c);
        lcd.cursor_x += 1;
    }

    /* If cursor passed the end of the current row, wrap to next line */
    if (lcd.cursor_x >= lcd.num_of_columns) {
        lcd.cursor_x = 0;
        lcd.cursor_y += 1;
        lcd.implied_newline = (c != '\n');
    }

    /* Wrap around to the first line if the row exceeds the configured number of rows */
    if (lcd.cursor_y >= lcd.num_of_lines) lcd.cursor_y = 0;

    /* Move the hardware cursor to match cursor state */
    move_to(lcd.cursor_x, lcd.cursor_y);
}

/*
 * Writes a null-terminated string to the LCD.
 */
void LCD_putstr(const char *s)
{
    while (*s != '\0')
    {
        char c = *s;
        LCD_putchar(c);
        s++;
    }
}

/*
 * Reverses the character buffer used to build an integer string.
 */
static void reverse_buffer(char *buf, int len)
{
    int start = 0;
    int end = len - 1;

    while (start < end) {
        char tmp = buf[start];
        buf[start] = buf[end];
        buf[end] = tmp;
        start++;
        end--;
    }
}

/*
 * Converts an integer to text and prints it to the LCD.
 */
void LCD_putint(int value)
{
    char buf[16];
    int index = 0;
    int is_negative = 0;

    if (value == 0) {
        LCD_putchar('0');
        return;
    }

    if (value < 0) {
        is_negative = 1;
        value = -value;
    }

    /* Build the digits in reverse */
    while (value > 0) {
        buf[index++] = (char)('0' + (value % 10));
        value /= 10;
    }

    /* Add the sign if negative */
    if (is_negative) {
        buf[index++] = '-';
    }

    /* Reverse the digits back to normal order */
    reverse_buffer(buf, index);

    /* Print each character to the LCD */
    for (int i = 0; i < index; ++i) {
        LCD_putchar(buf[i]);
    }
}
