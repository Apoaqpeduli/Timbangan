/*
	Serial command
	Author: Trisna Julian
	

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef SERIALCMD_h
#define SERIALCMD_h

#include "wiring_private.h"


const int COMMAND_NUMBER=20;
const int COMMAND_DATA_NUMBER=COMMAND_NUMBER;

char cmdLeter[COMMAND_NUMBER]={'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?'};
int cmdIndexNow=0;


String inputString="";
String inputString1="";
String inputString2="";
String inputString3="";
String inputStringBT="";

String cmd[COMMAND_DATA_NUMBER];
float cmdData[COMMAND_DATA_NUMBER];
char cmdChar[COMMAND_DATA_NUMBER];
int cmdSource=0;
HardwareSerial *cmdSerial = &Serial; //default to serial

static void nothing(void) {
}

static volatile voidFuncPtr cmdFunc[COMMAND_NUMBER] = {
    nothing,
    nothing,
    nothing,
    nothing,
    nothing,
    nothing,
    nothing,
	nothing,
    nothing,
	nothing,
	nothing,
	nothing,
	nothing,
	nothing,
	nothing,
	nothing,
	nothing,
	nothing,
	nothing,
	nothing
};

int searchCmdLeter(char _ltr){
  for(int i=0;i<COMMAND_NUMBER;i++){
    if(cmdLeter[i]==_ltr){
      return i;
    }
  }
  return -1;
}

void attachCommand(char _ltr,void (*userFunc)(void)){
  int cmdIndex=searchCmdLeter(_ltr);
  if(cmdIndex==-1){
    cmdLeter[cmdIndexNow]=_ltr;
    cmdFunc[cmdIndexNow]=userFunc;
    cmdIndexNow++;
  }
  else{
	cmdFunc[cmdIndex]=userFunc;
  }
}


void runCmd(int _cmdIndex){
  cmdFunc[_cmdIndex]();
}

char readCmd(String _inputString){
  String buff;
  int startIndex= _inputString.indexOf('>');
  if(startIndex==-1){
	  startIndex= 0;
  }
  else{
	  startIndex+=1;
  }
  char firstCmd=_inputString.charAt(startIndex);
  _inputString=_inputString.substring(startIndex+1);
  //command data parsing
  for (int i=0;i<COMMAND_DATA_NUMBER;i++){
    buff=_inputString.substring(0,_inputString.indexOf(';'));
	cmd[i]=buff;
    cmdData[i]=buff.toFloat();
	cmdChar[i]=buff[0];
    _inputString=_inputString.substring(_inputString.indexOf(';')+1);
  }
  return firstCmd;
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') { 
      int cmdIndex= searchCmdLeter(readCmd(inputString));
	  
      if(cmdIndex!=-1){
		cmdSource=0;
		cmdSerial=&Serial;
		runCmd(cmdIndex);  
	  }
	  else if(inputString!=""){
		Serial.println("Error 0: command invalid");
	  }
	  inputString = "";
    }
    else{
      inputString += inChar;
    }
  }
}

void runCmdString(String _s){
	int cmdIndex= searchCmdLeter(readCmd(_s));
	if(cmdIndex!=-1){
		runCmd(cmdIndex);  
	}
}

//arduino mega only
#if defined(HAVE_HWSERIAL1)

void serialEvent1() {
  while (Serial1.available()) {
    char inChar = (char)Serial1.read();
    if (inChar == '\n') { 
      int cmdIndex= searchCmdLeter(readCmd(inputString1));
	  
      if(cmdIndex!=-1){
		cmdSource=1;
		cmdSerial=&Serial1;
		runCmd(cmdIndex);  
	  }
	  else if (inputString1!=""){
		Serial1.println("Error 0: command invalid");
	  }
	  inputString1 = "";
    }
    else{
      inputString1 += inChar;
    }
  }
}
#endif
//arduino mega only
#if defined(HAVE_HWSERIAL2)
void serialEvent2() {
  while (Serial2.available()) {
    char inChar = (char)Serial2.read();
    if (inChar == '\n') { 
      int cmdIndex= searchCmdLeter(readCmd(inputString2));
	  
      if(cmdIndex!=-1){
		cmdSource=2;
		cmdSerial=&Serial2;
		runCmd(cmdIndex);  
	  }
	  else if (inputString2 != ""){
		Serial2.println("Error 0: command invalid");
	  }
	  inputString2 = "";
    }
    else{
      inputString2 += inChar;
    }
  }
}

void serialEvent3(){
	while (Serial3.available()) {
    char inChar = (char)Serial3.read();
    if (inChar == '\n') { 
      int cmdIndex= searchCmdLeter(readCmd(inputString3));
	  
      if(cmdIndex!=-1){
		cmdSource=3;
		cmdSerial=&Serial3;
		runCmd(cmdIndex);  
	  }
	  else if (inputString3 != ""){
		Serial3.println("Error 0: command invalid");
	  }
	  inputString3 = "";
    }
    else{
      inputString3 += inChar;
    }
  }
}
#endif

//arduino due only
#if defined(__SAM3X8E__)
void serialEvent1() {
  while (Serial1.available()) {
    char inChar = (char)Serial1.read();
    if (inChar == '\n') { 
      int cmdIndex= searchCmdLeter(readCmd(inputString1));
	  
      if(cmdIndex!=-1){
		cmdSource=1;
		cmdSerial=&Serial1;
		runCmd(cmdIndex);
	  }
	  else if (inputString1 != ""){
		Serial1.println("Error 0: command invalid");
	  }
	  inputString1 = "";
    }
    else{
      inputString1 += inChar;
    }
  }
}

void serialEvent2() {
  while (Serial2.available()) {
    char inChar = (char)Serial2.read();
    if (inChar == '\n') { 
      int cmdIndex= searchCmdLeter(readCmd(inputString2));
	  
      if(cmdIndex!=-1){
		cmdSource=2;
		cmdSerial=&Serial2;
		runCmd(cmdIndex);  
	  }
	  else if (inputString2 != ""){
		Serial2.println("Error 0: command invalid");
	  }
	  inputString2 = "";
    }
    else{
      inputString2 += inChar;
    }
  }
}

#endif

#ifdef _BLUETOOTH_SERIAL_H_
BluetoothSerial SerialBT;
void serialBTEvent(){
	while (SerialBT.available()) {
    char inChar = (char)SerialBT.read();
    if (inChar == '\n') { 
      int cmdIndex= searchCmdLeter(readCmd(inputStringBT));
	  
      if(cmdIndex!=-1){
		cmdSource=3;
		cmdSerial=&SerialBT;
		runCmd(cmdIndex);  
	  }
	  else if (inputStringBT != ""){
		SerialBT.println("Error 0: command invalid");
	  }
	  inputStringBT = "";
    }
    else{
      inputStringBT += inChar;
    }
  }
}

#endif

#if defined(ESP32)
void serialEvent2() {
  while (Serial2.available()) {
    char inChar = (char)Serial2.read();
    if (inChar == '\n') { 
      int cmdIndex= searchCmdLeter(readCmd(inputString2));
	  
      if(cmdIndex!=-1){
		cmdSource=2;
		cmdSerial=&Serial2;
		runCmd(cmdIndex);  
	  }
	  else if (inputString2 != ""){
		Serial2.println("Error 0: command invalid");
	  }
	  inputString2 = "";
    }
    else{
      inputString2 += inChar;
    }
  }
}
#endif

#endif
