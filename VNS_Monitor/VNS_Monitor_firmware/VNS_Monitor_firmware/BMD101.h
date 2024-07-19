/******************************************************************************
 *
 * This is a library for the BMD101 Bio-Signal System-on-Chip.
 *
 * Its a port of Mikroe's ECG Click 4 Library, made to work with Arduino IDE 
 * while being easier to use. You can find the original library here:
 * https://github.com/MikroElektronika/ECG_4_click/tree/master/library
 *
 * You'll find example sketches which should enable you to use the library.
 *
 * If you find bugs, please inform me!
 *
 * Written by b-init
 *
 ******************************************************************************/

#ifndef _BMD101_
#define _BMD101_

#include "Arduino.h"

#define BMD101_BAUD 57600
#define BMD101_FIFO_SIZE 64
#define BMD101_RAW_BUFFER_SIZE 128

#define BMD101_SYNC_BYTE 0xAA
#define BMD101_EXCODE_BYTE 0x55
#define BMD101_SIGNAL_QUALITY_CODE_BYTE 0x02
#define BMD101_SIGNAL_OFF 0x00
#define BMD101_SIGNAL_ON 0xC8
#define BMD101_HEART_RATE_CODE_BYTE 0x03
#define BMD101_RAW_DATA_CODE_BYTE 0x80

#define PICO_UART1_TX 8
#define PICO_UART1_RX 9 

struct BMD101_data {
    uint8_t signal_quality = 0;
    uint8_t heart_rate = 0;
    int8_t raw_idx = 0;
    int16_t raw_data[BMD101_RAW_BUFFER_SIZE];
};

struct BMD101_data_buf {
    uint8_t signal_quality = 0;
    uint8_t heart_rate = 0;
    int16_t raw_value = 0;
};

class BMD101 {
    public:
        BMD101(int8_t BMD101_CS, int8_t BMD101_RST);
        // BMD101(int8_t BMD101_CS, int8_t BMD101_RST, int8_t BMD101_RX, int8_t BMD101_TX);

        void begin(); // begins with default serial channel (Serial)
        void begin(Stream* serial); // begins with referenced serial channel

        void reset();
        void enable(); // set Chip-Select pin HIGH
        void disable(); // set Chip-Select pin LOW

        int8_t available(); // returns the length of bytes available to read in serial buffer
        int8_t data_available(); // returns the number of raw data values stored

        bool sensor_on(); // returns true if BMD101's on (according to its internal sensor detection), false otherwise
        uint8_t get_heart_rate(); // returns last valid heart rate
        int16_t get_raw_value(); // returns last valid raw value and removes it from struct

        void process(); // processes one byte from the serial buffer

    private:
        int8_t _cs;
        int8_t _rst;
        Stream* _serial;
        BMD101_data_buf _data_buf;
        BMD101_data _data;

        // void process_row();

};

#endif // _BMD101_
