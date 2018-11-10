#include <TroykaDHT.h>
#include <Timer.h>
#include <LcdKeypad.h>

#define P_RATE 10.12

DHT dht(2, DHT11);

LcdKeypad* myLcdKeypad = 0;
enum SCREEN_TYPE {SETUP, PARAM, DIF};
unsigned int curScreen = 1;

// Implement specific LcdKeypadAdapter in order to allow receiving key press events
class MyLcdKeypadAdapter : public LcdKeypadAdapter
{
private:
  LcdKeypad* m_lcdKeypad;
  unsigned char m_value;
public:
  MyLcdKeypadAdapter(LcdKeypad* lcdKeypad)
  : m_lcdKeypad(lcdKeypad)
  , m_value(5)
  { }

  void handleKeyChanged(LcdKeypad::Key newKey)
  {
    if (m_lcdKeypad)
    {
      if (LcdKeypad::UP_KEY == newKey) {
        m_value++;
      }
      else if (LcdKeypad::DOWN_KEY == newKey) {
        m_value--;
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
}

void loop()
{
  dht.read();
  
  unsigned int photoSensor;
  photoSensor = analogRead(A1);

  if(curScreen == SETUP) {
    myLcdKeypad->clear();
    myLcdKeypad->setCursor(0, 0);  
    myLcdKeypad->print("SETUP");
  }
  else if(curScreen == PARAM) {
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
    myLcdKeypad->print((int)(100 - (photoSensor / P_RATE)));
    myLcdKeypad->print("%");
  }


  yield();
  delay(2000);
}
