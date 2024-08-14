#include <TFT_eSPI.h> // Include the TFT_eSPI library

#include "frames.h"

TFT_eSPI tft = TFT_eSPI();  // Create a TFT_eSPI object

const int buttonPin = 32; 
int buttonState = 0;
int page = 2;

void setup() {

  pinMode(buttonPin, INPUT_PULLUP);
  
  tft.init();
  tft.setRotation(0); // Adjust rotation if needed
  tft.fillScreen(TFT_BLACK); // Clear the screen with black color
  
  tft.setTextColor(TFT_WHITE); // Set text color to white
  tft.setTextSize(2);          // Set text size
  tft.drawString("Hello, World!", 50, 100); // Print "Hello, World!" at (50, 100)

  drawGrayscaleBitmap(0, 0, stern, 240, 280);  
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
  tft.fillScreen(TFT_BLACK); // Fülle den Bildschirm mit Samuel
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(3);
  tft.drawString("Hello, World!", 20, 20); 
  drawGrayscaleBitmap(75, 93, car, 89, 95);
}

void pageTwo() {
  drawGrayscaleBitmap(0, 0, stern, 240, 280);  
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(3);
  tft.drawString("Hello, World!", 20, 20); 
}

void pageThree() {
  tft.fillScreen(TFT_BLACK); // Fülle den Bildschirm mit Samuel
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(3);
  tft.drawString("Seite 3", 20, 20); 
  tft.setTextSize(2);
  tft.drawString("AbC", 20, 50); 
  tft.setTextSize(1);
  tft.drawString("size 1", 20, 80); 
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
