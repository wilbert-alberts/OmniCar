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

typedef struct
{
    uint8_t pinA;
    uint8_t pinB;

    uint8_t currentA;
    uint8_t currentB;

    int16_t position;

} ABDecoder;

static const uint LED_PIN = PICO_DEFAULT_LED_PIN;

static void irq_pinChanged(ABDecoder *ab);
static void irq_any(uint gpio, uint32_t events);
static void decodeAB(
    uint8_t newA,
    uint8_t newB,
    uint8_t *previousA,
    uint8_t *previousB,
    int16_t *position);
static void initIO(ABDecoder *ab);

ABDecoder abFrontLeft = {
    4,
    5,

    false,
    false,

    0};

ABDecoder abFrontRight = {
    16,
    17,

    false,
    false,

    0};

ABDecoder abRearLeft = {
    8,
    9,

    false,
    false,

    0};
ABDecoder abRearRight = {
    12,
    13,

    false,
    false,

    0};

void abEncoders(void *p)
{
    printf("AB encoders");

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, 1);

    initIO(&abFrontLeft);
    initIO(&abFrontRight);
    initIO(&abRearLeft);
    initIO(&abRearRight);

    while (1)
    {
        vTaskDelay(5000 / portTICK_PERIOD_MS);

        printf("Motor Front Left:  %d\n", abFrontLeft.position);
        printf("Motor Front Right: %d\n", abFrontRight.position);
        printf("Motor Rear  Left:  %d\n", abRearLeft.position);
        printf("Motor Rear  Right: %d\n", abRearRight.position);
    }
}

static void initIO(ABDecoder *ab)
{
    gpio_init(ab->pinA);
    gpio_init(ab->pinB);
    ab->currentA = gpio_get(ab->pinA);
    ab->currentB = gpio_get(ab->pinB);
    ab->position = 0;
    gpio_set_irq_enabled_with_callback(ab->pinA, (1 << 2) | (1 << 3), true, irq_any);
    gpio_set_irq_enabled_with_callback(ab->pinB, (1 << 2) | (1 << 3), true, irq_any);
}

static void irq_any(uint gpio, uint32_t events)
{
    irq_pinChanged(&abFrontLeft);
    irq_pinChanged(&abFrontRight);
    irq_pinChanged(&abRearLeft);
    irq_pinChanged(&abRearRight);
}

static void irq_pinChanged(ABDecoder *ab)
{
    uint8_t newA = gpio_get(ab->pinA);
    uint8_t newB = gpio_get(ab->pinB);

    decodeAB(newA, newB, &ab->currentA, &ab->currentB, &ab->position);
}

static void decodeAB(
    uint8_t newA,
    uint8_t newB,
    uint8_t *previousA,
    uint8_t *previousB,
    int16_t *position)
{
    // Capture state
    uint8_t state = (*previousA << 3) | (newA << 2) | (*previousB << 1) | (newB);

    /*
          aold       anew    bold    bnew
    0       0         0       0       0           0
    1       0         0       0       1           -1
    2       0         0       1       0           +1
    3       0         0       1       1           0
    4       0         1       0       0           +1
    5       0         1       0       1           err
    6       0         1       1       0           err
    7       0         1       1       1           -1
    8       1         0       0       0           -1
    9       1         0       0       1           err
    10      1         0       1       0           err
    11      1         0       1       1           +1
    12      1         1       0       0           0
    13      1         1       0       1           +1
    14      1         1       1       0           -1
    15      1         1       1       1           0

    */
    // Process new state
    switch (state)
    {
    case 0:
    case 3:
    case 12:
    case 15:
        // No change
        break;
    case 1:
    case 7:
    case 8:
    case 14:
        (*position)--;
        break;
    case 2:
    case 4:
    case 11:
    case 13:
        (*position)++;
    case 5:
    case 6:
    case 9:
    case 10:
        // Error transition
        break;
    }

    // Update AB values
    *previousA = newA;
    *previousB = newB;
}
