
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

void pinInputByPolling(void *p)
{
    // printf("Starting task");
    bool p20;
    gpio_init(20);
    gpio_set_dir(20, 0);
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, 1);

    while (true)
    {
        while (p20)
        {
            p20 = gpio_get(20);
            gpio_put(LED_PIN, !p20);
        }
        printf("p20 went low\n");

        while (!p20)
        {
            p20 = gpio_get(20);
            gpio_put(LED_PIN, !p20);
        }
        printf("p20 went high\n");
    }
    while (1)
        ;
}
