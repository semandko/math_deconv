/***************************************************************************
  This is a 

  This sketch puts the sensor in color mode and reads the RGB and clear values.

  Designed specifically to work with the 


  Written by  for .
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include "packuart.h"
#include "mega_mgr.h"
#include "Adafruit_APDS9960.h"

//
void validSerialData(void);
void getSerialData(void);
void initAPDS9960(void);
void getAPDS9960(void);
//

quint8 buff[BUFF_LENGTH];
PackUART packerUART;
uint16_t r;
uint16_t g;
uint16_t b;
uint16_t c;
MegaMng megaMng;
Adafruit_APDS9960 apds;

void setup() 
{
  
  Serial.begin(9600);
  Serial.println("INFO: SERIAL=9600//");

//
  initAPDS9960();
//

}

void loop() 
{
  
  validSerialData(); //****************FSM********************

  getAPDS9960();
  
}


//**********************Framework***********************
void validSerialData()
{
    if (Serial.available() > 0) 
    {
        getSerialData();
        
        if (!packerUART.isPacketStructureCorrect(buff))
        {
            Serial.print("ERROR: Packet structure invalid//");
        }
        else
        {
            Serial.print("INFO: Packet structure valid//");
            
            MM_EVENT event = megaMng.checkInstructionType(buff);
            
            if (event != MM_EVENT::LAST_EVENT)
            {
                //****************FSM********************
                megaMng.executeFSM(event);
                //***************************************
            }
            else
            {
                Serial.print("ERROR: Invalid event received//");
            }
        }
    }
    
}

void getSerialData()
{  
    Serial.print("INFO: START COLLECTING DATA//");
    Serial.readBytes(buff, BUFF_LENGTH);
    Serial.print("INFO: DATA COLLECTED SUCCESSFUL//");
}

void initAPDS9960()
{
  if (!apds.begin())
  {
    Serial.println("failed to initialize device! Please check your wiring.");
  }
  else
  {
    Serial.println("Device initialized!");
  }
  
  apds.enableColor(true); // enable color sensign mode
}

void getAPDS9960()
{
  // wait for color data to be ready
  while(!apds.colorDataReady())
  {
    delay(5);
  }

  // get the data and print the different channels
  apds.getColorData(&r, &g, &b, &c);
  
  Serial.print("red: ");
  Serial.print(r);
  
  Serial.print(" green: ");
  Serial.print(g);
  
  Serial.print(" blue: ");
  Serial.print(b);
  
  Serial.print(" clear: ");
  
  Serial.println(c);
  Serial.println();
  
  delay(1000);
}
