#include <Adafruit_Debounce.h>

Adafruit_Debounce btnx(BTN_X, LOW);
Adafruit_Debounce btnr(BTN_R, LOW);

void initButtons() {
    btnx.begin();
//    pinMode(BTN_R, INPUT_PULLUP);
    btnr.begin();
}
