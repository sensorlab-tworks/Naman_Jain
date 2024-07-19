#include "BMD101.h"

BMD101 bmd(ECG_CS, ECG_RST);

void initECG() {
    // init BMD101 using UART1 
    Serial.println("Initializing ECG Module");
    Serial2.setRX(ECG_RX);
    Serial2.setTX(ECG_TX);
    Serial2.setFIFOSize(ECG_FIFO_SIZE);
    Serial2.begin(ECG_BAUD);

    bmd.begin(&Serial2);
    bmd.enable();
    bmd.reset();

    Serial.println("ECG Module Initialized");
    Serial.println();

}

// returns raw ecg value, must be run in loop
int16_t getECG() {
    while(Serial2.available()) {
//        Serial.println(Serial2.available());
        bmd.process();
        if(bmd.data_available()) {
            int16_t value = bmd.get_raw_value();
//            Serial.println(value);
            return value;
        }
    }
    return 0;
}

void stopECG() {
    bmd.disable();
    Serial.println("ECG sensor disabled");
}
