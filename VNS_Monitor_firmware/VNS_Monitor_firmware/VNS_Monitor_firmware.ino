#define BTN_X 26 // X
#define BTN_R 27 // R

#define ECG_CS 11
#define ECG_RST 10
#define ECG_RX 9
#define ECG_TX 8
#define ECG_FIFO_SIZE 64
#define ECG_BAUD 57600

#define SD_MISO 4
#define SD_CS 5
#define SD_SCK 6
#define SD_MOSI 7

#define TFT_MISO 16
#define TFT_MOSI 19
#define TFT_CLK  18
#define TFT_CS   17  // Chip select control pin (GPIO 20)
#define TFT_DC   20  // Data Command control pin (GPIO 18)
#define TFT_RST  21  // Reset pin (GPIO 19)

#define ROTATION 3
//#define BG_COLOR ILI9341_BLACK
#define LINE_COLOR  ILI9341_WHITE
#define CLK_COLOR ILI9341_BLACK
#define TEXT_BG_COLOR 0xF6BC
#define BG_COLOR 0x0351
#define FG_COLOR 0xef5d

#define UPDATE_DURATION 5 // in seconds
#define RECORD_DURATION 30 // in seconds

struct user_info
{
 String ID;
 String Name;
 String Date_time;
 int Duration;  
 String Age;
 String Gender;
 String Weight;
};
