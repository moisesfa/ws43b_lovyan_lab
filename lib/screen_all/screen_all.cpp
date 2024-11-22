#include "screen_all.h"
#include <LovyanGFX.hpp>
#include "config_lovyan_gfx.h"

extern LGFX tft;
static LGFX_Sprite sprite_sel_screen(&tft);

uint8_t screen_number = 1;

void draw_sprite_sel_screen(uint8_t screen)
{
    sprite_sel_screen.createSprite(WIDTH_SPRT_SEL_SCREEN, HEIGHT_SPRT_SEL_SCREEN);

    sprite_sel_screen.fillSprite(0x003030);
    //sprite_sel_screen.fillSprite(TFT_TRANSPARENT);
    //sprite_sel_screen.fillRoundRect(10,10,WIDTH_SPRT_SEL_SCREEN-20, HEIGHT_SPRT_SEL_SCREEN-20, 20,0x003030);
    
    switch (screen)
    {
    case 1:
        sprite_sel_screen.fillCircle(WIDTH_SPRT_SEL_SCREEN/2-20,HEIGHT_SPRT_SEL_SCREEN/2,8,TFT_LIGHTGRAY);
        sprite_sel_screen.fillCircle(WIDTH_SPRT_SEL_SCREEN/2+20,HEIGHT_SPRT_SEL_SCREEN/2,8,TFT_DARKGRAY);
        break;
    case 2:
        sprite_sel_screen.fillCircle(WIDTH_SPRT_SEL_SCREEN/2-20,HEIGHT_SPRT_SEL_SCREEN/2,8,TFT_DARKGRAY);
        sprite_sel_screen.fillCircle(WIDTH_SPRT_SEL_SCREEN/2+20,HEIGHT_SPRT_SEL_SCREEN/2,8,TFT_LIGHTGRAY);
        break;
    
    default:
        break;
    }
 
    sprite_sel_screen.pushSprite(350,420);
}