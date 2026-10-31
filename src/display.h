#pragma once

#include <Arduino.h>

// 1.14" ST7789 TFT, 240x135, wired to SPIM1.
// Пины берутся из variants/heltec_mesh_node_t114/variant.h:
//   CS=P0.11, DC=P0.12, MOSI=P1.09, SCK=P1.08, RST=P0.02
class Display
{
public:
    // Подать питание на панель (VTFT_CTRL), включить подсветку
    // и инициализировать ST7789 (включая поворот на 180°).
    bool init();

    // Подсветка (VTFT_LEDA): true = вкл.
    void setBacklight(bool on);

    // Очистить буфер (чёрный) и отправить кадр.
    void clear();

    // Текст белым по центру на чёрном фоне.
    void showCenteredText(const char *text);

    // Размеры экрана в пикселях.
    static uint16_t width()  { return TFT_WIDTH; }
    static uint16_t height() { return TFT_HEIGHT; }
};

// Глобальный экземпляр (одна панель на устройстве).
extern Display display;
