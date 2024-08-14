#include <TFT_eSPI.h> // Include the TFT_eSPI library

TFT_eSPI tft = TFT_eSPI();  // Create a TFT_eSPI object

void setup() {
  tft.init();
  tft.setRotation(0); // Adjust rotation if needed
  tft.fillScreen(TFT_BLACK); // Clear the screen with black color
  
  tft.setTextColor(TFT_WHITE); // Set text color to white
  tft.setTextSize(2);          // Set text size
  tft.drawString("Hello, World!", 50, 100); // Print "Hello, World!" at (50, 100)
}

void loop() {
  // Nothing to do here
}
