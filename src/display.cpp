#include "display.h"

#include <SPI.h>
#include <ST7789Spi.h>

namespace {
static SPIClass tftSpi(NRF_SPIM1, ST7789_MISO, ST7789_SCK, ST7789_SDA);

// ST7789Spi(spi, rst, dc, cs, geometry, width, height)
static ST7789Spi tft(&tftSpi, ST7789_RESET, ST7789_RS, ST7789_NSS,
                     GEOMETRY_RAWMODE, TFT_WIDTH, TFT_HEIGHT);

// --- Консоль: строки накапливаются, при переполнении уходят вверх ---
// Высота строки шрифта ArialMT_Plain_10 = 13 px (OLEDDisplayFonts.cpp).
constexpr uint16_t CONSOLE_LINE_H = 13;
constexpr uint8_t  CONSOLE_MAX_LINES = TFT_HEIGHT / CONSOLE_LINE_H; // 135/13 = 10
constexpr uint16_t CONSOLE_X = 4;   // левый отступ
constexpr uint16_t CONSOLE_Y0 = 2;  // верхний отступ
String consoleLines[CONSOLE_MAX_LINES];
uint8_t consoleCount = 0;

void redrawConsole()
{
    tft.clear();
    tft.setFont(ArialMT_Plain_10);
    tft.setRGB(0xFFFF); // ВАЖНО: иначе текст по умолчанию синий -> невидим
    tft.setTextAlignment(TEXT_ALIGN_LEFT);
    for (uint8_t i = 0; i < consoleCount; i++)
        tft.drawString(CONSOLE_X, CONSOLE_Y0 + i * CONSOLE_LINE_H, consoleLines[i]);
    tft.display();
}
} // namespace

Display display;

void Display::setBacklight(bool on)
{
    digitalWrite(VTFT_LEDA, on ? TFT_BACKLIGHT_ON : (TFT_BACKLIGHT_ON ? LOW : HIGH));
}

bool Display::init()
{
    pinMode(VTFT_CTRL, OUTPUT);
    digitalWrite(VTFT_CTRL, LOW); // panel power rail on
    pinMode(VTFT_LEDA, OUTPUT);
    delay(200);

    setBacklight(true);

    if (!tft.init())
        return false;

    // Ориентация: на этой панели картинка по умолчанию перевёрнута на 180°.
    tft.flipScreenVertically(); // MADCTL = MV|MX

    return true;
}

void Display::clear()
{
    tft.clear();
    tft.display();
}

void Display::showCenteredText(const char *text)
{
    clear();
    delay(300); // дать улететь кадру с чёрным фоном

    tft.setFont(ArialMT_Plain_10);
    tft.setRGB(0xFFFF); // ВАЖНО: текст белый (по умолчанию был синий -> невидим)
    tft.setTextAlignment(TEXT_ALIGN_CENTER);
    tft.drawString(TFT_WIDTH / 2, TFT_HEIGHT / 2, String(text));
    tft.display();
}

void Display::consoleClear()
{
    for (uint8_t i = 0; i < CONSOLE_MAX_LINES; i++)
        consoleLines[i] = "";
    consoleCount = 0;
    tft.clear();
    tft.display();
}

void Display::consolePrintln(const String &text)
{
    if (consoleCount < CONSOLE_MAX_LINES) {
        consoleLines[consoleCount++] = text;
    } else {
        // Скроллинг: сдвигаем строки вверх, освобождаем последнюю.
        for (uint8_t i = 0; i + 1 < CONSOLE_MAX_LINES; i++)
            consoleLines[i] = consoleLines[i + 1];
        consoleLines[CONSOLE_MAX_LINES - 1] = text;
    }
    redrawConsole();
}
