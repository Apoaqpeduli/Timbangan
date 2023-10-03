#include "config_pin.h"
#include <EEPROM.h>


void setup() {
  Serial.begin(57600); delay(10);
  Serial.println();
  Serial.println("Starting...");

  pinMode(add_tim,  INPUT_PULLUP);

  lcd.init(); 
  lcd.backlight(); 
  lcd.setCursor(1, 0); 
  lcd.print("Digital Scale "); 
  lcd.setCursor(0, 1);
  lcd.print(" 2 Ton MAX LOAD "); 

  load();

  LoadCell_1.begin();
  LoadCell_2.begin();
  LoadCell_3.begin();
  LoadCell_4.begin();

  //LoadCell_1.setReverseOutput();
  while ((loadcell_1_rdy + loadcell_2_rdy + loadcell_3_rdy + loadcell_4_rdy) < 4) { //run startup, stabilization and tare, both modules simultaniously
    if (!loadcell_1_rdy) loadcell_1_rdy = LoadCell_1.startMultiple(stabilizingtime, _tare);
    if (!loadcell_2_rdy) loadcell_2_rdy = LoadCell_2.startMultiple(stabilizingtime, _tare);
    if (!loadcell_3_rdy) loadcell_3_rdy = LoadCell_3.startMultiple(stabilizingtime, _tare);
    if (!loadcell_4_rdy) loadcell_4_rdy = LoadCell_4.startMultiple(stabilizingtime, _tare);
  }
 
  
  if (LoadCell_1.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711-1 wiring and pin designations");
    while (1);
  }
  else {
    LoadCell_1.setCalFactor(c_tim[0]); 
    //LoadCell_1.setCalFactor(9.21); 
  }

  if (LoadCell_2.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711-2 wiring and pin designations");
    while (1);
  }
  else {
    LoadCell_2.setCalFactor(c_tim[1]);
    //LoadCell_2.setCalFactor(8.11);
  }

  if (LoadCell_3.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711-3 wiring and pin designations");
    while (1);
  }
  else {
    LoadCell_3.setCalFactor(c_tim[2]);
    //LoadCell_3.setCalFactor(7.33);
  }

  if (LoadCell_4.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711-4 wiring and pin designations");
    while (1);
  }
  else {
    LoadCell_4.setCalFactor(c_tim[3]);
    //LoadCell_4.setCalFactor(8.95);
  }

}

void loop() {
  static boolean newDataReady = 0;
  const int serialPrintInterval = 1000; //increase value to slow down serial print activity

  // check for new data/start next conversion:
  if (LoadCell_1.update()) newDataReady = true;
  LoadCell_2.update();
  LoadCell_3.update();
  LoadCell_4.update();

  
  if (newDataReady) {
    if (millis() > t + serialPrintInterval) {
        d_tim[0] = LoadCell_1.getData()/1000;
        d_tim[1] = LoadCell_2.getData()/1000;
        d_tim[2] = LoadCell_3.getData()/1000;
        d_tim[3] = LoadCell_4.getData()/1000;
        zero();
        total = d_tim[0] + d_tim[1] + d_tim[2] + d_tim[3]+total_keseluruhan;
        
     
        String baris1 = String(d_tim[0]) + "Kg   " + String(d_tim[1]) + "Kg";
        String baris2 = String(d_tim[2]) + "Kg   " + String(d_tim[3]) + "Kg";
        String baris3 = "Total=" + String(total) + "Kg";

        lcd.backlight(); 
        lcd.setCursor(0, 1); 
        lcd.print(baris1); 
        lcd.setCursor(0, 2);
        lcd.print(baris2);
        lcd.setCursor(0, 3);
        lcd.print(baris3); 

        Serial.print("Load_cell output val: ");
        Serial.print(d_tim[0]);
        Serial.print("\t");
        Serial.print(d_tim[1]);
        Serial.print("\t");
        Serial.print(d_tim[2]);
        Serial.print("\t");
        Serial.println(d_tim[3]);
        newDataReady = 0;
        t = millis();
    }
  }

  int check=digitalRead(add_tim);

  if(check==0){
    Serial.println("simpan berat sebelumnya");
    total_keseluruhan=total;
    delay(1000);
  }

  // receive command from serial terminal
  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') {
        LoadCell_1.tareNoDelay();
        LoadCell_2.tareNoDelay();
        LoadCell_3.tareNoDelay();
        LoadCell_4.tareNoDelay();
    }
    else if (inByte == 'a'){
        calibrate1(); 
    } 
    else if (inByte == 'b'){
         calibrate2();
    }
    else if (inByte == 'c'){
        calibrate3(); 
    }
    else if (inByte == 'd') {
        calibrate4();
    }
    else if (inByte == 's'){
        save();
    }
    else if (inByte == 'l'){
        load();
    }
  }
  
  
  if (LoadCell_1.getTareStatus() == true) {
    Serial.println("Tare complete");
  }

}

void calibrate1() {
  Serial.println("***");
  Serial.println("Start calibration Timbangan 1:");
  Serial.println("Keadan Timbangan Tanpa Barang");
  Serial.println("Tekan 't' Untuk zero point");

  boolean _resume = false;
  while (_resume == false) {
    LoadCell_1.update();
    if (Serial.available() > 0) {
       char inByte = Serial.read();
       if (inByte == 't') LoadCell_1.tareNoDelay();
    }
    if (LoadCell_1.getTareStatus() == true) {
      Serial.println("Zero Selesai");
      _resume = true;
    }
  }

  Serial.println("Letakan Barang (Telah Ditimbang) untuk proses kalibrasi");
  Serial.println("Input berat sesuai dengan Sesuai Data yang Ditimbang");

  float known_mass = 0;
  _resume = false;
  while (_resume == false) {
    LoadCell_1.update();
    if (Serial.available() > 0) {
      known_mass = Serial.parseFloat();
      if (known_mass != 0) {
        Serial.print("Known mass is: ");
        Serial.println(known_mass);
        _resume = true;
      }
    }
  }
  LoadCell_1.update();
  LoadCell_1.refreshDataSet();
  c_tim[0] = LoadCell_1.getNewCalibration(known_mass); 

  Serial.print("Data Kalibrasi");
  Serial.print(c_tim[0]);
  _resume = true;
}

void calibrate2() {
  Serial.println("***");
  Serial.println("Start calibration Timbangan 2:");
  Serial.println("Keadan Timbangan Tanpa Barang");
  Serial.println("Tekan 't' Untuk zero point");

  boolean _resume = false;
  while (_resume == false) {
    LoadCell_2.update();
    if (Serial.available() > 0) {
       char inByte = Serial.read();
       if (inByte == 't') LoadCell_2.tareNoDelay();
    }
    if (LoadCell_2.getTareStatus() == true) {
      Serial.println("Zero Selesai");
      _resume = true;
    }
  }

  Serial.println("Letakan Barang (Telah Ditimbang) untuk proses kalibrasi");
  Serial.println("Input berat sesuai dengan Sesuai Data yang Ditimbang");

  float known_mass = 0;
  _resume = false;
  while (_resume == false) {
    LoadCell_1.update();
    if (Serial.available() > 0) {
      known_mass = Serial.parseFloat();
      if (known_mass != 0) {
        Serial.print("Known mass is: ");
        Serial.println(known_mass);
        _resume = true;
      }
    }
  }
  LoadCell_2.update();
  LoadCell_2.refreshDataSet();
  c_tim[1] = LoadCell_2.getNewCalibration(known_mass); 

  Serial.print("Data Kalibrasi");
  Serial.print(c_tim[1]);
  _resume = true;
}

void calibrate3() {
  Serial.println("***");
  Serial.println("Start calibration Timbangan 3:");
  Serial.println("Keadan Timbangan Tanpa Barang");
  Serial.println("Tekan 't' Untuk zero point");

  boolean _resume = false;
  while (_resume == false) {
    LoadCell_3.update();
    if (Serial.available() > 0) {
       char inByte = Serial.read();
       if (inByte == 't') LoadCell_3.tareNoDelay();
    }
    if (LoadCell_3.getTareStatus() == true) {
      Serial.println("Zero Selesai");
      _resume = true;
    }
  }

  Serial.println("Letakan Barang (Telah Ditimbang) untuk proses kalibrasi");
  Serial.println("Input berat sesuai dengan Sesuai Data yang Ditimbang");

  float known_mass = 0;
  _resume = false;
  while (_resume == false) {
    LoadCell_3.update();
    if (Serial.available() > 0) {
      known_mass = Serial.parseFloat();
      if (known_mass != 0) {
        Serial.print("Known mass is: ");
        Serial.println(known_mass);
        _resume = true;
      }
    }
  }
  LoadCell_3.update();
  LoadCell_3.refreshDataSet();
  c_tim[2] = LoadCell_3.getNewCalibration(known_mass); 

  Serial.print("Data Kalibrasi");
  Serial.print(c_tim[2]);
  _resume = true;
}

void calibrate4() {
  Serial.println("***");
  Serial.println("Start calibration Timbangan 4:");
  Serial.println("Keadan Timbangan Tanpa Barang");
  Serial.println("Tekan 't' Untuk zero point");

  boolean _resume = false;
  while (_resume == false) {
    LoadCell_4.update();
    if (Serial.available() > 0) {
       char inByte = Serial.read();
       if (inByte == 't') LoadCell_4.tareNoDelay();
    }
    if (LoadCell_4.getTareStatus() == true) {
      Serial.println("Zero Selesai");
      _resume = true;
    }
  }

  Serial.println("Letakan Barang (Telah Ditimbang) untuk proses kalibrasi");
  Serial.println("Input berat sesuai dengan Sesuai Data yang Ditimbang");

  float known_mass = 0;
  _resume = false;
  while (_resume == false) {
    LoadCell_4.update();
    if (Serial.available() > 0) {
      known_mass = Serial.parseFloat();
      if (known_mass != 0) {
        Serial.print("Known mass is: ");
        Serial.println(known_mass);
        _resume = true;
      }
    }
  }
  LoadCell_4.update();
  LoadCell_4.refreshDataSet();
  c_tim[3] = LoadCell_4.getNewCalibration(known_mass); 

  Serial.print("Data Kalibrasi");
  Serial.print(c_tim[3]);
  _resume = true;
}

void save(){
    int eeAddress = 0;
    varCali customVar = {
        c_tim[0],
        c_tim[1],
        c_tim[2],
        c_tim[3]
    };
    Serial.println("Save sukses");
    eeAddress += sizeof(float);
    EEPROM.put(eeAddress, customVar);
}

void load(){
    int eeAddress = sizeof(float);
    varCali customVar;
    EEPROM.get(eeAddress, customVar);

    c_tim[0]=customVar.cali1;
    c_tim[1]=customVar.cali2;
    c_tim[2]=customVar.cali3;
    c_tim[3]=customVar.cali4;

    Serial.println("Read custom object from EEPROM: ");
    for(int a=0;a<=3;a++){
        Serial.println(c_tim[a]);
    }
}

void zero(){
    if(d_tim[0]<0){
        d_tim[0]=0;
    }
    if(d_tim[1]<0){
        d_tim[1]=0;
    }
    if(d_tim[2]<0){
        d_tim[2]=0;
    }
    if(d_tim[3]<0){
        d_tim[3]=0;
    }
}
