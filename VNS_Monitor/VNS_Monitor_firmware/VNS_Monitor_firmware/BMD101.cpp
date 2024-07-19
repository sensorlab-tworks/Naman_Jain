#include "BMD101.h"

BMD101::BMD101(int8_t cs, int8_t rst) {
    _cs = cs;
    _rst = rst;
}

void BMD101::begin() {
    return this->begin(&Serial);
}

void BMD101::begin(Stream* serial) {

    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH);

    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);

    _serial = serial;
    // _serial.setFIFOSize(BMD101_FIFO_SIZE);
    // _serial->begin(BMD101_BAUD); 

}

void BMD101::reset() {
    digitalWrite(_rst, LOW);
    delay(200);
    digitalWrite(_rst, HIGH);
    delay(200);
}

void BMD101::enable() {
    digitalWrite(_cs, HIGH);
}

void BMD101::disable() {
    digitalWrite(_cs, LOW);
}

int8_t BMD101::available() {
    return _serial->available();
}

int8_t BMD101::data_available() {
    return _data.raw_idx;
}

bool BMD101::sensor_on() {
    return _data.signal_quality;
}

uint8_t BMD101::get_heart_rate() {
    return _data.heart_rate;
}

int16_t BMD101::get_raw_value() {
    int8_t idx = --(_data.raw_idx);
    return _data.raw_data[idx];
}

void BMD101::process() {
    static uint8_t rx_buff[ 256 ];
    static uint8_t rx_cnt = 0;
    static uint8_t rx_idx;
    static uint8_t payload_size;
    static uint8_t row_check = 0;
    static uint8_t op_code;
    static uint8_t row_size;
    static uint8_t row_size_check;
    static uint8_t checksum = 0;
    static uint8_t row_cnt = 0;
    uint8_t rx_dat;
    
    rx_dat = _serial->read();
    
    // processing buffer byte by byte
    if ((rx_cnt == 0) && (rx_dat != BMD101_SYNC_BYTE)) {
        rx_cnt = 0;
        
        return;
    }
    else if ((rx_cnt == 1) && (rx_dat != BMD101_SYNC_BYTE)) {
        rx_cnt = 0;
        
        return;
    }
    else if (rx_cnt == 2) {
        payload_size = rx_dat;
        rx_cnt++;
        row_check = 1;
        
        return;
    }
    else if (rx_cnt > 2) {
        if (rx_cnt - 3 < payload_size) {
            if (rx_dat == BMD101_EXCODE_BYTE) {
                row_check = 1;
                checksum += rx_dat;
                rx_cnt++;
                
                return;
            }
            if (row_check == 1) {
                op_code = rx_dat;
                row_check = 0;
                checksum += rx_dat;
                
                if ((op_code == BMD101_SIGNAL_QUALITY_CODE_BYTE) || (op_code == BMD101_HEART_RATE_CODE_BYTE)) {
                    row_size = 1;
                    row_size_check = 0;
                }
                else if (op_code == BMD101_RAW_DATA_CODE_BYTE) {
                    row_size_check = 1;
                }
                else {
                    rx_cnt = 0;
                    checksum = 0;
                    
                    return;
                }
                
                rx_idx = 0;
                rx_cnt++;
                
                return;
            }
            
            if (row_size_check == 1) {
                row_size = rx_dat;
                row_size_check = 0;
                checksum += rx_dat;
                rx_cnt++;
                
                return;
            }
            
            if (rx_idx < row_size) {
                rx_buff[ rx_idx ] = rx_dat;
                rx_idx++;
                checksum += rx_dat;
                
                if (rx_idx == row_size) {
                    
                    // entire row read, store data in buffer struct
                    if(op_code == BMD101_SIGNAL_QUALITY_CODE_BYTE) {
                        _data_buf.signal_quality = rx_buff[0];
                    }
                    else if(op_code == BMD101_HEART_RATE_CODE_BYTE) {
                        _data_buf.heart_rate = rx_buff[0];
                    }
                    else if(op_code == BMD101_RAW_DATA_CODE_BYTE) {
                        _data_buf.raw_value = (rx_buff[0] << 8) + rx_buff[1];
                    }

                    row_cnt++;
                    row_check = 1;
                }
                
                rx_cnt++;
                
                return;
            }
        }
        else {
            checksum = ~checksum;
            
            if (checksum == rx_dat) {
                // data valid, updata valid data struct from buffer data
                _data.signal_quality = _data_buf.signal_quality;
                _data.heart_rate = _data_buf.heart_rate;
                if(_data.raw_idx == BMD101_RAW_BUFFER_SIZE) {
                    // raw data buffer full! reset index
                    _data.raw_idx = 0;
                }
                _data.raw_data[_data.raw_idx] = _data_buf.raw_value;
                _data.raw_idx++;
            }
            else {
                // data invalid
                // dont update data
            }
            
            checksum = 0;
            row_cnt = 0;
            rx_cnt = 0;
            rx_idx = 0;
            
            return;
        }
    }
    
    rx_cnt++;
}




// void BMD101::process_row(uint8_t *op_code, uint8_t *row_size, uint8_t *rx_buff)
// {
//     uint8_t idx_cnt;

//     if(*op_code == BMD101_SIGNAL_QUALITY_CODE_BYTE) {
//         _data_buf.signal_quality = rx_buff[0];
//     }
//     else if(*opcode == BMD101_HEART_RATE_CODE_BYTE) {
//         _data_buf.heart_rate = rx_buff[0];
//     }
//     else if(*opcode == BMD101_RAW_DATA_CODE_BYTE) {
//         _data_buf.raw_value = (rx_buff[0] << 8) + rx_buff[1];
//     }
// }
