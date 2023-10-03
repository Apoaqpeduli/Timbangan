#include <Wire.h>
#include "src/hx711/HX711_ADC.h"
#include "src/liib/serialCmd.h"
#include "src/LiquidCrystal_I2C/LiquidCrystal_I2C.h"
#include <EEPROM.h>

struct varCali
{
    float cali1;
    float cali2;
    float cali3;
    float cali4;
    /* data */
};

float c_tim[4]={ 9.21, 8.11, 7.33, 8.95};
float d_tim[4];
unsigned long t = 0;
float total;
float total_keseluruhan;

byte loadcell_1_rdy = 0;
byte loadcell_2_rdy = 0;
byte loadcell_3_rdy = 0;
byte loadcell_4_rdy = 0;

#define dk_1 3
#define sck_1 2

#define dk_2 5
#define sck_2 4

#define dk_3 7
#define sck_3 6

#define dk_4 9
#define sck_4 8

#define add_tim 10

unsigned long stabilizingtime = 2000;
boolean _tare = true;

LiquidCrystal_I2C lcd(0x27,20,4);

HX711_ADC LoadCell_1(dk_1, sck_1);
HX711_ADC LoadCell_2(dk_2, sck_2);
HX711_ADC LoadCell_3(dk_3, sck_3);
HX711_ADC LoadCell_4(dk_4, sck_4);
