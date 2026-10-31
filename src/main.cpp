#include <Arduino.h>

#include "console.h"
#include "display.h"
#include "led.h"

void setup()
{
    Console::init();
    Led::init();

    Console::println("Initializing TFT (ST7789)...");
    if (!display.init()) {
        Console::println("TFT init() failed");
        return;
    }
    Console::println("TFT init OK");

    display.showCenteredText("Hello World!");
    Console::println("'Hello World!' shown on TFT");
}

void loop()
{
    Led::blink(); // зелёный LED: 100 мс вкл / 900 мс выкл
}
