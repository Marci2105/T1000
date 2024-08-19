#include <TFT_eSPI.h>
#include <SPI.h>
#include <lvgl.h>
#include <iostream>

#include "frames.h"

#define CrowPanel_24

static const uint16_t screenWidth = 240;
static const uint16_t screenHeight = 280;

const int buttonPin = 32;
int buttonState = 0;
int page = 2;

lv_obj_t *labelPageName = NULL;

lv_obj_t * objectArray[3] = {NULL, NULL, NULL};


TFT_eSPI tft = TFT_eSPI(); /* TFT entity */

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[screenWidth * screenHeight / 13];

//_______________________
/* display flash */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t *)&color_p->full, w * h, true);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}

//interrupt function von dem Button
void IRAM_ATTR handleInterrupt() {
    Serial.println("Interrupt occurred!");
} 

void setup() {
  Serial.begin(9600); /*serial init */

  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), handleInterrupt, RISING);

  //LCD init
  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  delay(100);

  //lvgl init
  lv_init();

  lv_disp_draw_buf_init(&draw_buf, buf1, NULL, screenWidth * screenHeight / 13);

  /* Initialize the display */
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /* Change the following line to your display resolution */
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /* Initialize the (dummy) input device driver */
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  lv_indev_drv_register(&indev_drv);

  tft.fillScreen(TFT_BLACK);

  drawGrayscaleBitmap(0, 0, stern, 240, 280);

  stdOverlay();

  //overlay();
  createPageName();

  lv_timer_handler();

  Serial.println("Setup done");
}


void loop() {
  buttonState = digitalRead(buttonPin);

  if (buttonState == LOW) {
    delay(200);
    if (buttonState == LOW) {
      if (page >= 4) {
        page = 1;
      } else {
        page += 1;
      }
      showPage(page);
    }
  }
}



// was immer angezeigt wird
void stdOverlay() {

  lv_obj_t *containerOben = lv_obj_create(lv_scr_act());
  lv_obj_set_size(containerOben, 250, 40);
  lv_obj_align(containerOben, LV_ALIGN_TOP_MID, 0, -5);
  lv_obj_set_style_bg_color(containerOben, lv_color_hex(0x0000ff), LV_PART_MAIN);  // Set background color to red
  lv_obj_set_style_bg_opa(containerOben, LV_OPA_COVER, LV_PART_MAIN);              // Make sure the background is fully opaque

  // Label Oben 1 in Container Oben
  lv_obj_t *labelO1 = lv_label_create(containerOben);
  lv_label_set_text(labelO1, "CLS 450 4 Matic");
  lv_obj_center(labelO1);

  const int containerUntenBreite = 150;
  // container mit Punkten unten
  lv_obj_t *containerUnten = lv_obj_create(lv_scr_act());
  lv_obj_set_size(containerUnten, containerUntenBreite, 40);
  lv_obj_align(containerUnten, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_set_style_radius(containerUnten, 20, LV_PART_MAIN);
  lv_obj_set_style_bg_color(containerUnten, lv_color_hex(0x909090), LV_PART_MAIN);  // Set background color to red
  lv_obj_set_style_bg_opa(containerUnten, LV_OPA_COVER, LV_PART_MAIN);              // Make sure the background is fully opaque

  const int numPoint = 4;
  const int sizePoint = 10;
  const int sizeSpace = 15;
  // Label in container unten
  for (int i = 0; i < numPoint; i++) {
    lv_obj_t *point = lv_btn_create(containerUnten);  // Punkt als Button erstellen
    lv_obj_set_size(point, 10, 10);
    lv_obj_align(point, LV_ALIGN_LEFT_MID, ((containerUntenBreite) / numPoint) * i, 0);
    lv_obj_set_style_radius(point, 10 / 2, LV_PART_MAIN);
    lv_obj_set_style_bg_color(point, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(point, LV_OPA_COVER, LV_PART_MAIN);
  }
}

void createPageName() {
  labelPageName = lv_label_create(lv_scr_act());
  objectArray[0] = labelPageName;
  lv_obj_set_style_transform_angle(labelPageName, 900, LV_PART_MAIN);
  lv_obj_align(labelPageName, LV_ALIGN_LEFT_MID, 20, 0);
  lv_obj_set_style_text_color(labelPageName, lv_color_hex(0x0000ff), LV_PART_MAIN);
  lv_label_set_text(labelPageName, "Seite 2");
}


void pageName(const char *seitenName) {
  lv_label_set_text(objectArray[0], seitenName);
}

lv_obj_t * barChart(int fuellstand) {
  lv_obj_t *bar = lv_bar_create(lv_scr_act());  // Create a bar object on the active screen
  lv_obj_set_size(bar, 150, 20);                // Set the size of the bar (width, height)
  lv_obj_align(bar, LV_ALIGN_CENTER, 0, 50);     // Align the bar to the center of the screen

  lv_bar_set_value(bar, fuellstand, LV_ANIM_OFF);
  return bar;
}

void clearBitmap(int sizeX, int sizeY, int posX, int posY) {
  for (int i = 0; i < sizeY; i++) {
    for (int j = 0; j < sizeX; j++) {
      tft.drawPixel(j + posX, i + posY, 0xFFFF);
    }
  }
}

lv_obj_t * pageOne() {
  stdOverlay();
  int fuellstand = 20;
  lv_obj_t * barPtr = barChart(fuellstand);
  pageName("seite 1");
  return barPtr;
}

void pageTwo() {
  stdOverlay();
  pageName("seite 2");
  drawGrayscaleBitmap(75, 93, car, 89, 95);
}

void pageThree() {
  stdOverlay();
  pageName("seite 3");
  clearBitmap(89, 95, 75, 93);
  // Fülle den Bildschirm mit Samuel
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(3);
  tft.drawString("Seite 3", 20, 20);
  tft.setTextSize(2);
  tft.drawString("AbC", 20, 50);
  tft.setTextSize(1);
  tft.drawString("size 1", 20, 80);
}

void pageFour() {
  stdOverlay();
  pageName("seite 4");
}

void showPage(int page) {

  if (page == 1) {
    pageOne();
  } else if (page == 2) {
    pageTwo();
  } else if (page == 3) {
    pageThree();
  } else if (page == 4){
    pageFour();
  }

  lv_timer_handler();
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
