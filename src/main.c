/**
 * @file main.c
 *
 * @brief Main function
 */

#include "mcu.h"
#include "rf24.h"
#include "rf24_debug.h"
#include "spi.h"
#include "led.h"
#include "tim.h"

/*****************************************
 * Private Constant Definitions
 *****************************************/

#define IS_RECEIVER 0

#define ENABLE_AA 0

#define BASE_PAYLOAD_SIZE 15

/*****************************************
 * Private Variables
 *****************************************/

uint8_t addresses[2][5] = {{0xE7, 0xE7, 0xE7, 0xE7, 0xE8}, {0xC2, 0xC2, 0xC2, 0xC2, 0xC1}};

#if (ENABLE_AA == 1)
uint8_t buffer[] = {'V', 'i', 'r', 't', 'u', 'a', 'l', ' ', 'h', 'u', 'g', 's', 0, '\r', '\n'};
#else
uint8_t buffer[] = {
    'V', 'i', 'r', 't', 'u', 'a', 'l', ' ', 'h', 'u', 'g', 's', 0, '\r', '\n',
    'V', 'i', 'r', 't', 'u', 'a', 'l', ' ', 'h', 'u', 'g', 's', 0, '\r', '\n'
};
#endif

/*****************************************
 * Main Function
 *****************************************/

int main(void) {
    /* Init */

    mcu_init();
    led_init();
    MX_SPI1_Init();
    MX_TIM2_Init();
    HAL_TIM_Base_Start(&htim2);

    led_control(LED_SHIELD, LED_SET);
    HAL_Delay(1000);

    /* Device config */

    rf24_dev_t* p_dev;
    rf24_dev_t device = rf24_get_default_config();
    p_dev = &device;

    p_dev->platform_setup.hspi = &hspi1,
    p_dev->platform_setup.csn_port = GPIOC,
    p_dev->platform_setup.csn_pin = GPIO_PIN_7,

    p_dev->platform_setup.ce_port = GPIOA,
    p_dev->platform_setup.ce_pin = GPIO_PIN_9,

    p_dev->platform_setup.irq_port = GPIOB,
    p_dev->platform_setup.irq_pin = GPIO_PIN_6,

    p_dev->payload_size = (BASE_PAYLOAD_SIZE * (2 - ENABLE_AA)),

    HAL_Delay(10);

    rf24_status_t my_dev_status = RF24_SUCCESS;

    /* Test Init */

    printf("================ NRF24 TEST ================\r\n");
    printf("Calling init [....]");

    if (rf24_init(p_dev) == RF24_SUCCESS) {
        /* Deixou praticamente no deafult, só mudou em CONFIG CRCO */
        printf("\b\b\b\b\b\b[ OK ]\r\n");

        led_control(LED_SHIELD, LED_RESET);
        HAL_Delay(300);
        led_control(LED_SHIELD, LED_SET);
    } else {
        printf("\b\b\b\b\b\b[FAIL]\r\n");

        led_control(LED_SHIELD, RESET);

        for (;;)
            ;
    }

    /* A partir daq vou configurar como ta no GettingStarted_HandlingData.ino */

    my_dev_status = rf24_set_output_power(p_dev, RF24_18_dBm);

#if (IS_RECEIVER == 1)
    my_dev_status = rf24_open_writing_pipe(p_dev, addresses[0]);
    my_dev_status = rf24_open_reading_pipe(p_dev, 1, addresses[1]);
#else
    my_dev_status = rf24_open_writing_pipe(p_dev, addresses[1]);
    my_dev_status = rf24_open_reading_pipe(p_dev, 1, addresses[0]);
#endif

    printf("\n DEV STATUS = %d\r\n\n", my_dev_status);

    rf24_debug_dump_registers(p_dev);

#if (IS_RECEIVER == 1)
    my_dev_status = rf24_start_listening(p_dev);
#endif

    /* Time test variables */

    uint32_t transmission_delay = 0;
    uint32_t receiver_delay = 0;
    uint32_t current_time = 0;

    /* Main loop */

    for (;;) {
#if (IS_RECEIVER == 1)
        printf("Receiving delay: %ld\r\n", receiver_delay);
        current_time = __HAL_TIM_GET_COUNTER(&htim2);

        if ((my_dev_status = rf24_available(p_dev, NULL)) == RF24_SUCCESS) {
            while ((my_dev_status = rf24_available(p_dev, NULL)) == RF24_SUCCESS) {
                my_dev_status = rf24_read(p_dev, buffer, p_dev->payload_size);
            }

            receiver_delay = __HAL_TIM_GET_COUNTER(&htim2) - current_time;

            printf("Recebendo: ");

            for (int i = 0; i < p_dev->payload_size; i++) {
                if (i == 12) {
                    printf(" no. %d", (int) buffer[i]);
                } else {
                    printf("%c", (char) buffer[i]);
                }
            }

            printf("\r\n");
        } else {
            receiver_delay = 0xFFFF;
        }

#else
        printf("Sending Virtual Hug %d!\r\n", buffer[12]);

        HAL_Delay(500);

        printf("\r\n");

        printf("Transmission delay: %ld\r\n", transmission_delay);
        current_time = __HAL_TIM_GET_COUNTER(&htim2);

        if ((my_dev_status =
                 rf24_write(p_dev, buffer, (BASE_PAYLOAD_SIZE * (2 - ENABLE_AA)), ENABLE_AA)) == RF24_SUCCESS) {
            printf("Virtual Hug Sent!\r\n");
            transmission_delay = __HAL_TIM_GET_COUNTER(&htim2) - current_time;
        } else {
            transmission_delay = 0xFFFF;
        }

        printf("\r\n");
        (buffer[12])++;
#endif

        rf24_debug_print_status(p_dev);
        printf("\r\n");
        HAL_Delay(500);
    }
}
