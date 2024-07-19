#include <SD.h>
#include <SPI.h>

void initSD() {
  Serial.println("Initializing SD Card...");
  SPI.setRX(SD_MISO);
  SPI.setTX(SD_MOSI);
  SPI.setSCK(SD_SCK);

  // see if the card is present and can be initialized:
  if (!SD.begin(SD_CS)) {
    criticalError("Card failed, or not present");
  }
  Serial.println("SD Card initialized.");
  Serial.println();
}

void removeFile(String filename) {
  if (SD.exists(filename)) {
    SD.remove(filename);
    Serial.print(filename);
    Serial.println(" Removed.");
  }
}
