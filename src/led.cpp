#include "led.h"

namespace {
bool on = false;
uint32_t next = 0;
} // namespace

void Led::init()
{
    pinMode(PIN_LED1, OUTPUT);
    set(false);
    next = 0;
}

void Led::set(bool state)
{
    on = state;
    digitalWrite(PIN_LED1, state ? LOW : HIGH);
}

void Led::blink(uint16_t onMs, uint16_t offMs)
{
    if (millis() < next)
        return;
    set(!on);
    next = millis() + (on ? onMs : offMs);
}
