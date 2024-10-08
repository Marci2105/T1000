// gnd -> gnd
// vcc -> 3v3
// scl -> g18
// sda -> g23
// res -> g17
// dc -> g16
// cs -> g5
// gnd -> knopf -> g32

// Bild schloss offen Schloss zu für zentralverriegelung

// Anzeige für Fenster offen oder zu

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
//[7] = main_cont
//[8] = labelFenster
lv_obj_t *objectArray[9] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };


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


void setup() {
  Serial.begin(9600); /*serial init */

  pinMode(buttonPin, INPUT_PULLUP);

  //LCD init
  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

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

  drawGrayscaleBitmap(0, 0, stern, 240, 280);
  stdOverlay(0);

  //overlay();
  createPageName();

  pageHome();
  set_dark_mode_theme();

  lv_timer_handler();
  lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);

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

void set_dark_mode_theme() {
  // Create a dark mode theme
  static lv_style_t dark_style;
  lv_style_init(&dark_style);

  // Background color for screens and containers
  lv_style_set_bg_color(&dark_style, lv_color_black());
  lv_style_set_text_color(&dark_style, lv_color_white());

  // Apply the style to the default screen and any containers
  lv_obj_add_style(lv_scr_act(), &dark_style, 0);
}

void setStyle1(lv_obj_t *container) {

  static lv_style_t style_cont;
  lv_style_init(&style_cont);
  lv_style_set_border_color(&style_cont, lv_color_hex(0x000000));
  lv_style_set_border_width(&style_cont, 0);
  lv_style_set_bg_color(&style_cont, lv_color_black());

  lv_obj_clear_flag(container, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_set_style_text_color(container, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

  lv_obj_add_style(container, &style_cont, 0);
}


// was immer angezeigt wird
void stdOverlay(int page) {

  lv_obj_t *containerOben = lv_obj_create(lv_scr_act());
  lv_obj_set_size(containerOben, 250, 30);
  lv_obj_align(containerOben, LV_ALIGN_TOP_MID, 0, -5);
  lv_obj_set_style_bg_color(containerOben, lv_color_hex(0x0000ff), LV_PART_MAIN);  // Set background color to red
  lv_obj_set_style_bg_opa(containerOben, LV_OPA_COVER, LV_PART_MAIN);              // Make sure the background is fully opaque
  setStyle1(containerOben);

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
  lv_obj_set_style_bg_color(containerUnten, lv_color_hex(0x000000), LV_PART_MAIN);  // Set background color to red
  lv_obj_set_style_bg_opa(containerUnten, LV_OPA_COVER, LV_PART_MAIN);              // Make sure the background is fully opaque
  setStyle1(containerUnten);

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
    setStyle1(point);
    if (i == page) {
      lv_obj_set_style_bg_color(point, lv_color_hex(0x0000ff), LV_PART_MAIN);
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
  setStyle1(objectArray[0]);
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
      tft.drawPixel(j + posX, i + posY, 0x0000);
    }
  }
}

void tuerenAnpassen() {
  int vorneLinks = 0;
  int vorneRechts = 0;
}
void pageHome() {
  //  drawGrayscaleBitmap(30,100,home_car,180,90);
  if (objectArray[7] != NULL) {
    lv_obj_del(objectArray[7]);
    objectArray[7] = NULL;
  }
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
  lv_label_set_text(labelRange, "hier Range");
  setStyle1(labelRange);

  lv_obj_t *labelAvg = lv_label_create(lv_scr_act());
  objectArray[5] = labelAvg;
  lv_obj_align(labelAvg, LV_ALIGN_TOP_LEFT, 110, 170);
  lv_label_set_text(labelAvg, "hier avg");
  setStyle1(labelAvg);

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

  lv_obj_t *labelFenster = lv_label_create(lv_scr_act());
  objectArray[8] = labelFenster;
  lv_obj_align(labelFenster, LV_ALIGN_TOP_LEFT, 50, 100);
  lv_label_set_text(labelFenster, "alle Fenster geschlossen");
  setStyle1(labelFenster);

}



void pageActions() {

  lv_obj_del(objectArray[8]);
  pageName("Aktionen");
  lv_obj_t *labelActions = lv_label_create(lv_scr_act());
  objectArray[6] = labelActions;
  lv_obj_align(labelActions, LV_ALIGN_TOP_LEFT, 50, 100);
  lv_label_set_text(labelActions, "zeige hier Aktionen an");
  setStyle1(labelActions);
}

void pageDriversLog() {
  pageName("Fahrtbuch");
  lv_obj_del(objectArray[6]);
  objectArray[6] = NULL;

  static lv_style_t style_cont;
  lv_style_init(&style_cont);
  lv_style_set_border_color(&style_cont, lv_color_hex(0x0000ff));  // Setzt die Umrandungsfarbe auf Rot
  lv_style_set_border_width(&style_cont, 2);                       // Setzt die Breite der Umrandung auf 3 Pixel

  static lv_style_t style_mainCont;
  lv_style_init(&style_mainCont);
  lv_style_set_border_color(&style_mainCont, lv_color_hex(0xffffff));  // Setzt die Umrandungsfarbe auf Rot
  lv_style_set_border_width(&style_mainCont, 2);                       // Setzt die Breite der Umrandung auf 3 Pixel

  lv_obj_t *main_cont = lv_obj_create(lv_scr_act());
  objectArray[7] = main_cont;
  lv_obj_set_size(main_cont, 210, 240);             // Größe anpassen
  lv_obj_add_style(main_cont, &style_mainCont, 0);  // Stil anwenden
  lv_obj_align(main_cont, LV_ALIGN_TOP_LEFT, 25, 35);
  setStyle1(main_cont);

  // Erster Container
  lv_obj_t *cont1 = lv_obj_create(main_cont);
  lv_obj_set_size(cont1, 200, 110);         // Breite und Höhe anpassen
  lv_obj_add_style(cont1, &style_cont, 0);  // Stil anwenden
  lv_obj_align(cont1, LV_ALIGN_TOP_MID, 0, -13);
  setStyle1(cont1);
  lv_obj_add_style(cont1, &style_mainCont, 0);

  // Labels für Container 1
  lv_obj_t *label1 = lv_label_create(cont1);
  lv_label_set_text(label1, "Fahrtnr: 1\nStartUhrzeit: 08:00\nEndUhrzeit: 10:00\nkm: 25\nTankfuellstand: 75%");
  setStyle1(label1);
  lv_obj_align(label1, LV_ALIGN_CENTER, 0, 0);

  // Zweiter Container
  lv_obj_t *cont2 = lv_obj_create(main_cont);
  lv_obj_set_size(cont2, 200, 110);  // Breite und Höhe anpassen
  lv_obj_align(cont2, LV_ALIGN_BOTTOM_MID, 0, -9);
  setStyle1(cont2);
  lv_obj_add_style(cont2, &style_mainCont, 0);

  // Labels für Container 2
  lv_obj_t *label2 = lv_label_create(cont2);
  lv_label_set_text(label2, "Fahrtnr: 2\nStartUhrzeit: 12:00\nEndUhrzeit: 14:00\nkm: 50\nTankfuellstand: 60%");
  lv_obj_align(label2, LV_ALIGN_CENTER, 0, 0);
  setStyle1(label2);
}

void showPage(int page) {
  objectArray[3] = NULL;
  objectArray[4] = NULL;
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

  stdOverlay(page);

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
