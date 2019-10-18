/**
 * @file led.h
 *
 * @brief Control Nucleo and Shield LEDs
 */

#ifndef __LED_H__
#define __LED_H__

/*****************************************
 * Public Types
 *****************************************/

/**
 * @brief Possible actions for the gpio.
 */
typedef enum gpio_action {
    LED_SET,
    LED_RESET,
    LED_TOGGLE,
} gpio_action_t;

typedef enum led_color {
    LED_SHIELD = 0,
} led_color_t;

/*****************************************
 * Public Functions Prototypes
 *****************************************/

void led_init();

void led_control(led_color_t color, gpio_action_t action);

#endif // __LED_H__
