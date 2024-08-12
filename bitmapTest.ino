#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#include "frames.h"

#define TFT_CS        5
#define TFT_RST       17
#define TFT_DC        16

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);



void setup() {
  Serial.begin(115200);
  // Initialize the display
  tft.init(240, 280); // Initialize with the display size
  tft.setSPISpeed(80000000);
  delay(500);
  tft.setRotation(2); // Set the rotation if needed
  tft.fillScreen(ST77XX_BLACK); // Fülle den Bildschirm mit Samuel
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(0, 0);
}

void loop() {

  tft.drawRGBBitmap(0, 0, frame6, 240, 280);
  tft.drawRGBBitmap(0, 0, frame7, 240, 280);
  tft.drawRGBBitmap(0, 0, frame8, 240, 280);

}