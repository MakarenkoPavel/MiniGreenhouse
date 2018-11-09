#include <TroykaDHT.h>
#include <Timer.h>
#include <LcdKeypad.h>

DHT dht(2, DHT11);

LcdKeypad* myLcdKeypad = 0;

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

  // Specific handleKeyChanged() method implementation - define your actions here
  void handleKeyChanged(LcdKeypad::Key newKey)
  {
    if (0 != m_lcdKeypad)
    {
      if (LcdKeypad::UP_KEY == newKey)
      {
        m_value++;
      }
      else if (LcdKeypad::DOWN_KEY == newKey)
      {
        m_value--;
      }
      //m_lcdKeypad->setCursor(6, 0);            // position the cursor at beginning of the second line
      //m_lcdKeypad->print(m_value);             // print the value on the second line of the display
      //m_lcdKeypad->print("                ");  // wipe out characters behind the printed value
    }
  }
};

void setup()
{
  // открываем последовательный порт для мониторинга действий в программе
  Serial.begin(9600);
  dht.begin();

  myLcdKeypad = new LcdKeypad();  
  myLcdKeypad->attachAdapter(new MyLcdKeypadAdapter(myLcdKeypad));
  myLcdKeypad->setBacklight(static_cast<LcdKeypad::LcdBacklightColor>(LcdKeypad::LCDBL_WHITE));
  
  myLcdKeypad->setCursor(0, 0);  
  myLcdKeypad->print("Temerature:");
  myLcdKeypad->setCursor(0, 1); 
  myLcdKeypad->print("Humidity:");
}

void loop()
{
  // считывание данных с датчика
  dht.read();
  // проверяем состояние данных
  switch(dht.getState()) {
    // всё OK
    case DHT_OK:
      // выводим показания влажности и температуры

      myLcdKeypad->setCursor(11, 0); 
      myLcdKeypad->print(dht.getTemperatureC());
      myLcdKeypad->print("                ");

      myLcdKeypad->setCursor(9, 1); 
      myLcdKeypad->print((int)dht.getHumidity());
      myLcdKeypad->print("%");
      
      break;
    // ошибка контрольной суммы
    case DHT_ERROR_CHECKSUM:
      Serial.println("Checksum error");
      break;
    // превышение времени ожидания
    case DHT_ERROR_TIMEOUT:
      Serial.println("Time out error");
      break;
    // данных нет, датчик не реагирует или отсутствует
    case DHT_ERROR_NO_REPLY:
      Serial.println("Sensor not connected");
      break;
  }

  yield();
  delay(2000);
}
