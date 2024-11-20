#include <Arduino.h>
#include <ESP_IOExpander_Library.h>
#include "expander.h"
#include "Wire.h"

ESP_IOExpander *expander;

// Extender Pin define
#define TP_RST 1
#define LCD_BL 2
#define LCD_RST 3
#define SD_CS 4
#define USB_SEL 5

// I2C Pin define
#define I2C_MASTER_NUM 0
#define I2C_MASTER_SDA_IO 8
#define I2C_MASTER_SCL_IO 9

void ExpanderInit() {
  Serial.println("IO expander init...");
  pinMode(GPIO_NUM_4, OUTPUT);
  
  Wire.begin();
  delay(50);

  expander = new ESP_IOExpander_CH422G(I2C_MASTER_NUM, ESP_IO_EXPANDER_I2C_CH422G_ADDRESS_000);
  expander->init();
  expander->begin();
  expander->multiPinMode(TP_RST | LCD_BL | LCD_RST | SD_CS | USB_SEL, OUTPUT);
  BLblink();
  //gt911 initialization, must be added, otherwise the touch screen will not be recognized  
  //initialization begin
  expander->multiDigitalWrite(TP_RST | LCD_RST, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_4, LOW);
  delay(100);
  expander->multiDigitalWrite(TP_RST | LCD_RST, HIGH);
  delay(200);
  //initialization end
}

void BLset(byte state){
    expander->digitalWrite(LCD_BL, state);
}

void BLblink(void){
  //Turn off backlight
  BLset(LOW);
  delay(100);
  BLset(HIGH);
  delay(100);
}