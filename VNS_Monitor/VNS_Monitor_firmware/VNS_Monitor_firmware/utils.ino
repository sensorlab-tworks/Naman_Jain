unsigned long tt2, tt3;
int data_count = 0;

String temp_filename = "";
File logFile;

void interruptECG(){
  if(logFile){
    logFile.close();
  }
  stopECG();
  Serial.println("ECG Recording Interrupted");
}

String recordECG(user_info* user_data, int duration_s = 300) {
    initSD();
    initECG();

    String id = user_data->ID;
    String date_time = user_data->Date_time;

    String filename = "VNS_ECG_Data/" + id + '/' + date_time + '_' + id;
    
    int file_index = 1;
    temp_filename = filename + "_0";
    while(SD.exists(temp_filename)) {
        temp_filename = filename + '_' + file_index;
        file_index++;
    }
    filename = temp_filename + ".txt";

    logFile = SD.open(filename, FILE_WRITE);
    data_count = 0;

    if (logFile) {
        Serial.println("All modules initalized successfully");
        Serial.print("Logging data for ");
        Serial.print(duration_s);
        Serial.println(" seconds");
        Serial.print("ID: ");
        Serial.println(id);

        logFile.println(user_data->ID);
        logFile.println(user_data->Date_time);
        logFile.println(user_data->Name);
        logFile.println(user_data->Age);
        logFile.println(user_data->Gender);
        logFile.println(user_data->Weight);
        
        unsigned long start_time = millis();
        unsigned long t1 = millis();
        // keep recording for duration_s
        while(millis() - start_time <= duration_s*1000) {
        
            while(Serial2.available()) {
//                Serial.println(Serial2.available());
                bmd.process();
                if(bmd.data_available()) {
                    int16_t value = bmd.get_raw_value();
                    ++data_count;
                    logFile.println(value);
                }
            }
            
            if(millis() - t1 >= UPDATE_DURATION*1000) {
                uint8_t hr = bmd.get_heart_rate();
                uint16_t seconds = duration_s - ((millis() - start_time)/1000);
                Serial.print("Heartrate: ");
                Serial.print(hr);
                Serial.print(" | ");
                Serial.print("Time Remaining: ");
                Serial.print(seconds);
                Serial.println(" seconds");
                screenOverlayStats(String(hr), String(seconds));
                    
                t1 = millis(); 
            }

            if (!digitalRead(BTN_X)) {
              break;
            }
        }

        logFile.close();
        stopECG();
        Serial.println("Recording Completed");
        Serial.print("Total data points took: ");
        Serial.println(data_count);

    }
    else {
        stopECG();
        criticalError("Unable to open Log File. Recording Terminated");
    }

    return filename;
}

void demoRead(String filename, int16_t *demo_data){
  File dataFile = SD.open(filename);

  if(dataFile){
    for(int i = 0; i < 2500; ++i){
      dataFile.readStringUntil('\n');  
    }
    for(int i = 0; i < 320; ++i) {
      int16_t val = dataFile.readStringUntil('\n').toInt();
      demo_data[i] = val;
    }
    dataFile.close();
  }
}

void criticalError(String error) {
  Serial.print("ERROR: ");
  Serial.println(error);
  clearTFT();
  screenText("ERROR: " + error);
  while(1);
//   exit(0);
  return;
}
