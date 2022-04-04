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

static void irq_pinChanged(uint gpio, uint32_t events);
static const uint LED_PIN = PICO_DEFAULT_LED_PIN;

void pinInputByIRQ(void *p)
{
    printf("Starting nothing");
    gpio_init(20);
    gpio_set_dir(20, 0);
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, 1);

    // enable irq on both flanks
    gpio_set_irq_enabled_with_callback(20, 0x0f, true, irq_pinChanged);
    while (1)
        ;
}

void irq_pinChanged(uint gpio, uint32_t events)
{
    if (gpio == 20)
    {
        // printf("Pin 20 changed, events: %d \n", events);
        bool p20 = gpio_get(20);

        gpio_put(LED_PIN, !p20);
    }
}
