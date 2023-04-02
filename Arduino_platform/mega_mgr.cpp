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
*   ---------   -------------   ---------------------------------------------------------------------------
*   1.04.2023   Serhii Shahan   Initialization of methods defenition for MegaMng class
*
*  \endcond
***********************************************************************************************************/

#include "mega_mgr.h"
#include "HardwareSerial.h"

static MM_TABLE megaFSMtable[] =
{   //current state     event                           next state                      funcPtr
    {MM_STATE::IDLE,    MM_EVENT::CONFIG_EVENT,         MM_STATE::CONFIG_HW_MODULES,    &MegaMng::hardwareConfiguration}
};

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

MM_EVENT MegaMng::checkInstructionType(const quint8 (&buff)[BUFF_LENGTH])
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

void MegaMng::hardwareConfiguration(void)
{
    // To be implemented later
    Serial.print("INFO: Start configuration HW modules//");
}

void MegaMng::executeFSM(MM_EVENT event)
{
    quint8 megaFSMtableSize = (sizeof(megaFSMtable) / sizeof(MM_TABLE));
    int i;

    for (i = 0; i < megaFSMtableSize; ++i)
    {
        if (megaFSMtable[i].State == getCurrentState() && 
            megaFSMtable[i].event == event)
        {
            setCurrentState(megaFSMtable[i].nextState);
            Serial.print("INFO: Switched State to CONFIG_HW_MODULES//");
            megaFSMtable[i].funcPtr();
            break;
        }
    }

    if (i == megaFSMtableSize)
    {
        Serial.print("WARN: State transition not found//");
    }
    Serial.print("INFO: FSM executed//");
}