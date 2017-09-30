#include <LiquidCrystal.h>
#include <OneWire.h>

//  Celsius
#define bCelsius  1

//  Temperature Set
float fTempSet = 25;  //  Deg Celcius
//  Temperature Range
float fTempRange = 2;  //  Deg Celcius

//  Modes Sensor(s)
#define  ModeSensorOne      0
#define  ModeSensorsAverage 1
#define  ModeSensorsAny     2
int iModeMultiSensor = ModeSensorOne;

//  Modes Alarm for Temperature
#define  ModeWatchForTempInRange  0
#define  ModeWatchForTempOutRange 1
#define  ModeWatchForTempMin      2
#define  ModeWatchForTempMax      3

int iModeWatchTemp = ModeWatchForTempMax;

//  Mode Edition
#define  ModeEditionNone      0
#define  ModeEditionSensor    1
#define  ModeEditionWatchFor  2
#define  ModeEditionRange     3

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
OneWire  ds(11);  // on pin 10 (a 4.7K resistor is necessary)

void setup() {

  Serial.begin(9600);
  lcd.begin(16, 2);

  lcd.setCursor(0, 0);
  lcd.print("TempWatcher V1.0");
  lcd.setCursor(0, 1);
  lcd.print("Renaud D'Andre");

  lcd.clear();

}

void loop() {

  float fT; 

  while (GetNextTemp(&fT)){
    

    //Serial.print("T = ");
    Serial.println(fT);

    //  Line 1
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 0);

    String sStatus = "";
    if (GetWatchTempStatus(fT, &sStatus))
    {
      lcd.print("! " + sStatus);
    }
    else
    {

      lcd.print("Set:");
      lcd.print(String(fTempSet,0)); 
      lcd.print("+-");
      lcd.print(String(fTempRange,0));
    }

    String sTemperature = "T:";
    sTemperature += String(fT, 3);
    sTemperature += "C";
    
    //  Line 2
    lcd.setCursor(0, 1);
    lcd.print(sTemperature);

  }
}

bool GetWatchTempStatus(float fT, String *sStatus)
{

  float fLowRange = fTempSet - fTempRange;
  float fHighRange = fTempSet + fTempRange;

  bool bTisInRange = false;
  if ((fLowRange < fT) && (fT < fHighRange)) 
  {
    //  fT Is In Range
    bTisInRange = true;
    *sStatus = "Temp In Range";
  }
  else
  {
    bTisInRange = false;
    *sStatus = "Temp Out of Range";
  }

  switch(iModeWatchTemp){
    case 0: // In
      return bTisInRange; 
      break;
    case 1: //Out
      return !bTisInRange; 
      break;
  }
  
}

bool GetNextTemp(float *fT)
{

  //Serial.println("Start GetNextTemp");


  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;
  
  if ( !ds.search(addr)) {
    //Serial.println("No more addresses");
    //Serial.println();
    ds.reset_search();
    delay(250);
    return false;
  }
  
  /*
  Serial.print("ROM =");
  for( i = 0; i < 8; i++) {
    //Serial.write(' ');
    Serial.print(addr[i], HEX);
  }
  */

  if (OneWire::crc8(addr, 7) != addr[7]) {
      //Serial.println("CRC is not valid!");
      return false;
  }
  //Serial.println();
 
  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      //Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      //Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      //Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      //Serial.println("Device is not a DS18x20 family device.");
      return false;
  } 

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  /*
  Serial.print("  Data = ");
  Serial.print(present, HEX);
  Serial.print(" ");
  */

  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    //Serial.print(data[i], HEX);
    //Serial.print(" ");
  }
  //Serial.print(" CRC=");
  //Serial.print(OneWire::crc8(data, 8), HEX);
  //Serial.println();

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }

  //Serial.println();
  //Serial.print("T = ");

  if (bCelsius) {
    celsius = (float)raw / 16.0;
    //Serial.print(celsius);
    //Serial.println(" Celsius");
    *fT = celsius;
  }
  else
  {
    //  fahrenheit
    fahrenheit = celsius * 1.8 + 32.0;
    //Serial.print(fahrenheit);
    //Serial.println(" Fahrenheit");
  }

  return true;

}


void ShowTemp()
{

  Serial.println("Start ShowTemps");


  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;
  
  if ( !ds.search(addr)) {
    Serial.println("No more addresses");
    Serial.println();
    ds.reset_search();
    delay(250);
    return;
  }
  
  /*
  Serial.print("ROM =");
  for( i = 0; i < 8; i++) {
    //Serial.write(' ');
    Serial.print(addr[i], HEX);
  }
  */

  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
  //Serial.println();
 
  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      //Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      //Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      //Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      //Serial.println("Device is not a DS18x20 family device.");
      return;
  } 

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  /*
  Serial.print("  Data = ");
  Serial.print(present, HEX);
  Serial.print(" ");
  */

  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    //Serial.print(data[i], HEX);
    //Serial.print(" ");
  }
  //Serial.print(" CRC=");
  //Serial.print(OneWire::crc8(data, 8), HEX);
  //Serial.println();

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }

  //Serial.println();
  Serial.print("T = ");

  if (bCelsius) {
    celsius = (float)raw / 16.0;
    Serial.print(celsius);
    Serial.println(" Celsius");
  }
  else
  {
    //  fahrenheit
    fahrenheit = celsius * 1.8 + 32.0;
    Serial.print(fahrenheit);
    Serial.println(" Fahrenheit");
  }

}
