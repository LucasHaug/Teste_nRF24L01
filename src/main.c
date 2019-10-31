/**
 * @file main.c
 *
 * @brief Main function
 */

#include "mcu.h"
#include "RF24.h"
#include "spi.h"
#include "led.h"

/*****************************************
 * Private Constant Definitions
 *****************************************/

#define IS_RECEIVER 0

#define PAYLOAD_SIZE 15

/*****************************************
 * Private Variables
 *****************************************/

uint8_t addresses[2][5] = {{0xE7, 0xE7, 0xE7, 0xE7, 0xE8}, {0xC2, 0xC2, 0xC2, 0xC2, 0xC1}};

uint8_t buffer[] = {'V', 'i', 'r', 't', 'u', 'a', 'l', ' ', 'h', 'u', 'g', 's', 0, '\r', '\n'};

// typedef struct data {

// } my_data;

/*****************************************
 * Main Function
 *****************************************/

int main(void) {
    mcu_init();
    led_init();
    MX_SPI1_Init();

    led_control(LED_SHIELD, LED_SET);
    HAL_Delay(1000);

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

    p_dev->payload_size = PAYLOAD_SIZE,

    HAL_Delay(10);

    printf("================ NRF24 TEST ================\r\n");
    printf("Calling init [....]");

    if (rf24_init(p_dev)) {
        // Deixou praticamente no deafult, só mudou em CONFIG CRCO
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

    rf24_dump_registers(p_dev);

    // A partir daq vou configurar como ta no GettingStarted_HandlingData.ino

    rf24_set_output_power(p_dev, RF24_12_dBm);

#if (IS_RECEIVER == 1)
    rf24_open_writing_pipe(p_dev, addresses[0]);
    rf24_open_reading_pipe(p_dev, 1, addresses[1]);
#else
    rf24_open_writing_pipe(p_dev, addresses[1]);
    rf24_open_reading_pipe(p_dev, 1, addresses[0]);
#endif

    rf24_dump_registers(p_dev);

#if (IS_RECEIVER == 1)
    rf24_start_listening(p_dev);
#endif

    for (;;) {
#if (IS_RECEIVER == 1)

        if (rf24_available(p_dev, NULL)) {
            while (rf24_available(p_dev, NULL)) {
                rf24_read(p_dev, buffer, p_dev->payload_size);
            }

            printf("Recebendo: ");

            for (int i = 0; i < p_dev->payload_size; i++) {
                if (i == 12) {
                    printf(" no. %d", (int) buffer[i]);
                } else {
                    printf("%c", (char) buffer[i]);
                }
            }

            printf("\r\n");
        }

#else
        printf("Sending Virtual Hug %d!\r\n", buffer[12]);

        HAL_Delay(500);

        printf("\r\n");

        if (rf24_write(p_dev, buffer, PAYLOAD_SIZE, true)) {
            printf("Virtual Hug Sent!\r\n");
        }

        printf("\r\n");
        (buffer[12])++;
#endif

        rf24_print_status(p_dev);
        printf("\r\n");
        HAL_Delay(500);
    }
}
