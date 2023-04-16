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
#include "PWFusion_TCA9548A.h"

//
void validSerialData(void);
void getSerialData(void);
void initTCA9548A(void);
void setTCA9548A(void);
void tcaSelect(uint8_t i);
void scanerTCA9548A(void);
void initAPDS9960(void);
void getAPDS9960(void);
//

quint8 buff[BUFF_LENGTH];
PackUART packerUART;
uint16_t r;
uint16_t g;
uint16_t b;
uint16_t c;
uint16_t luxVal;
MegaMng megaMng;
Adafruit_APDS9960 apds;
TCA9548A i2cMux;



//**************************************************************************
//**************************************************************************
//**************************************************************************
void setup() 
{
  
  Serial.begin(9600);
  Serial.println("INFO: SERIAL=9600//");
  
  initTCA9548A();
  initAPDS9960();

}

void loop() 
{
  
  validSerialData(); //****************FSM********************
  getAPDS9960();
  
}
//**************************************************************************
//**************************************************************************
//**************************************************************************


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


void initTCA9548A()
{
  // Initialize I2C multiplexor
  i2cMux.begin();
  setTCA9548A();
}

void setTCA9548A()
{
  i2cMux.setChannel(CHAN0); 
  Serial.println("Channel 0 selected"); 
  delay(500);
  Serial.println("Get selected Channel"); 
  Serial.println(i2cMux.getChannel()); 

  //i2cMux.setChannel(CHAN0 | CHAN4); 
  //Serial.println("Channels 0 and 4 selected"); 
  //delay(500);

  //i2cMux.setChannel(CHAN_NONE); 
  //Serial.println("No channels selected"); 
  //delay(500);
}

void tcaSelect(uint8_t i)
{
  if (i > 7) return;
 
  Wire.beginTransmission(BASE_ADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

void scanerTCA9548A()
{
    Serial.println("\nTCAScanner ready!");
    
    for (uint8_t t = 0; t < 8; t++)
	{
      tcaSelect(t);
      Serial.print("TCA Port #");
	  Serial.println(t);

      for (uint8_t addr = 0; addr <= 127; addr++)
	  {
        if (addr == BASE_ADDR) continue;

        Wire.beginTransmission(addr);
        if (!Wire.endTransmission())
		{
          Serial.print("Found I2C 0x");
		  Serial.println(addr,HEX);
        }
      }
    }
    Serial.println("\ndone");
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
  
  // configuration
  //
  //apds.setADCIntegrationTime(uint16_t iTimeMS); // defaul (ms) =  10 // 2.78ms = 1 cycle (low sensitivity) // 27.8ms = 10 cycles // 200ms = 72 cycles // 712ms = 256 cycles (max sensitivity) 
  //float iTimeMS = apds.getADCIntegrationTime();
  //apds.setADCGain(APDS9960_AGAIN_4X); // apds9960AGain_t: // APDS9960_AGAIN_1X // APDS9960_AGAIN_4X // APDS9960_AGAIN_16X // APDS9960_AGAIN_64X
  //apds9960AGain_t gain = apds.getADCGain();
  
  apds.enableColor(true); // enable color sensign mode
}

void getAPDS9960()
{
  // wait for color data to be ready
  while (!apds.colorDataReady())
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
  
  // get the data and print the illuminance in LUX
  luxVal = apds.calculateLux(r, g, b);
  Serial.print(" illuminance in LUX: ");
  Serial.println(luxVal);
  Serial.println();
  
  delay(1000);
}
