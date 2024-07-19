#include "FreeSerif12pt7b.h"
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "Dialog_bold_20.h"
#include "ui_bitmap.h"

#define ID_Y 127
#define HR_Y 75
#define SECONDS_Y 180

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
uint16_t row[320];

void initTFT() {
    Serial.println("Initializing TFT Display");

    SPI.setRX(TFT_MISO);
    SPI.setSCK(TFT_CLK);
    SPI.setTX(TFT_MOSI);
    tft.begin(64000000); // Can set SPI clock rate
    tft.setRotation(ROTATION);
    tft.fillScreen(BG_COLOR);
    tft.setFont(&Dialog_bold_20);

    // read diagnostics (optional but can help debug problems)
    uint8_t x = tft.readcommand8(ILI9341_RDMODE);
    Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
    x = tft.readcommand8(ILI9341_RDMADCTL);
    Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
    x = tft.readcommand8(ILI9341_RDPIXFMT);
    Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
    x = tft.readcommand8(ILI9341_RDIMGFMT);
    Serial.print("Image Format: 0x"); Serial.println(x, HEX);
    x = tft.readcommand8(ILI9341_RDSELFDIAG);
    Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX); 
}

void clearTFT(){
  tft.fillScreen(BG_COLOR);
}

void screenWelcome() {
  drawBitmap(0,0,welcome_screen,320,240);
}

void screenWaitingID() {
   drawBitmap(0, 0, waiting_for_id_screen,320,240);  
}

void screenPressToStart() {
  drawBitmap(0, 0, start_recording_screen,320,240);
}

void screenOverlayID(String id) {
  tft.setTextColor(FG_COLOR);
  tft.setCursor(150 - (5*id.length()), ID_Y);
  tft.print(id);
}

void screenScanning() {
  drawBitmap(0, 0, recording_screen,320,240);
}

void screenOverlayStats(String hr, String seconds) {

//  tt1 = micros();

    
  uint16_t hr_x = 150 - (5*hr.length());
  uint16_t seconds_x = 150 - (5*seconds.length());
  
  tft.fillRect(125, HR_Y-16, 60, 16, BG_COLOR);
  tft.fillRect(120, SECONDS_Y-16, 70, 16, BG_COLOR);
  tft.setTextColor(FG_COLOR);
  tft.setCursor(hr_x, HR_Y);
  tft.print(hr);
  tft.setCursor(seconds_x, SECONDS_Y);
  tft.print(seconds);

//    Serial.print("OOOOOOOOOO TIME: "); Serial.println(micros() - tt1);

}

void screenCompleted() {
 drawBitmap(0, 0, data_view_screen,320,240);
}

void screenSleep() {
   drawBitmap(0, 0, sleep_screen,320,240);
   
}

void screenRestarting() {
  tft.setTextColor(LINE_COLOR);

  tft.setCursor(80, 100); // Set cursor for the first line
  tft.print("RESTARTING");
  tft.setCursor(140, 140); // Set cursor for the first line
}


void screenText(String txt) {
  tft.fillRect(0, 0, 320, 22, BG_COLOR);
  tft.setCursor(10, 22);
  tft.print(txt);
}


void errorScreen(){
  tft.fillScreen(ILI9341_RED);
}

void printGraph(int16_t arr[320]) {
  for(int i = 0; i < 320; ++i) {
    tft.drawFastVLine(i, 40, 160, BG_COLOR);
    int val = map(arr[i], -32768, 32767, 40, 200);
    tft.drawPixel(i, val, FG_COLOR);
    tft.drawPixel(i, val-1, FG_COLOR);
    tft.drawPixel(i, val+1, FG_COLOR);

  }
}


void drawBitmap(int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h) {

  tft.startWrite();
  tft.setAddrWindow(x, y, w, h);
  for (int j = 0; j < h; ++j) {
    for (int i = 0; i < w; ++i) {
      row[i] = pgm_read_word(&bitmap[w*j + i]);
    }
    tft.writePixels(row, w);
  }
  tft.endWrite();
  
}
