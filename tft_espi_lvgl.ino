#include <TFT_eSPI.h>
#include <lvgl.h>
#include <iostream>

#include "frames.h"

#define CrowPanel_24

static const uint16_t screenWidth = 240;
static const uint16_t screenHeight = 280;

const int buttonPin = 32;
int buttonState = 0;
int page = 0;


//[0] = labelPageName
//[1] = barChart
//[2] = labelRange
//[3] = containerOben
//[4] = containerUnten
//[5] = labelAvg
//[6] = labelActions
lv_obj_t *objectArray[7] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };


TFT_eSPI tft = TFT_eSPI(); /* TFT entity */

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[screenWidth * screenHeight / 13];

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
  stdOverlay(0);

  //overlay();
  createPageName();

  pageHome();

  lv_timer_handler();

  Serial.println("Setup done");
}


void loop() {
  buttonState = digitalRead(buttonPin);

  if (buttonState == LOW) {
    delay(300);
    if (buttonState == LOW) {
      if (page >= 6) {
        page = 0;
      } else {
        page += 1;
      }
      showPage(page);
    }
  }
  lv_timer_handler();
  delay(10);
}



// was immer angezeigt wird
void stdOverlay(int page) {

  lv_obj_t *containerOben = lv_obj_create(lv_scr_act());
  lv_obj_set_size(containerOben, 250, 30);
  lv_obj_align(containerOben, LV_ALIGN_TOP_MID, 0, -5);
  lv_obj_set_style_bg_color(containerOben, lv_color_hex(0x0000ff), LV_PART_MAIN);  // Set background color to red
  lv_obj_set_style_bg_opa(containerOben, LV_OPA_COVER, LV_PART_MAIN);              // Make sure the background is fully opaque

  objectArray[3] = containerOben;
  // Label Oben 1 in Container Oben
  lv_obj_t *labelO1 = lv_label_create(containerOben);
  lv_label_set_text(labelO1, "CLS 450 4 Matic");
  lv_obj_center(labelO1);

  const int containerUntenBreite = 150;
  // container mit Punkten unten
  lv_obj_t *containerUnten = lv_obj_create(lv_scr_act());
  lv_obj_set_size(containerUnten, containerUntenBreite, 20);
  lv_obj_align(containerUnten, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_set_style_radius(containerUnten, 10, LV_PART_MAIN);
  lv_obj_set_style_bg_color(containerUnten, lv_color_hex(0x909090), LV_PART_MAIN);  // Set background color to red
  lv_obj_set_style_bg_opa(containerUnten, LV_OPA_COVER, LV_PART_MAIN);              // Make sure the background is fully opaque

  objectArray[4] = containerUnten;

  const int numPoint = 7;
  const int sizePoint = 10;
  const int sizeSpace = (containerUntenBreite - 25 * (numPoint - 1) - 10) / 2;
  // Label in container unten
  for (int i = 0; i < numPoint; i++) {
    lv_obj_t *point = lv_btn_create(containerUnten);  // Punkt als Button erstellen
    lv_obj_set_size(point, 10, 10);
    lv_obj_align(point, LV_ALIGN_LEFT_MID, 10 + i * 15, 0);
    lv_obj_set_style_radius(point, 10 / 2, LV_PART_MAIN);
    if (i == page) {
      lv_obj_set_style_bg_color(point, lv_color_hex(0x000000), LV_PART_MAIN);
    } else {
      lv_obj_set_style_bg_color(point, lv_color_white(), LV_PART_MAIN);
    }
    lv_obj_set_style_bg_opa(point, LV_OPA_COVER, LV_PART_MAIN);
  }
}

void createPageName() {
  objectArray[0] = lv_label_create(lv_scr_act());
  lv_obj_set_style_transform_angle(objectArray[0], 900, LV_PART_MAIN);
  lv_obj_align(objectArray[0], LV_ALIGN_LEFT_MID, 20, 0);
  lv_obj_set_style_text_color(objectArray[0], lv_color_hex(0x0000ff), LV_PART_MAIN);
  lv_label_set_text(objectArray[0], "Seite 2");
}


void pageName(const char *seitenName) {
  lv_label_set_text(objectArray[0], seitenName);
}

lv_obj_t *barChart(int fuellstand) {
  lv_obj_t *bar = lv_bar_create(lv_scr_act());  // Create a bar object on the active screen
  lv_obj_set_size(bar, 150, 20);                // Set the size of the bar (width, height)
  lv_obj_align(bar, LV_ALIGN_CENTER, 0, 80);    // Align the bar to the center of the screen

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

void tuerenAnpassen() {
  int vorneLinks = 0;
  int vorneRechts = 0;
}
void pageHome() {
  //  drawGrayscaleBitmap(30,100,home_car,180,90);
  pageName("Home");
}

void pageOne() {
  clearBitmap(180, 90, 30, 100);
  int fuellstand = 20;
  drawGrayscaleBitmap(74, 63, tank, 114, 133);
  objectArray[1] = barChart(fuellstand);
  pageName("Seite 1");
}


void pageTwo() {
  clearBitmap(114, 133, 74, 63);
  lv_obj_del(objectArray[1]);
  objectArray[1] = NULL;
  pageName("Seite 2");
  lv_timer_handler();
  delay(5);
  drawGrayscaleBitmap(40, 50, range, 60, 60);
  drawGrayscaleBitmap(40, 150, avg, 60, 60);

  //style für label
  static lv_style_t style;
  lv_style_init(&style);
  lv_style_set_text_font(&style, &lv_font_montserrat_20);  // Set the font to Montserrat 28

  // Apply the style to the label
  //label
  lv_obj_t *labelRange = lv_label_create(lv_scr_act());
  objectArray[2] = labelRange;
  lv_obj_align(labelRange, LV_ALIGN_TOP_LEFT, 110, 70);
  lv_obj_set_style_text_color(labelRange, lv_color_hex(0x000000), LV_PART_MAIN);
  lv_label_set_text(labelRange, "hier Range");

  lv_obj_t *labelAvg = lv_label_create(lv_scr_act());
  objectArray[5] = labelAvg;
  lv_obj_align(labelAvg, LV_ALIGN_TOP_LEFT, 110, 170);
  lv_obj_set_style_text_color(labelAvg, lv_color_hex(0x000000), LV_PART_MAIN);
  lv_label_set_text(labelAvg, "hier avg");

  lv_obj_add_style(labelRange, &style, LV_PART_MAIN);
  lv_obj_add_style(labelAvg, &style, LV_PART_MAIN);
}

void pageThree() {
  clearBitmap(60, 60, 40, 50);
  clearBitmap(60, 60, 40, 150);
  if (objectArray[2] != NULL) {
    lv_obj_del(objectArray[2]);
    objectArray[2] = NULL;
    lv_obj_del(objectArray[5]);
    objectArray[5] = NULL;
  }
  lv_timer_handler();
  pageName("Seite 3");
  drawGrayscaleBitmap(60, 40, car_close, 124, 208);
  draw8BitRGBBitmap(100, 100, vorneLinks, 20, 53);
}

void pageFour() {
  clearBitmap(124, 208, 60, 40);
  pageName("Meldungen");
}

void pageActions() {
  pageName("Aktionen");
  lv_obj_t *labelActions = lv_label_create(lv_scr_act());
  objectArray[6] = labelActions;
  lv_obj_align(labelActions, LV_ALIGN_TOP_LEFT, 50, 100);
  lv_obj_set_style_text_color(labelActions, lv_color_hex(0x000000), LV_PART_MAIN);
  lv_label_set_text(labelActions, "zeige hier Aktionen an");
}

void pageDriversLog() {
  pageName("Fahrtbuch");
  lv_obj_del(objectArray[6]);
  objectArray[6] = NULL;
}

void showPage(int page) {
  lv_obj_del(objectArray[3]);
  objectArray[3] = NULL;
  lv_obj_del(objectArray[4]);
  objectArray[4] = NULL;
  stdOverlay(page);
  if (page == 0) {
    pageHome();
  } else if (page == 1) {
    pageOne();
  } else if (page == 2) {
    pageTwo();
  } else if (page == 3) {
    pageThree();
  } else if (page == 4) {
    pageFour();
  } else if (page == 5) {
    pageActions();
  } else if (page == 6) {
    pageDriversLog();
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

void draw8BitRGBBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h) {
  for (int16_t j = 0; j < h; j++) {
    for (int16_t i = 0; i < w; i++) {
      uint8_t rgb = bitmap[j * w + i];
      uint16_t color = rgb332ToRGB565(rgb);
      tft.drawPixel(x + i, y + j, color);
    }
  }
}

uint16_t rgb332ToRGB565(uint8_t rgb332) {
  // Extract the 3-3-2 bit components from the 8-bit RGB332 value
  uint8_t r = (rgb332 >> 5) & 0x07;  // Red component (3 bits)
  uint8_t g = (rgb332 >> 2) & 0x07;  // Green component (3 bits)
  uint8_t b = rgb332 & 0x03;         // Blue component (2 bits)

  // Convert 3-bit red to 5-bit red for RGB565
  uint16_t r5 = (r * 255 / 7) >> 3;  // Scale 3-bit red to 5 bits
  // Convert 3-bit green to 6-bit green for RGB565
  uint16_t g6 = (g * 255 / 7) >> 2;  // Scale 3-bit green to 6 bits
  // Convert 2-bit blue to 5-bit blue for RGB565
  uint16_t b5 = (b * 255 / 3) >> 3;  // Scale 2-bit blue to 5 bits

  // Combine the components into a 16-bit RGB565 value
  return (r5 << 11) | (g6 << 5) | b5;
}
