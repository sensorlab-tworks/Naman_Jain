#define S0_INITIALIZING 0
#define S1_WAITING_ID 1
#define S2_PRESS_TO_START 2
#define S3_RECORDING 3
#define S4_VIEW 4
#define S5_SLEEP 5

struct user_info user_data;
String filename = "";
int state = S0_INITIALIZING;
int screen_state = -1;

void resetHandler() {
  interruptECG();
  state = S0_INITIALIZING;
}

void setup() {
  delay(1000);
  Serial.begin(115200);

  initButtons();
  initTFT();
  clearTFT();
//  attachInterrupt(digitalPinToInterrupt(BTN_X), resetHandler, RISING);
}

void loop() {

  
  if (state == S0_INITIALIZING) {
    if (screen_state != S0_INITIALIZING) {
        screenWelcome();
        screen_state = S0_INITIALIZING;
    }

    if (startAP()) {
      // ap started successfully
      state = S1_WAITING_ID;
    }
    else {
      // ap couldn't start
      state = S0_INITIALIZING;
    }
  }


  if (state == S1_WAITING_ID) {
    if (screen_state != S1_WAITING_ID) {
        screenWaitingID();
        screen_state = S1_WAITING_ID;
    }
    
    user_data = getID();
    if (user_data.ID == "") {
      Serial.println("ID not received");
//      state = S1k_SLEEP;
    }
    else {
      Serial.print("ID Received: ");
      Serial.println(user_data.ID);
      stopAP();
      state = S2_PRESS_TO_START;
    }
  }


  if (state == S2_PRESS_TO_START) {
    if (screen_state != S2_PRESS_TO_START) {
        screenPressToStart();
        screenOverlayID(user_data.Name);
        screen_state = S2_PRESS_TO_START;
    }

    while(1) {
      btnx.update();
      btnr.update();
      if(btnr.justPressed()) {
        state = S0_INITIALIZING;
        break;
      }
      if(btnx.justPressed()) {
        state = S3_RECORDING;
        break;
      }
      delay(10); 
    }
  }


  if (state == S3_RECORDING) {
    if (screen_state != S3_RECORDING) {
        screenScanning();
        screen_state = S3_RECORDING;
    }
    
    // attach interrupt or whatever
    filename = recordECG(&user_data, user_data.Duration);

    state = S4_VIEW;
  }


  if (state == S4_VIEW) {
    if (screen_state != S4_VIEW) {
        screenCompleted();
        screen_state = S4_VIEW;
    }

    int16_t demo_data[320];
    demoRead(filename, demo_data);
    printGraph(demo_data);

    while(1) {
      btnx.update();
      btnr.update();
      if(btnx.justPressed()) {
        state = S5_SLEEP;
        break;
      }
      if(btnr.justPressed()) {
        removeFile(filename);
        state = S2_PRESS_TO_START;
        break;
      }
      delay(10); 
    }
  }


  if (state == S5_SLEEP) {
    if (screen_state != S5_SLEEP) {
//        clearTFT();
        screenSleep();
        screen_state = S5_SLEEP;
    }

    // reset user data
    memset(&user_data, 0, sizeof(user_data));
    filename = "";

    while(1) {
      btnx.update();
      btnr.update();
      if(btnr.justPressed()) {
        state = S0_INITIALIZING;
        break;
      }
      if(btnx.justPressed()) {
        state = S0_INITIALIZING;
        break;
      }
      delay(10); 
    }
  }

}
