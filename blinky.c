
/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Library includes. */
#include <stdio.h>
#include "pico/stdlib.h"
#if (mainRUN_ON_CORE == 1)
#include "pico/multicore.h"
#endif


static const uint LED_PIN = PICO_DEFAULT_LED_PIN;

void blinky(void *p)
{
    printf("Starting blinky");
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, 1);

    while (true)
    {
        gpio_put(LED_PIN, 1);
        printf("Blink high\n");
        vTaskDelay(500 / portTICK_PERIOD_MS);
        gpio_put(LED_PIN, 0);
        printf("Blink low\n");
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}