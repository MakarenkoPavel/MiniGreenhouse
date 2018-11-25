#include <TroykaDHT.h>
#include <Timer.h>
#include <LcdKeypad.h>

#define P_RATE 10.12

#define RELE_1 3
#define RELE_2 4

#define ON 0
#define OFF 1

DHT dht(2, DHT11);

LcdKeypad* myLcdKeypad = 0;
enum SCREEN_TYPE {SETUP, PARAM, DIF};
unsigned int curScreen = 1;
unsigned int curSetup = 0;
unsigned int brightnessCurrent = 0;
unsigned int brightnessLimit = 50;

// Implement specific LcdKeypadAdapter in order to allow receiving key press events
class MyLcdKeypadAdapter : public LcdKeypadAdapter
{
private:
  LcdKeypad* m_lcdKeypad;
public:
  MyLcdKeypadAdapter(LcdKeypad* lcdKeypad)
  : m_lcdKeypad(lcdKeypad)
  { }

  void handleKeyChanged(LcdKeypad::Key newKey)
  {
    if (m_lcdKeypad)
    {
      if (curScreen == SETUP && LcdKeypad::UP_KEY == newKey) {
        if(curSetup > 0) {
          curSetup--;
        }
      }
      else if ( curScreen == SETUP && LcdKeypad::DOWN_KEY == newKey) {
        if(curSetup < 2) {
          curSetup++;
        }
      }

      if(LcdKeypad::LEFT_KEY == newKey) {
        if(curScreen > 0) curScreen--;
      }
      else if(LcdKeypad::RIGHT_KEY == newKey) {
        if(curScreen < 2) curScreen++;
      }
    }
  }
};

void setup()
{
  Serial.begin(9600);
  dht.begin();

  myLcdKeypad = new LcdKeypad();  
  myLcdKeypad->attachAdapter(new MyLcdKeypadAdapter(myLcdKeypad));
  myLcdKeypad->setBacklight(static_cast<LcdKeypad::LcdBacklightColor>(LcdKeypad::LCDBL_WHITE));

  digitalWrite(RELE_1, OFF); 
  digitalWrite(RELE_2, OFF);   
  
  pinMode(RELE_1, OUTPUT);   
  pinMode(RELE_2, OUTPUT);  
}

void loop()
{
  dht.read();
  
  unsigned int photoSensor;
  photoSensor = analogRead(A1);

  brightnessCurrent = (int)(100 - (photoSensor / P_RATE));
  if(brightnessCurrent < brightnessLimit) {
    digitalWrite(RELE_1, ON); 
   } else {
    digitalWrite(RELE_1, OFF);
   }
  

  if(curScreen == SETUP) {
    myLcdKeypad->clear();
    myLcdKeypad->setCursor(0, 0);  
    myLcdKeypad->print("Set Brightness");
    myLcdKeypad->setCursor(0, 1); 
    myLcdKeypad->print("Set Temperature");

    myLcdKeypad->setCursor(15, curSetup); 
    myLcdKeypad->print("-");
  }
  else if(curScreen == PARAM) {
    myLcdKeypad->clear();
    switch(dht.getState()) {
    case DHT_OK:
      myLcdKeypad->setCursor(0, 0);  
      myLcdKeypad->print("Temerature:");
      
      myLcdKeypad->setCursor(11, 0); 
      myLcdKeypad->print(dht.getTemperatureC());
      myLcdKeypad->print("                ");

      myLcdKeypad->setCursor(0, 1); 
      myLcdKeypad->print("Humidity:");

      myLcdKeypad->setCursor(9, 1); 
      myLcdKeypad->print((int)dht.getHumidity());
      myLcdKeypad->print("%");
      
      break;
    case DHT_ERROR_CHECKSUM:
      myLcdKeypad->clear();
      myLcdKeypad->setCursor(0, 0);
      myLcdKeypad->print("Checksum error");
      break;
    case DHT_ERROR_TIMEOUT:
      myLcdKeypad->clear();
      myLcdKeypad->setCursor(0, 0);
      myLcdKeypad->print("Time out error");
      break;
    case DHT_ERROR_NO_REPLY:
      myLcdKeypad->clear();
      myLcdKeypad->setCursor(0, 0);
      myLcdKeypad->print("Sensor is not");
      myLcdKeypad->setCursor(0, 1);
      myLcdKeypad->print("connected");
      break;
    }
  }
  else if(curScreen == DIF) {
    myLcdKeypad->clear();
    myLcdKeypad->setCursor(0, 0);  
    myLcdKeypad->print("Brightness:");
    myLcdKeypad->setCursor(11, 0);  
    myLcdKeypad->print(brightnessCurrent);
    myLcdKeypad->print("%");
  }
   
  yield();
  delay(2000);
}
