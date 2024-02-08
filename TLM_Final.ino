#include <LiquidCrystal.h>
#include <dht_nonblocking.h>
#define DHT_SENSOR_TYPE DHT_TYPE_11

#define pirPin 4
#define ledPin 6

int val = 0;
bool motionState = false; // We start with no motion detected.

static const int DHT_SENSOR_PIN = 2;
DHT_nonblocking dht_sensor( DHT_SENSOR_PIN, DHT_SENSOR_TYPE );

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

struct input {
  int c1, c2, c3;
};

bool returnOutput(input z) {
  if (z.c1 == 0 && z.c2 == 0 && z.c3 == 0) {
    return false; //room temperature
  }
  else if (z.c1 == 0 && z.c2 == 0 && z.c3 == 1) {
    return true;
  }
  else if (z.c1 == 0 && z.c2 == 1 && z.c3 == 0) {
    return true;
  }
  else {
    return false;
  }
}

int TempTruthTable[4][3] = {
  {0, 0, 0}, //default temp return 0
  {0, 0, 1}, //decrease temp return -1
  {0, 1, 0} //increase temp return 1
};

int HumTruthTable[4][3] = {
  {0, 0, 0},
  {0, 0, 1},
  {0, 1, 0}
};


void setup() {
  Serial.begin(9600);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  
  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
}


static bool measure_environment( float *temperature, float *humidity )
{
  static unsigned long measurement_timestamp = millis( );

  /* Measure once every four seconds. */
  if( millis( ) - measurement_timestamp > 3000ul )
  {
    if( dht_sensor.measure( temperature, humidity ) == true )
    {
      measurement_timestamp = millis( );
      return( true );
    }
  }

  return( false );
}

void loop() {
  input v;
  
  val = digitalRead(pirPin);
  if (val == HIGH) {
    digitalWrite(ledPin, HIGH);
  
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(0, 0);
    
    float temperature;
    float humidity;
    
    int timer = 30;
  
    /* Measure temperature and humidity.  If the functions returns
       true, then a measurement is available. */
    if( measure_environment( &temperature, &humidity ) == true )
    {
      temperature = (temperature * (9 / 5)) + 32;
      //temperature = 81;
      //temperature = 49;
      lcd.clear();
      lcd.print( "Temp = " );
      lcd.print( temperature, 1 );
      lcd.print( " F");
      lcd.setCursor(0, 1);
      lcd.print("Humi = " );
      lcd.print( humidity, 1 );
      lcd.print( " %" );
      
      delay(5000);
      double temp = temperature;

      if(temp > 80) {
        v.c1 = 0;
        v.c2 = 1;
        v.c3 = 0;
        if (returnOutput(v) == true) {
          temp -= 68;
          lcd.setCursor(0, 0);
          lcd.clear();
          lcd.print( "Temp = " );
          lcd.print( temperature, 1 );
          lcd.print( " F");
          lcd.setCursor(0,1);
          lcd.print( "Change by -" );
          lcd.print( temp, 1 );
        }
      } 
      else if(temp < 50) {
        v.c1 = 0;
        v.c2 = 0;
        v.c3 = 1;
        if (returnOutput(v) == true) {
          temp = 68 - temp;
          lcd.clear();
          lcd.print( "Temp = " );
          lcd.print( temperature, 1 );
          lcd.print( " F");
          lcd.setCursor(0,1);
          lcd.print( "Change by +" );
          lcd.print( temp, 1 );
        }
      }
      
      delay(10000);
      while (timer != 0)  {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Change to 68 F");
        lcd.setCursor(0, 1);
        lcd.print("Motion Timer: ");
        lcd.print(timer, 1);                                                          
        timer--;                                                                      //Add temp resistor back
        delay(1000);
      }
    }
  }
  else {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("No motion ");
    lcd.setCursor(0,1);
    lcd.print("detected...");
    digitalWrite(ledPin, LOW);
  }
}