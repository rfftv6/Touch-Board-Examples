/*******************************************************************************

 Bare Conductive Multi Board Touch MP3 player
 ------------------------------------------
 
 secondary_board.ino - interface sending touch data via Serial1 when A0 goes high

 Connect this to a board running primary_board.ino (with a micro SD card having
 TRACK000.MP3 up to TRACK083.MP3). TX on this board must connect to RX on the 
 primary board, and A0 on this board must connect to one of A0 to A5 on the 
 primary board. The pin that A0 is connected to defines this boards position in 
 the sequence of secondary boards - A0 is the first secondary board, controlling
 TRACK012.MP3 to TRACK023.MP3, A1 is the second secondary board, controlling
 TRACK024.MP3 to TRACK035.MP3, and so on up to a total of 7 boards (1 primary 
 and 6 secondary).

 Note: all boards need power and they require their ground connections to be
 connected together. It is possible to power groups of up to four boards 
 together by commoning the 5V connection between boards, but further groups
 require an extra USB connection.
 
 Based on code by Jim Lindblom and plenty of inspiration from the Freescale 
 Semiconductor datasheets and application notes.
 
 Bare Conductive code written by Stefan Dzisiewski-Smith and Peter Krige.
 
 This work is licensed under a Creative Commons Attribution-ShareAlike 3.0 
 Unported License (CC BY-SA 3.0) http://creativecommons.org/licenses/by-sa/3.0/
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.

*******************************************************************************/

// compiler error handling
#include "Compiler_Errors.h"

// touch includes
#include <MPR121.h>
#include <Wire.h>
#define MPR121_ADDR 0x5C
#define MPR121_INT 4

const int triggerPin = A0;
boolean thisTriggerValue = false;
boolean lastTriggerValue = false;

void setup(){  
  Wire.begin();
  MPR121.begin(MPR121_ADDR);
  MPR121.setInterruptPin(MPR121_INT);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  pinMode(triggerPin, INPUT);
  digitalWrite(triggerPin, LOW); // ensure internal pullup is disabled
 
  for(int i=0; i<12; i++){
    MPR121.setTouchThreshold(i, 40);
    MPR121.setTouchThreshold(i, 20);
  }
}

void loop(){
  processInputs();
  thisTriggerValue = digitalRead(triggerPin);
  if(thisTriggerValue && !lastTriggerValue){ // rising edge triggered
    sendSerialStatus();
  }
  lastTriggerValue = thisTriggerValue;
}

void processInputs() {
  if(MPR121.touchStatusChanged()){    
    MPR121.updateTouchData();
  }
}

void sendSerialStatus(){
  Serial1.begin(9600);
  Serial1.write('T');
    for(int i=0; i<12; i++){
      if(MPR121.getTouchData(i)){
        Serial1.write('1');
      } else {
        Serial1.write('0');
      }
    }
  Serial1.end();
}
