/**
 * @file led.c
 *
 * @brief Control Nucleo and Shield LEDs
 */

#include "led.h"
#include "gpio.h"

/*****************************************
 * Private Types
 *****************************************/

typedef struct led {
    GPIO_TypeDef* port;
    uint16_t      pin;
} led_t;

/*****************************************
 * Private Variables Declarations
 *****************************************/

static led_t m_led[] = {
    {
        .port = GPIOA, // LED_SHIELD
        .pin = GPIO_PIN_1,
    }
};

/*****************************************
 * Public Functions Bodies Definitions
 *****************************************/

void led_init(void) {
    MX_GPIO_Init();

    led_control(LED_SHIELD, LED_RESET);
}

void led_control(led_color_t color, gpio_action_t action) {
    switch (action) {
        case LED_TOGGLE: {
            HAL_GPIO_TogglePin(m_led[color].port, m_led[color].pin);
            break;
        }

        case LED_SET: {
            HAL_GPIO_WritePin(m_led[color].port, m_led[color].pin, GPIO_PIN_SET);
            break;
        }

        case LED_RESET: {
            HAL_GPIO_WritePin(m_led[color].port, m_led[color].pin, GPIO_PIN_RESET);
            break;
        }
    }
}
