#include <Keypad.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#include <SoftwareSerial.h>

SoftwareSerial HC12(3, 2); // HC-12 TX Pin, HC-12 RX Pin

#define OLED_RESET 4

Adafruit_SH1106 display(OLED_RESET);

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char customKey;
//define the symbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {7,12,11,9}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {8, 6, 10}; //connect to the column pinouts of the keypad

String message = "";
String receivedMessage = "";

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup(){
  Serial.begin(9600);
  HC12.begin(9600); 
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0,0);
  display.setTextColor(WHITE);
  display.println("RF Messenger");
  display.setCursor(45,30);
  display.print("v0.1");
  display.display(); 
  delay(2000);
  clearDisplay();
}
  
void loop(){
  customKey = customKeypad.getKey();
  
  // If Arduino's HC12 rx buffer has data
  while(HC12.available()){
    receivedMessage += char(HC12.read());
    delay(200);
  }

  if(receivedMessage.length() > 0){
    Serial.println("");
    Serial.print("Message received: ");
    Serial.println(receivedMessage);
    clearDisplay();
    display.println("Received:");
    display.println(receivedMessage);
    display.display();  
    receivedMessage = "";
  }
    
  if (customKey){    
    message += customKey;
    display.print(customKey);
    display.display();
    
    // Wipe the screen after message received with two *s
    if(message.endsWith("**")){
       message = message.substring(0, message.length()-2);
       clearDisplay();
       display.print(message);
       display.display();
    }
    
    // Sending the message
    if(message.endsWith("##")){
      // Remove the last two #
      message = message.substring(0, message.length()-2);
      clearDisplay();
      display.print("Sending...");
      Serial.println("Sending: ");
      Serial.print(message);
      display.display();
      delay(200);
      clearDisplay();
      HC12.write(message.c_str());
      message = "";
    }
  }
}

void clearDisplay() {
  display.setCursor(0,0);
  display.clearDisplay();
  display.display();
}

