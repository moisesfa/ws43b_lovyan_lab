#include <LovyanGFX.hpp>
#include "config_lovyan_gfx.h"

// Dirección para convrtir fuentes https://rop.nl/truetype2gfx/   
#include "myfonts_scr_01/AlibabaSans_Bold100pt7b.h"

const char* title = "Hello World";

extern LGFX tft;

void screen_01_view(void) 
{
    tft.fillScreen(0x003030);
    tft.fillRect(0, 0, 800, 70, 0x000028);
    tft.setTextColor(TFT_GREEN);
    tft.setTextSize(4);
    tft.setCursor(250, 20);
    tft.println(title);
    
    tft.setTextSize(1);
    tft.setFont(&AlibabaSans_Bold100pt7b);
    //tft.setFont(&fonts::DejaVu72);
    tft.setTextDatum(CC_DATUM);
    tft.drawString("01234",400,250);
    tft.setFont(&fonts::DejaVu18);
}