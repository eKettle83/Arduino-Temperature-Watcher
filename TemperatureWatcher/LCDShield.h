
// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5


// read the buttons
int read_LCD_buttons()
{
  adc_key_in = analogRead(0);

  //Calibration Renaud
  //0
  //129
  //306
  //480
  //721
  //1023

  if (adc_key_in > 872) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result

  if (adc_key_in < 65)   return btnRIGHT;
  if (adc_key_in < 218)  return btnUP;
  if (adc_key_in < 393)  return btnDOWN;
  if (adc_key_in < 601)  return btnLEFT;
  if (adc_key_in < 790)  return btnSELECT;

  return btnNONE;  // when all others fail, return this...
}
