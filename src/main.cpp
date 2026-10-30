#include <Arduino.h>
#include <SPI.h>
#include <ST7789Spi.h> // meshtastic/st7789 driver (OLEDDisplay-based)

// 1.14" ST7789 TFT, 240x135, wired to SPIM1. Pins from variant.h:
//   CS=P0.11, DC=P0.12, MOSI=P1.09, SCK=P1.08, RST=P0.02
static SPIClass tftSpi(NRF_SPIM1, ST7789_MISO, ST7789_SCK, ST7789_SDA);

// ST7789Spi(spi, rst, dc, cs, geometry, width, height)
ST7789Spi tft(&tftSpi, ST7789_RESET, ST7789_RS, ST7789_NSS,
              GEOMETRY_RAWMODE, TFT_WIDTH, TFT_HEIGHT);

static void backlight(bool on)
{
    digitalWrite(VTFT_LEDA, on ? TFT_BACKLIGHT_ON : (TFT_BACKLIGHT_ON ? LOW : HIGH));
}

void setup()
{
    Serial.begin(115200);
    pinMode(PIN_LED1, OUTPUT);
    digitalWrite(PIN_LED1, HIGH); // off

    pinMode(VTFT_CTRL, OUTPUT);
    digitalWrite(VTFT_CTRL, LOW); // panel power rail on
    pinMode(VTFT_LEDA, OUTPUT);
    delay(200);

    backlight(true);

    Serial.println("Initializing TFT (ST7789)...");
    if (!tft.init()) {
        Serial.println("TFT init() failed");
        return;
    }
    Serial.println("TFT init OK");

    // Ориентация: на этой панели картинка по умолчанию перевёрнута на 180°.
    tft.flipScreenVertically(); // MADCTL = MV|MX

    // Чёрный фон + белый текст.
    tft.clear();
    tft.display();
    delay(300);

    tft.setFont(ArialMT_Plain_10);
    tft.setRGB(0xFFFF); // ВАЖНО: текст белый (по умолчанию был синий -> невидим)
    tft.setTextAlignment(TEXT_ALIGN_CENTER);
    tft.drawString(TFT_WIDTH / 2, TFT_HEIGHT / 2, String("Hello World!"));
    tft.display();
    Serial.println("'Hello World!' shown on TFT");
}

void loop()
{
    // Неблокирующее мигание зелёного светодиода (LOW = on).
    static uint32_t next = 0;
    static bool on = false;
    if (millis() >= next) {
        on = !on;
        digitalWrite(PIN_LED1, on ? LOW : HIGH);
        next = millis() + (on ? 100 : 900);
    }
}
