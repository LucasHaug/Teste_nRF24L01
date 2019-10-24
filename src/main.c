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

uint8_t buffer[] = {'V','i','r','t','u','a','l',' ','h','u','g','s', 0 , '\r', '\n'};

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

    rf24_t rf24 = rf24_get_default_config();
    rf24.hspi = &hspi1,
    rf24.csn_port = GPIOC,
    rf24.csn_pin = GPIO_PIN_7,

    rf24.ce_port = GPIOA,
    rf24.ce_pin = GPIO_PIN_9,

    rf24.irq_port = GPIOB,
    rf24.irq_pin = GPIO_PIN_6,

    rf24.payload_size = PAYLOAD_SIZE,

    HAL_Delay(10);

    printf("================ NRF24 TEST ================\r\n");
    printf("Calling init [....]");

    if (rf24_init(&rf24)) {
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

    rf24_dump_registers(&rf24);

    // A partir daq vou configurar como ta no GettingStarted_HandlingData.ino

    rf24_set_output_power(&rf24, RF24_12_dBm);

#if (IS_RECEIVER == 1)
    rf24_open_writing_pipe(&rf24, addresses[0]);
    rf24_open_reading_pipe(&rf24, 1, addresses[1]);
#else
    rf24_open_writing_pipe(&rf24, addresses[1]);
    // rf24_open_reading_pipe(&rf24, 0, addresses[0]);
#endif

    rf24_dump_registers(&rf24);

#if (IS_RECEIVER == 1)
    rf24_start_listening(&rf24);
#endif

    for (;;) {
#if (IS_RECEIVER == 1)
        if (rf24_available(&rf24, NULL)) {
            while (rf24_available(&rf24, NULL)) {
                rf24_read(&rf24, buffer, rf24.payload_size);
            }

            printf("Recebendo: ");

            for (int i = 0; i < rf24.payload_size; i++) {
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

        if (rf24_write(&rf24, buffer, PAYLOAD_SIZE, true)) {
            printf("Virtual Hug Sent!\r\n");
        }

        printf("\r\n");
        (buffer[12])++;
#endif

        rf24_print_status(&rf24);
        printf("\r\n");
        HAL_Delay(500);
    }
}
