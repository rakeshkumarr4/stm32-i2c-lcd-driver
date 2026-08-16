#include "stm32f4xx.h" /* Change this to the matching device header for your MCU,
                        e.g. stm32f407xx.h, stm32f103xx.h, etc. */
#include "i2c.h"
#include "lcd.h"

void SystemClock_Config(void)
{
    /* Minimal HSI config for 16 MHz clock */
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY)) {}
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_HSI;
}

void I2C_Init(void)
{
    /* Enable I2C1 and GPIOB clocks */
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    /* PB8 = SCL, PB9 = SDA */
    GPIOB->MODER &= ~(3U << (8 * 2));
    GPIOB->MODER |= (2U << (8 * 2));
    GPIOB->OTYPER |= (1U << 8);
    GPIOB->PUPDR &= ~(3U << (8 * 2));
    GPIOB->AFR[1] &= ~(0xF << ((8 - 8) * 4));
    GPIOB->AFR[1] |= (4U << ((8 - 8) * 4));

    GPIOB->MODER &= ~(3U << (9 * 2));
    GPIOB->MODER |= (2U << (9 * 2));
    GPIOB->OTYPER |= (1U << 9);
    GPIOB->PUPDR &= ~(3U << (9 * 2));
    GPIOB->AFR[1] &= ~(0xF << ((9 - 8) * 4));
    GPIOB->AFR[1] |= (4U << ((9 - 8) * 4));

    /* Disable I2C before configuring */
    I2C1->CR1 &= ~I2C_CR1_PE;

    /* Configure I2C clock: 16 MHz / (2 * 80) = 100kHz approx */
    I2C1->CR2 = 16;
    I2C1->CCR = 80;
    I2C1->TRISE = 17;

    /* Enable I2C */
    I2C1->CR1 |= I2C_CR1_PE;
}

int main(void)
{
    SystemClock_Config();
    I2C_Init();

    /* Address, rows, columns */
    I2C_LCD_Init(0x23, 2, 16);

    /* Turn cursor on */
    show_cursor();

    /* Print text */
    LCD_putstr("Hello");
    move_to(0, 1);
    LCD_putstr("World");

    while (1)
    {
        /* Application loop */
    }
}
