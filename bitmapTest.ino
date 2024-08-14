#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#include "frames.h"

#define TFT_CS        5
#define TFT_RST       17
#define TFT_DC        16

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);



const int buttonPin = 32; 
int buttonState = 0;
int page = 2;


void setup() {
  Serial.begin(115200);

  pinMode(buttonPin, INPUT_PULLUP);
  // Initialize the display
  tft.init(240, 280);  // Initialize with the display size
  tft.setSPISpeed(80000000);
  delay(500);
  tft.setRotation(2);            // Set the rotation if needed
  tft.fillScreen(ST77XX_BLACK);  // Fülle den Bildschirm mit Samuel
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(0, 0);
  pageTwo();

}


void loop() {

  buttonState = digitalRead(buttonPin);

  if (buttonState == LOW) {
    delay(200);
    if (buttonState == LOW) {
      if (page >= 3) {
        page = 1;
      } else {
        page += 1;
      }
      showPage(page);
    }
  }

}


void pageOne() {
  tft.fillScreen(ST77XX_BLACK); // Fülle den Bildschirm mit Samuel
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);
  tft.setCursor(20, 20);
  tft.print("seite 1");
  drawGrayscaleBitmap(75, 93, car, 89, 95);
}

void pageTwo() {
  tft.setCursor(0, 0);
  drawGrayscaleBitmap(0, 0, stern, 240, 280);  
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);
  tft.setCursor(20, 20);
  tft.print("seite 2");
}

void pageThree() {
  tft.fillScreen(ST77XX_BLACK); // Fülle den Bildschirm mit Samuel
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);
  tft.setCursor(20, 20);
  tft.print("seite 3");
  tft.setCursor(50, 20);
  tft.print("abc");
}

void showPage(int page) {
  switch (page) {
    case 1:
      pageOne();
      break;
    case 2:
      pageTwo();
      break;
    case 3:
      pageThree();
      break;
  }
}


void drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h) {
  for (int16_t j = 0; j < h; j++) {
    for (int16_t i = 0; i < w; i++) {
      uint8_t gray = bitmap[j * w + i];
      uint16_t color = grayToRGB565(gray);
      tft.drawPixel(x + i, y + j, color);
    }
  }
}

// Function to convert 8-bit grayscale to 16-bit RGB565
uint16_t grayToRGB565(uint8_t gray) {
  uint8_t r = gray;
  uint8_t g = gray;
  uint8_t b = gray;
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}


