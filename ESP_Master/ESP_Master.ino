#include <Bluepad32.h>
#include <ArduinoJson.h>

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

JsonDocument doc; // packet name

const int BUFFER_SIZE = 255;
char controllerInfo[BUFFER_SIZE];

// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedController(ControllerPtr ctl) {
    bool foundEmptySlot = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
            sprintf(controllerInfo, "CALLBACK: Controller is connected, index=%d\n", i);
            Serial1.printf(controllerInfo);
            // Additionally, you can get certain gamepad properties like:
            // Model, VID, PID, BTAddr, flags, etc.
            ControllerProperties properties = ctl->getProperties();
            Serial.printf(controllerInfo, "Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id,
                           properties.product_id);
            myControllers[i] = ctl;
            foundEmptySlot = true;
            break;
        }
    }
    if (!foundEmptySlot) {
        Serial.printf("CALLBACK: Controller connected, but could not found empty slot");
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    bool foundController = false;

    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
            myControllers[i] = nullptr;
            foundController = true;
            break;
        }
    }

    if (!foundController) {
        Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
    }
}

void dumpGamepad(ControllerPtr ctl) {
    doc["index"] = ctl->index();        // Controller Index
    doc["dpad"] = ctl->dpad();         // D-pad
    doc["buttons"] = ctl->buttons();      // bitmask of pressed buttons
    doc["axisX"] = ctl->axisX();        // (-511 - 512) left X Axis
    doc["axisY"] = ctl->axisY();        // (-511 - 512) left Y axis
    doc["axisRX"] = ctl->axisRX();       // (-511 - 512) right X axis
    doc["axisRY"] = ctl->axisRY();       // (-511 - 512) right Y axis
    doc["brake"] = ctl->brake();        // (0 - 1023): brake button
    doc["throttle"] = ctl->throttle();     // (0 - 1023): throttle (AKA gas) button
    doc["miscButtons"] = ctl->miscButtons();  // bitmask of pressed "misc" buttons
    doc["gyroX"] = ctl->gyroX();        // Gyro X
    doc["gyroY"] = ctl->gyroY();        // Gyro Y
    doc["gyroZ"] = ctl->gyroZ();        // Gyro Z
    doc["accelX"] = ctl->accelX();       // Accelerometer X
    doc["accelY"] = ctl->accelY();       // Accelerometer Y
    doc["accelZ"] = ctl->accelZ();       // Accelerometer Z

    // Serialize the JSON object into a char array
    size_t bytesWritten = serializeJson(doc, controllerInfo, BUFFER_SIZE);
    controllerInfo[bytesWritten++] = '\n';

    // Check if serialization was successful
    if (bytesWritten < BUFFER_SIZE) {
      // Send the serialized JSON object over serial
      Serial.printf("bytes written: %d\n", bytesWritten);
      Serial.write(controllerInfo, bytesWritten);
      Serial.println();

      Serial1.write(controllerInfo, bytesWritten);
      Serial.println(); // Add newline for readability or termination
    } else {
      Serial.println("JSON object too large to fit in buffer");
    }
}

void dumpMouse(ControllerPtr ctl) {
    Serial.printf("idx=%d, buttons: 0x%04x, scrollWheel=0x%04x, delta X: %4d, delta Y: %4d\n",
                   ctl->index(),        // Controller Index
                   ctl->buttons(),      // bitmask of pressed buttons
                   ctl->scrollWheel(),  // Scroll Wheel
                   ctl->deltaX(),       // (-511 - 512) left X Axis
                   ctl->deltaY()        // (-511 - 512) left Y axis
    );
    sprintf(controllerInfo, "idx=%d, buttons: 0x%04x, scrollWheel=0x%04x, delta X: %4d, delta Y: %4d\n",
                   ctl->index(),        // Controller Index
                   ctl->buttons(),      // bitmask of pressed buttons
                   ctl->scrollWheel(),  // Scroll Wheel
                   ctl->deltaX(),       // (-511 - 512) left X Axis
                   ctl->deltaY()        // (-511 - 512) left Y axis
    );
    Serial1.printf(controllerInfo);
}

void dumpKeyboard(ControllerPtr ctl) {
    // TODO: Print pressed keys
    Serial.printf("idx=%d\n", ctl->index());
    sprintf(controllerInfo, "idx=%d\n", ctl->index());
    Serial1.printf(controllerInfo);
}

void dumpBalanceBoard(ControllerPtr ctl) {
    Serial.printf("idx=%d,  TL=%u, TR=%u, BL=%u, BR=%u, temperature=%d\n",
                   ctl->index(),        // Controller Index
                   ctl->topLeft(),      // top-left scale
                   ctl->topRight(),     // top-right scale
                   ctl->bottomLeft(),   // bottom-left scale
                   ctl->bottomRight(),  // bottom-right scale
                   ctl->temperature()   // temperature: used to adjust the scale value's precision
    );
    sprintf(controllerInfo, "idx=%d,  TL=%u, TR=%u, BL=%u, BR=%u, temperature=%d\n",
                   ctl->index(),        // Controller Index
                   ctl->topLeft(),      // top-left scale
                   ctl->topRight(),     // top-right scale
                   ctl->bottomLeft(),   // bottom-left scale
                   ctl->bottomRight(),  // bottom-right scale
                   ctl->temperature()   // temperature: used to adjust the scale value's precision
    );
    Serial1.printf(controllerInfo);
}

void processGamepad(ControllerPtr ctl) {
  static int led = 0;
    // There are different ways to query whether a button is pressed.
    // By query each button individually:
    //  a(), b(), x(), y(), l1(), etc...
    if (ctl->a()) {
        static int colorIdx = 0;
        // Some gamepads like DS4 and DualSense support changing the color LED.
        // It is possible to change it by calling:
        switch (colorIdx % 3) {
            case 0:
                // Red
                ctl->setColorLED(255, 0, 0);
                break;
            case 1:
                // Green
                ctl->setColorLED(0, 255, 0);
                break;
            case 2:
                // Blue
                ctl->setColorLED(0, 0, 255);
                break;
        }
        colorIdx++;
    }

   if (ctl->dpad()==DPAD_UP) {
       // Turn on the 4 LED. Each bit represents one LED.
       if (led < 4){
         led++;
         doc[]
       }
       // Some gamepads like the DS3, DualSense, Nintendo Wii, Nintendo Switch
       // support changing the "Player LEDs": those 4 LEDs that usually indicate
       // the "gamepad seat".
       // It is possible to change them by calling:
       ctl->setPlayerLEDs(led & 0x0f);
   }

   if (ctl->dpad()==DPAD_DOWN) {
       // Turn on the 4 LED. Each bit represents one LED.
       if (led > 1){
         led--;
       }
       // Some gamepads like the DS3, DualSense, Nintendo Wii, Nintendo Switch
       // support changing the "Player LEDs": those 4 LEDs that usually indicate
       // the "gamepad seat".
       // It is possible to change them by calling:
       ctl->setPlayerLEDs(led & 0x0f);
   }

   if (ctl->x()) {
        // Duration: 255 is ~2 seconds
        // force: intensity
        // Some gamepads like DS3, DS4, DualSense, Switch, Xbox One S support
        // rumble.
        // It is possible to set it by calling:
       ctl->setRumble(0xc0 /* force */, 0xc0 /* duration */);
   }

    // Another way to query controller data is by getting the buttons() function.
    // See how the different "dump*" functions dump the Controller info.
    dumpGamepad(ctl);
}

void processMouse(ControllerPtr ctl) {
    // This is just an example.
    if (ctl->scrollWheel() > 0) {
        // Do Something
    } else if (ctl->scrollWheel() < 0) {
        // Do something else
    }

    // See "dumpMouse" for possible things to query.
    dumpMouse(ctl);
}

void processKeyboard(ControllerPtr ctl) {
    // This is just an example.
    if (ctl->isKeyPressed(Keyboard_A)) {
        // Do Something
        Serial.println("Key 'A' pressed");
    }

    // Don't do "else" here.
    // Multiple keys can be pressed at the same time.
    if (ctl->isKeyPressed(Keyboard_LeftShift)) {
        // Do something else
        Serial.println("Key 'LEFT SHIFT' pressed");
    }

    // Don't do "else" here.
    // Multiple keys can be pressed at the same time.
    if (ctl->isKeyPressed(Keyboard_LeftArrow)) {
        // Do something else
        Serial.println("Key 'Left Arrow' pressed");
    }

    // See "dumpKeyboard" for possible things to query.
    dumpKeyboard(ctl);
}

void processBalanceBoard(ControllerPtr ctl) {
    // This is just an example.
    if (ctl->topLeft() > 10000) {
        // Do Something
    }

    // See "dumpBalanceBoard" for possible things to query.
    dumpBalanceBoard(ctl);
}

void processControllers() {
    for (auto myController : myControllers) {
        if (myController && myController->isConnected() && myController->hasData()) {
            if (myController->isGamepad()) {
                processGamepad(myController);
            }
        }
    }
}

void setup() {
  Serial.begin(115200);//open serial via USB to PC on default port
  Serial1.begin(115200, SERIAL_8N1, 16, 17);//open the other serial port
  Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
  const uint8_t* addr = BP32.localBdAddress();
  Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.forgetBluetoothKeys();
  BP32.enableVirtualDevice(true);
  delay(1000);
}

void loop() {  
  bool dataUpdated = BP32.update();
    if (dataUpdated) {
        processControllers();
    }
    delay(150);
}

