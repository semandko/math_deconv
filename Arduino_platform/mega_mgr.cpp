/***********************************************************************************************************
*  \copyright 
*  \file mega_mgr.cpp
*  \brief file containes methods defenition for MegaMng class
*  \author Serhii Shahan
*  \date 1.04.2023
*
*  \cond
*   Revision History
*   Date        Author          Description
*   ----------  -------------   ---------------------------------------------------------------------------
*   01.04.2023  Serhii Shahan   Initialization of methods defenition for MegaMng class
*   28.04.2023  Serhii Shahan   Implement main functionalities of Mega Platform
*
*  \endcond
***********************************************************************************************************/

#include "mega_mgr.h"
#include "HardwareSerial.h"
#include "wiring_private.h"

MegaMng::MegaMng()
{
    current_state_ = MM_STATE::IDLE;
}

MegaMng::~MegaMng()
{

}

MM_STATE MegaMng::getCurrentState(void) const
{
    return current_state_;
}

void MegaMng::setCurrentState(MM_STATE state)
{
    if (state > FIRST_STATE && state < LAST_STATE)
    {
        current_state_ = state;
    }
    else
    {
        Serial.print("ERROR: invalid state id//");
    }
}

MM_EVENT MegaMng::checkInstructionType(const quint8 (&buff)[SERIAL_BUFFER_LENGTH])
{
    MM_EVENT event;
    switch (buff[INSTRUCTION_FRAME_POS])
    {
        case CONFIG_INSTRUCTION:
            event = CONFIG_EVENT;
            break;
        case TEST_INSTRUCTION2:
            event = EVENT_2;
            break;
        // case TEST_INSTRUCTION3:
        //     event = EVENT_3;
        //     break;
        default:
            event = LAST_EVENT;
            break;   
    }
    return event; 
}

void MegaMng::executeFSM(MM_EVENT event)
{
    switch (current_state_)
    {
    case IDLE:
        switch (event)
        {
        case CONFIG_EVENT:
            setCurrentState(MM_STATE::CONFIG_APDS_MODULES);
            apdsConfiguration();
            break;
        default:
            break;
        }
        break;
    case CONFIG_APDS_MODULES:
        switch (event)
        {
        case CONFIG_SUCCESS_EVENT:
            setCurrentState(MM_STATE::READY_TO_CAPTURE_DATA);
            sendConfigSuccess();
            break;
        case CONFIG_FAILURE_EVENT:
            setCurrentState(MM_STATE::IDLE);
            sendConfigFailure();
            break;
        default:
            break;
        }
        break;
    case READY_TO_CAPTURE_DATA:
        switch (event)
        {
        case CAPTURE_DATA_APDS1_EVENT:
            setCurrentState(MM_STATE::DATA_CAPTURED);
            captureDataAPDS1();
            break;
        case CAPTURE_DATA_APDS2_EVENT:
            setCurrentState(MM_STATE::DATA_CAPTURED);
            captureDataAPDS2();
            break;
        default:
            break;
        }
        break;
    case DATA_CAPTURED:
        switch (event)
        {
        case SEND_CAPTURED_DATA_EVENT:
            setCurrentState(MM_STATE::IDLE);
            sendCapturedData();
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    Serial.print("INFO: FSM executed//");
}

void MegaMng::mainLoop(void)
{
    while (1)
    {
        // loop
        if (checkForSerialData())
        {
            if (getSerialData())
            {
                Serial.println("INFO: Serial data is valid//");

                if (!packer_uart_.isPacketStructureCorrect(serial_data_buffer_))
                {
                    Serial.println("ERROR: Packet structure invalid//");
                }
                else
                {
                    Serial.println("INFO: Packet structure valid//");
                    
                    MM_EVENT event = checkInstructionType(serial_data_buffer_);
                    
                    if (event != MM_EVENT::LAST_EVENT)
                    {
                        //****************FSM********************
                        executeFSM(event);
                        //***************************************
                    }
                    else
                    {
                        Serial.println("ERROR: Invalid event received//");
                    }
                }
            }
            else
            {
                Serial.println("ERROR: Serial data is invalid//");
            }
        }

        Serial.println("INFO: mainLoop working//");
        delay(5000);
    }
}

bool MegaMng::checkForSerialData(void)
{
    if (Serial.available() > 0)
    {
        Serial.println("INFO: Serial data available//");
        return true;
    }
    return false;
}

bool MegaMng::getSerialData(void)
{
    memset(serial_data_buffer_, 0, SERIAL_BUFFER_LENGTH);
    size_t ret = Serial.readBytes(serial_data_buffer_, SERIAL_BUFFER_LENGTH);
    if (ret != SERIAL_BUFFER_LENGTH)
    {
        memset(serial_data_buffer_, 0, SERIAL_BUFFER_LENGTH);
        return false;
    }
    return true;
}

void MegaMng::initMega(void)
{
    Serial.begin(9600);
    //Serial.println("INFO: SERIAL=9600//");
    Wire.begin();
    i2c_mux_.begin();
}

bool MegaMng::selectI2cMuxChannel(uint8_t channel)
{
    i2c_mux_.setChannel(channel);
    delay(1);
    Serial.print("Set Channel = ");
    Serial.println(channel, HEX);

    uint8_t getChannel;
    getChannel = i2c_mux_.getChannel();
    Serial.print("Get selected Channel = ");
    Serial.println(getChannel, HEX);

    if (channel == getChannel)
    {
        return true;        
    }
    return false;
}

void MegaMng::apdsConfiguration(void)
{
    bool isConfigAPDS1 = false, isConfigAPDS2 = false;
    if (selectI2cMuxChannel(CHAN1))
    {
        if (apds1_.begin())
        {
            isConfigAPDS1 = true;
            apds1_.enableColor(true);
            Serial.println("INFO: First APDS inited//");
        }
        else
        {
            Serial.println("ERROR: fail to init first APDS//");
        }
    }
    else
    {
        Serial.println("ERROR: fail to select channel CHAN1//");
    }

    if (selectI2cMuxChannel(CHAN2))
    {
        if (apds2_.begin())
        {
            isConfigAPDS2 = true;
            apds2_.enableColor(true);
            Serial.println("INFO: Second APDS inited//");
        }
        else
        {
            Serial.println("ERROR: fail to init second APDS//");
        }
    }
    else
    {
        Serial.println("ERROR: fail to select channel CHAN2//");
    }

    if (isConfigAPDS1 && isConfigAPDS2)
    {
        executeFSM(MM_EVENT::CONFIG_SUCCESS_EVENT);
    }
    else
    {
        executeFSM(MM_EVENT::CONFIG_FAILURE_EVENT);
    }
}

void MegaMng::sendConfigSuccess(void)
{
    memset(serial_data_buffer_, 0, SERIAL_BUFFER_LENGTH);
    packer_uart_.packInstruction(CONFIG_SUCCESS_INSTRUCTION, serial_data_buffer_);
    Serial.write(serial_data_buffer_, SERIAL_BUFFER_LENGTH);
    Serial.println("INFO: Success response successfully send//");
}

void MegaMng::sendConfigFailure(void)
{
    memset(serial_data_buffer_, 0, SERIAL_BUFFER_LENGTH);
    packer_uart_.packInstruction(CONFIG_FAILURE_INSTRUCTION, serial_data_buffer_);
    Serial.write(serial_data_buffer_, SERIAL_BUFFER_LENGTH);
    Serial.println("INFO: Failure response successfully send//");
}

void MegaMng::captureDataAPDS1(void)
{
    while (!apds1_.colorDataReady())
    {
      delay(5);
    }
    apds1_.getColorData(&rgbc_data_.red, &rgbc_data_.green, &rgbc_data_.blue, &rgbc_data_.clear);
    Serial.println("INFO: APDS1 data successfully cuptured//");

    executeFSM(MM_EVENT::SEND_CAPTURED_DATA_EVENT);
}

void MegaMng::captureDataAPDS2(void)
{
    while (!apds2_.colorDataReady())
    {
      delay(5);
    }
    apds2_.getColorData(&rgbc_data_.red, &rgbc_data_.green, &rgbc_data_.blue, &rgbc_data_.clear);
    Serial.println("INFO: APDS2 data successfully cuptured//");

    executeFSM(MM_EVENT::SEND_CAPTURED_DATA_EVENT);
}

void MegaMng::sendCapturedData(void)
{
    AnalogTypeDef signalTable[SIZE_OF_SIGNALS];

    memset(serial_data_buffer_, 0, SERIAL_BUFFER_LENGTH);

    packer_uart_.packInstruction(APDS_DATA_INSTRUCTION, serial_data_buffer_);
    packer_uart_.packConfig(signalTable);

    signalTable[APDS_RED].value     = rgbc_data_.red;
    signalTable[APDS_GREEN].value   = rgbc_data_.green;
    signalTable[APDS_BLUE].value    = rgbc_data_.blue;
    signalTable[APDS_CLEAR].value   = rgbc_data_.clear;

    packer_uart_.pack(signalTable[APDS_RED], serial_data_buffer_);
    packer_uart_.pack(signalTable[APDS_GREEN], serial_data_buffer_);
    packer_uart_.pack(signalTable[APDS_BLUE], serial_data_buffer_);
    packer_uart_.pack(signalTable[APDS_CLEAR], serial_data_buffer_);

    Serial.write(serial_data_buffer_, SERIAL_BUFFER_LENGTH);
    Serial.println("INFO: APDS data successfully send//");
}
