#include "packuart.h"
#include "mega_mgr.h"

void setup() 
{
  Serial.begin(9600);
}

quint8 buff[BUFF_LENGTH];

void getSerialData()
{  
    Serial.print("INFO: START COLLECTING DATA//");
    Serial.readBytes(buff, BUFF_LENGTH);
    Serial.print("INFO: DATA COLLECTED SUCCESSFUL//");
}

PackUART packerUART;
MegaMng megaMng;

void loop() 
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
                megaMng.executeFSM(event);
            }
            else
            {
                Serial.print("ERROR: Invalid event received//");
            }
        }
    }
}